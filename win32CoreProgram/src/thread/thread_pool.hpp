/****************************************************************************
**
** Copyright (C) 2023 635672377@qq.com
** All rights reserved.
**
****************************************************************************/

// 1����η�װһ��any����
// 2���̼߳����ͨ��
// 3�����ܵĺ�����η�װ

#ifndef thread_pool_hpp
#define thread_pool_hpp

#include <mutex>
#include <functional>
#include <queue>
#include <future>
#include <thread>
#include <memory>
#include <chrono>
#include <iostream>
#include <unordered_map>

namespace ThreadPool {
// any����
class Any {
 public:
  Any() = default;
  ~Any() = default;
  Any(const Any&) = delete;
  Any& operator=(const Any&) = delete;
  /* Any(Any&& any) = default;
   Any& operator=(Any&&) = default;*/

  template <typename T>
  Any(T data) : base_(std::make_unique<Derive<T>>(data)) {}

  template <typename T>
  T cast_() {
    // ע������ķ�װ���ǵ㾦֮��
    Derive<T>* pd = dynamic_cast<Derive<T>*>(base_.get());
    if (pd == nullptr) {
      throw "type is unmatch";
    }

    return pd->data_;
  }

 private:
  class Base {
   public:
    virtual ~Base() = default;
  };

  template <typename T>
  class Derive : public Base {
   public:
    Derive(T data) : data_(data) {}
    T data_;
  };

 private:
  std::unique_ptr<Base> base_;
};

void test_any() {
  Any any_1(10);
  int value = any_1.cast_<int>();

  Any any_2(std::string("linux"));
  std::string str = any_2.cast_<std::string>();
}

// �����������
const int TASK_MAX_THRESHHOLD = 2;
// ����߳���������
const int THREAD_MAX_THRESHHOLD = 1024;
// ����ʱ��60s
const int THREAD_MAX_IDLE_TIME = 60;

enum class PoolMode { MODE_FIXED, MODE_CACHED };

// 1��������η�װ����λ�ȡ����ֵ
// 2���߳�����˳�
// 3����������
class Thread {
 public:
  using ThreadFunc = std::function<void(int)>;

  Thread(ThreadFunc func) : func_(func), threadId_(generateId_++) {}

  ~Thread() = default;

  void start() {
    std::thread thr(func_, threadId_);
    thr.detach();
  }

  int getId() const { return threadId_; }

 private:
  ThreadFunc func_;
  static int generateId_;
  int threadId_;
};
int Thread::generateId_ = 0;

class ThreadPool {
 public:
  ThreadPool()
      : initThreadSize_(0),
        threadSizeThreshHold_(THREAD_MAX_THRESHHOLD),
        curThreadSize_(0),
        idleThreadSize_(0),
        taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD),
        poolMode_(PoolMode::MODE_FIXED),
        isPoolRuning_(false) {}

  ~ThreadPool() {
    isPoolRuning_ = false;

    // �ȴ��̳߳����е��̷߳��أ�����״̬������ || ����ִ������
    std::unique_lock<std::mutex> lock(taskQueMtx_);
    notEmpty_.notify_all();
    exitCond_.wait(lock, [&]() -> bool { return threads_.size() == 0; });
  }

  ThreadPool(ThreadPool&&) = delete;
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  void setMode(PoolMode mode) {
    if (checkRunningState()) {
      return;
    }

    poolMode_ = mode;
  }

  void setTaskQueMaxThreshHold(int taskThreshHold) {
    if (checkRunningState()) {
      return;
    }
    taskQueMaxThreshHold_ = taskThreshHold;
  }

  void setThreadSizeThreshHold(int threshSizeHold) {
    if (checkRunningState()) {
      return;
    }

    if (poolMode_ == PoolMode::MODE_CACHED) {
      threadSizeThreshHold_ = threshSizeHold;
    }
  }

