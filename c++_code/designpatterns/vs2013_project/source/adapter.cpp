/****************************************************************************
**
** Copyright (C) 2017 liushixiongcpp@163.com 
** All rights reserved.
**
****************************************************************************/

/*
 * ������ģʽ������Ľӿڸ�Ϊ�û�ϣ��������һ���ӿڣ�����ӿڲ��������⣬�������������Ͷ�����������һ�㶼�ö���������
 * ��Ϊ�˸�����ǰ�Ĵ���
 *
 */

#ifndef adapter_h
#define adapter_h

#include <iostream>

using std::cout;
using std::endl;

//stl��deque��queue��satck����������������
//deque���Ǳ��������
namespace adapter 
{
//˫�˶���
class Deque
{
public:
    void PushBack(const int &pElement) { cout << "deque push back." << endl; }
    void PushFront(const int &pElement) { cout << "deque push front." << endl; }
    void PopBack() { cout << "deque pop back." << endl; }
    void PopFront() { cout << "deque pop front." << endl; }
};

//˳������
class Sequence
{
public:
    virtual void Push(const int &pElement) = 0;
    virtual void Pop() = 0;

protected:
    Deque mDeque;
};

//ջ���ݽṹ
class Stack : public Sequence
{
public:
    void Push(const int &pElement) 
    { 
        cout << "stack push..." << endl;
        mDeque.PushBack(pElement);
    }

    void Pop()
    {
        cout << "stack pop..." << endl;
        mDeque.PopBack();
    }
};

//�������ݽṹ
class Queue : public Sequence
{
public:
    void Push(const int &pElement)
    {
        cout << "queue push..." << endl;
        mDeque.PushBack(pElement);
    }

    void Pop()
    {
        cout << "queue pop..." << endl;
        mDeque.PopFront();
    }
};

static void test_adapter()
{
    cout << "######################################\n" \
            "##  this is adapter demo...         ##\n" \
            "##                                  ##\n" \
            "######################################"   \
            << endl;

    Queue queue;
    queue.Push(1);
    queue.Pop();

    Stack stack;
    stack.Push(2);
    stack.Pop();

}

};

#endif //adapter_h
