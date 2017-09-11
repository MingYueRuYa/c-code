/****************************************************************************
**
** Copyright (C) 2017 liushixiongcpp@163.com (http://www.2144.cn)
** All rights reserved.
**
****************************************************************************/

/*
 * ����ģʽ����ָ����һ�����ڴ�������Ľӿڣ����������ʵ������һ���࣬
 * ȱ�㣺����򵥹�������Υ���˿������ԭ�򣬵���Ҳ��һ��ȱ����ǣ�����µĲ�Ʒ��Ҫ����һ��������Ҫ����������
 *
 */

#ifndef factory_h
#define factory_h

#include <iostream>

using std::cout;
using std::endl;

namespace factory 
{
class SingleCore
{
public:
    virtual void Show() = 0;
};

class SingleCoreA :public SingleCore
{
public:
    void Show()
    {
        cout << "this is singleCoreA..." << endl;
    }
};

class SingleCoreB : public SingleCore
{
public:
    void Show()
    {
        cout << "this is singleCoreB..." << endl;
    }
};

class Factory
{
public:    
    virtual SingleCore *CreateSingleCore() = 0;
};

class FactoryA : public Factory
{
public:
    SingleCore *CreateSingleCore() { return new SingleCoreA(); }
};

class FactoryB : public Factory
{
public:
    SingleCore *CreateSingleCore() { return new SingleCoreB(); }
};

static void test_factory()
{
    cout << "######################################\n" \
            "##  this is factory demo...         ##\n" \
            "##                                  ##\n" \
            "######################################"   \
            << endl;
    
    SingleCore *basecore1 = (new FactoryA())->CreateSingleCore();
    basecore1->Show();
    SingleCore *basecore2 = (new FactoryB())->CreateSingleCore();
    basecore2->Show();
}

};

#endif //factory_h