  // 1���ɱ�ģ�����
  // 2�������۵�
  // 3������ֵ����
  // 4����ʱ����
  template <typename Func, typename... Args>
  auto submitTask(Func&& func, Args&&... args)
      -> std::future<decltype(func(args...))> {
    // ������񣬷��뵽���������
    using RType = decltype(func(args...));
    auto task = std::make_shared<std::packaged_task<RType()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
    std::future<RType> result = task->get_future();

    // ��ȡ��
    std::unique_lock<std::mutex> lock(taskQueMtx_);
    // �û��ύ����������������ܳ���1s������ʧ�ܷ���
    if (!notFull_.wait_for(lock, std::chrono::seconds(1), [&]() -> bool {
          return taskSize_ < (size_t)taskQueMaxThreshHold_;
        })) {
      // ��ʾ�ȴ�1s�������������������
      std::cerr << "task queue is full, submit task fail" << std::endl;
      // ����һ���յ����񣬷���
      auto task = std::make_shared<std::packaged_task<RType()>>(
          []() -> RType { return RType(); });
      // ִ������
      (*task)();
      return task->get_future();
    }

    // ���task����δ��, ����lambda���ʽ
    taskQue_.emplace([task] { (*task)(); });
    taskSize_++;

    // ֪ͨ��������в�Ϊ�գ����Խ�����������
    notEmpty_.notify_all();

    // cachedģʽ���������С����ĳ�������Ҫ�������������Ϳ����߳��������ж��Ƿ���Ҫ�ٴ����߳�
    if (poolMode_ == PoolMode::MODE_CACHED && taskSize_ > idleThreadSize_ &&
        curThreadSize_ < threadSizeThreshHold_) {
      std::cout << ">>>> create new thread..." << std::endl;

      // �����µ��߳�
      auto ptr = std::make_unique<Thread>(
          std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
      int threadId = ptr->getId();
      threads_.emplace(threadId, std::move(ptr));
      // �����߳�
      threads_[threadId]->start();
      // �޸��߳��й�����
      curThreadSize_++;
      idleThreadSize_++;
    }
    return result;
  }

  // �����߳�
  void start(int initThreadSize = std::thread::hardware_concurrency()) {
    // ����״̬
    isPoolRuning_ = true;

    // ��¼��ʼ�̸߳���
    initThreadSize_ = initThreadSize;
    curThreadSize_ = initThreadSize;

    // �����̶߳���
    for (int i = 0; i < initThreadSize_; ++i) {
      auto ptr = std::make_unique<Thread>(
          std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
      int threadId = ptr->getId();
      threads_.emplace(threadId, std::move(ptr));
    }

    for (int i = 0; i < initThreadSize_; ++i) {
      // TODO: ������񲻶�
      threads_[i]->start();
      idleThreadSize_++;
    }
  }

  bool checkRunningState() { return isPoolRuning_; }

 private:
  void threadFunc(int threadId) {
    auto lastTime = std::chrono::high_resolution_clock().now();

    for (;;) {
      Task task;
      {
        // �Ȼ�ȡ��
        std::unique_lock<std::mutex> lock(taskQueMtx_);
        std::cout << "tid:" << std::this_thread::get_id() << "���Ի�ȡ����"
                  << std::endl;

        // cachedģʽ�£��п����Ѿ������˺ܶ���̡߳����ǿ���ʱ�䳬��60s��Ӧ�ðѶ�����߳̽������յ�
        // ����������1������initThreadSize_ 2������ʱ�䳬��60s
        // ����ж��ǳ�ʱ���ػ���ִ�����񷵻أ�
        while (taskQue_.size() == 0) {
          // �̳߳��˳�
          if (!isPoolRuning_) {
            threads_.erase(threadId);
            std::cout << "threadid:" << std::this_thread::get_id() << " exit"
                      << std::endl;
            exitCond_.notify_all();
            // �̺߳�������
            return;
          }

          if (poolMode_ == PoolMode::MODE_CACHED) {
            if (std::cv_status::timeout ==
                notEmpty_.wait_for(lock, std::chrono::seconds(1))) {
              auto now = std::chrono::high_resolution_clock().now();
              auto dur = std::chrono::duration_cast<std::chrono::seconds>(
                  now - lastTime);
              if (dur.count() >= THREAD_MAX_IDLE_TIME &&
                  curThreadSize_ > initThreadSize_) {
                // ���յ�ǰ�߳�
                threads_.erase(threadId);
                curThreadSize_--;
                idleThreadSize_--;
                std::cout << "threadid:" << std::this_thread::get_id()
                          << " exit" << std::endl;
                return;
              }
            }
          } else {
            notEmpty_.wait(lock);
          }
        }  // while

        idleThreadSize_--;
        std::cout << "tid:" << std::this_thread::get_id() << "��ȡ����ɹ�"
                  << std::endl;
        // ȡ������
        task = taskQue_.front();
        taskQue_.pop();
        taskSize_--;

        // �����Ȼ��ʣ�����񣬼���֪ͨ�����߳�ִ������
        if (taskQue_.size() > 0) {
          notEmpty_.notify_all();
        }

        // ȡ��һ�����񣬽���֪ͨ��֪ͨ���Լ����ύ��������
        notFull_.notify_all();
      }  // �˳���ǰ�����򣬾ͻ��Զ��ͷ���

      if (task != nullptr) {
        task();
      }

      // �����Ѿ�ִ�����
      idleThreadSize_++;
      // �����߳�ִ���������ʱ��
      lastTime = std::chrono::high_resolution_clock().now();
    }  // for
  }

 private:
  // �߳��б�
  std::unordered_map<int, std::unique_ptr<Thread>> threads_;
  // ��ʼ���߳�����
  int initThreadSize_;
  int threadSizeThreshHold_;
  // ��ǰ�̳߳ص��߳�����
  std::atomic_int curThreadSize_;
  std::atomic_int idleThreadSize_;

  // Task�������
  using Task = std::function<void()>;
  std::queue<Task> taskQue_;
  std::atomic_int taskSize_;
  // �����������
  int taskQueMaxThreshHold_;

  // �߳�ͬ�����
  std::mutex taskQueMtx_;
  std::condition_variable notFull_;
  std::condition_variable notEmpty_;
  // ��Դ����
  std::condition_variable exitCond_;

  PoolMode poolMode_;
  std::atomic_bool isPoolRuning_;
};


int sum1(int a, int b)
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // �ȽϺ�ʱ
    return a + b;
}
int sum2(int a, int b, int c)
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return a + b + c;
}

void test_thread_pool()
{
    ThreadPool pool;
    // pool.setMode(PoolMode::MODE_CACHED);
    pool.start(2);

    std::future<int> r1 = pool.submitTask(sum1, 1, 2);
    std::future<int> r2 = pool.submitTask(sum2, 1, 2, 3);
    std::future<int> r3 = pool.submitTask([](int b, int e)->int {
        int sum = 0;
        for (int i = b; i <= e; i++)
            sum += i;
        return sum;
        }, 1, 100);
    std::future<int> r4 = pool.submitTask([](int b, int e)->int {
        int sum = 0;
        for (int i = b; i <= e; i++)
            sum += i;
        return sum;
        }, 1, 100);
    std::future<int> r5 = pool.submitTask([](int b, int e)->int {
        int sum = 0;
        for (int i = b; i <= e; i++)
            sum += i;
        return sum;
        }, 1, 100);
    //future<int> r4 = pool.submitTask(sum1, 1, 2);

    std::cout << r1.get() << std::endl;
    std::cout << r2.get() << std::endl;
    std::cout << r3.get() << std::endl;
    std::cout << r4.get() << std::endl;
    std::cout << r5.get() << std::endl;
}


}  // namespace ThreadPool

#endif // thread_pool_hpp