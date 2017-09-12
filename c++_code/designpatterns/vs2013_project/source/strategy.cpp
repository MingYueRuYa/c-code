/****************************************************************************
**
** Copyright (C) 2017 liushixiongcpp@163.com
** All rights reserved.
**
****************************************************************************/

/*
 * ����ģʽ�� ������һϵ����㷨�������Ƿ�װ���������������滻���ǡ�ͬʱ�㷨�ĸı䲻��Ӱ��ͻ��˵Ĵ���
 * ����˵��Щ�㷨����ɵĹ�����һ����ֻ��ʵ�ֵ��ϴ��ڲ��졣
 * �͹���ģʽ�����𣺹���ģʽ��Ϊ�˴���һ�����������������ģʽ��ͬһ������ͬ��ʵ�֡�����˵ͬ�������벻ͬ�����Ч��
 *
 */

#ifndef strategy_h
#define strategy_h

#include <iostream>

using std::cout;
using std::endl;

namespace strategy
{
class ReplaceAlgorithm
{
public:
    virtual void Replace() = 0;
};

class LRU_ReplaceAlgorithm : public ReplaceAlgorithm
{
public:
    void Replace()
    {
        cout << "Least recently used replace algorithm." << endl;
    }
};

class FIFO_ReplaceAlgorithm : public ReplaceAlgorithm
{
public:
    void Replace()
    {
        cout << "first in first out replace algorithm." << endl;
    }
};

class Random_ReplaceAlgorithm : public ReplaceAlgorithm
{
public:
    void Replace()
    {
        cout << "random replace algorithm." << endl;
    }
};

class Cache
{
public:
    typedef enum _AlgorithmType
    {
        LRU,
        FIFO,
        Random
    }AlgorithmType;

public:
    Cache(ReplaceAlgorithm *pAlogrihtm): mAlogrithm(pAlogrihtm)
    {
    }

    Cache(AlgorithmType pType): mAlogrithm(NULL) //��ϼ򵥹���ģʽ
    {
        if (LRU == pType) {
            mAlogrithm = new LRU_ReplaceAlgorithm();
        } else if (FIFO == pType) {
            mAlogrithm = new FIFO_ReplaceAlgorithm();
        } else if (Random == pType) {
            mAlogrithm = new Random_ReplaceAlgorithm();
        } else {
            mAlogrithm = NULL;
        }
    }

    ~Cache()
    {
        delete mAlogrithm;
        mAlogrithm = NULL;
    }

    void Repalce()
    {
        if (NULL == mAlogrithm) { return; }

        mAlogrithm->Replace();
    }

private:
    ReplaceAlgorithm *mAlogrithm;
};

static void test_strategy()
{
    cout << "######################################\n" \
            "##  this is strategy demo...        ##\n" \
            "##                                  ##\n" \
            "######################################"   \
        << endl;
    
    Cache cache(new LRU_ReplaceAlgorithm()); //���ֳ�ʼ���ķ�ʽ���ã���Ϊ������е��㷨ϸ�ڱ�¶���ͻ��ˣ�Υ�������ط���(����֪��ԭ��)
    cache.Repalce();

    Cache cache1(Cache::FIFO);  //����һ��enum��ʾ�㷨�����࣬�ͻ��˲���Ҫ֪���㷨�ľ��嶨����ʲô
    cache1.Repalce();
}
};

#endif //strategy_h
