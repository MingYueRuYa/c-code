/****************************************************************************
**
** Copyright (C) 2017 liushixiongcpp@163.com
** All rights reserved.
**
****************************************************************************/

#ifndef thread_h
#define thread_h

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

using std::cout;
using std::endl;
using std::thread;

namespace use_thread
{

static void func(int a1, int a2, int a3)
{
    cout << "a1=" <<a1 << ", a2=" << a2 << ", a3=" << a3 << endl;
}

static void Continuefunc(int a1, int a2, int a3)
{
    //while (1) {
        cout << "a1=" <<a1 << ", a2=" << a2 << ", a3=" << a3 << endl;
    //}
}

static void test_useage_thread()
{
    cout << "----------------basic useage thread--------------------" << endl;
    thread t1(func, 1, 2, 3);
    //��ȡt1�߳�ID
    cout << "get t1 thread id=" << std::hex << t1.get_id() << endl;
    //join�����������߳�
    t1.join(); 

    thread t2(Continuefunc, 1, 2, 3);
    cout << "get t2 thread id=" << std::hex << t2.get_id() << endl;
    //��ִ̨��t2de�̺߳��������Ҳ�����Ϊmain�����������̺߳���δִ����������쳣����vs2013�²����̺߳���û��ִ����main���������˳�
    t2.detach();

    cout << "after t2, main is runing" << endl;

    //��lambda���ʽ��Ϊ���󶨵��̺߳���
    thread t4([](int a, int b, int c) {
        //�߳�����5S
        std::this_thread::sleep_for(std::chrono::seconds(2));
        cout << "a=" << a << ", b=" << b << ", c=" << c << endl;
    }, 4, 5, 6);
    t4.join();
    
    //��ȡCPU�ͺ���
    cout << "cpu kernel number:" << thread::hardware_concurrency() << endl;

    //��������������ע����䣬�ͻᱬ���쳣����Ϊ�̶߳��������̺߳��������ˡ�Ҫȷ���̶߳�����������ڱ��̺߳�����
    //thread t3(func, 7, 8, 9);
    cout << "----------------basic useage thread--------------------" << endl;
}

static std::mutex g_lock;

static void lock_thread() //mutex����������
{
    g_lock.lock();
    
    static int number = 0;
    cout << "in id:" << std::this_thread::get_id() << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cout << "number=" << ++number << endl;

    g_lock.unlock();
}

static void lock_guard_thread()
{
    std::lock_guard<std::mutex> lock(g_lock);
    
    static int number = 0;
    cout << "in id:" << std::this_thread::get_id() << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cout << "number=" << ++number << endl;
}

static std::timed_mutex time_mutex;

static void time_mutex_thread()
{
    //���ó�ʱ��
    std::chrono::milliseconds timeout(3000);

    while (true) {
        //����ʱ����������������100msʱ�ͷ���false
        if (time_mutex.try_lock_for(timeout)) {
            cout << std::this_thread::get_id() << ": do work with the mutex." << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        } else {
            cout << std::this_thread::get_id() << ": do work without the mutex." << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    } //while
}

static void synchronize_thread()
{
    cout << "----------------synchronisze thread--------------------" << endl;

    cout << "----------------test mutex--------------------" << endl;
    thread t1(lock_thread);
    thread t2(lock_thread);
    thread t3(lock_thread);

    t1.join();
    t2.join();
    t3.join();
    cout << "----------------test mutex--------------------" << endl;

    cout << "----------test lock guard mutex---------------" << endl;
    thread t4(lock_guard_thread);
    thread t5(lock_guard_thread);
    thread t6(lock_guard_thread);

    t4.join();
    t5.join();
    t6.join();
    cout << "----------test lock guard mutex---------------" << endl;

    cout << "-------------test time mutex------------------" << endl;
    thread t7(time_mutex_thread);
    //thread t8(time_mutex_thread);
    t7.join();
    //t8.join();
    cout << "-------------test time mutex------------------" << endl;

    cout << "----------------synchronisze thread--------------------" << endl;
}

static void test_thread()
{
    cout << "**********************test thread**********************" << endl;
    //test_useage_thread();
    synchronize_thread();
    cout << "**********************test thread**********************" << endl;
}

};

#endif //thread_h