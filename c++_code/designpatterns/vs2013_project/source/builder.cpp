/****************************************************************************
**
** Copyright (C) 2017 liushixiongcpp@163.com
** All rights reserved.
**
****************************************************************************/

/*
 * ������ģʽ�������Ӷ���Ĺ����ͱ�ʾ���룬ʹ��ͬ���Ĺ�����̴�����ͬ�ı�ʾ
 *
 */

#ifndef builder_h
#define builder_h

#include <iostream>

using std::cout;
using std::endl;

namespace builder
{
class Builder
{
public:
    virtual void BuildHead() {}
    virtual void BuildBody() {}
    virtual void BuildLeftArm() {}
    virtual void BuildRightArm() {}
    virtual void BuildLeftLeg() {}
    virtual void BuildRightLeg() {}
};

class ThinBuilder : public Builder
{
public:
    void BuildHead() { cout << "build thin head" << endl; }
    void BuildBody() { cout << "build thin body" << endl; }
    void BuildLeftArm() { cout << "build thin left arm" << endl; }
    void BuildRightArm() { cout << "build thin right arm" << endl; }
    void BuildLeftLeg() { cout << "build thin left leg" << endl; }
    void BuildRightLeg() { cout << "build thin right leg" << endl; }
};

class FatBuilder : public Builder
{
public:
    void BuildHead() { cout << "build fat head" << endl; }
    void BuildBody() { cout << "build fat body" << endl; }
    void BuildLeftArm() { cout << "build fat left arm" << endl; }
    void BuildRightArm() { cout << "build fat right arm" << endl; }
    void BuildLeftLeg() { cout << "build fat left leg" << endl; }
    void BuildRightLeg() { cout << "build fat right leg" << endl; }
};

//���˵��directorȥ���ˣ��ٽ�Create�����õ�builder�У�����template method
class Director
{
public:
    Director(Builder *pBuilder) : mBuilder(pBuilder) {}

    void Create()
    {
        mBuilder->BuildHead();
        mBuilder->BuildBody();
        mBuilder->BuildLeftArm();
        mBuilder->BuildRightArm();
        mBuilder->BuildLeftLeg();
        mBuilder->BuildRightLeg();
    }

private:
    Builder *mBuilder;
};


static void test_builder()
{
    cout << "######################################\n" \
            "##  this is builder demo...         ##\n" \
            "##                                  ##\n" \
            "######################################"   \
        << endl;
    FatBuilder fat;
    Director director_fat(&fat);
    director_fat.Create();

    cout << "--------build fat-------" << endl;

    ThinBuilder thin;
    Director director_thin(&thin);
    director_thin.Create();
    cout << "--------build thin-------" << endl;
}
};

#endif //builder_h
