/****************************************************************************
**
** Copyright (C) 2017 liushixiongcpp@163.com
** All rights reserved.
**
****************************************************************************/

/*
 * ģ��ģʽ�� �ڸ����ж������㷨�ĹǼܣ������ʵ����������ʵ��
 *
 */

#ifndef template_method_h
#define template_method_h

#include <iostream>

using std::cout;
using std::endl;

namespace template_method
{

class Resume
{
public:
    void FillResume()
    {
        SetName();
        SetAge();
        SetEducation();
    }

protected:
    virtual void SetName() {}

    virtual void SetAge() {}

    virtual void SetEducation() {}
    //....
};

class ResumeA : public Resume
{
protected:
    void SetName() { cout << "A set name..." << endl; }
    void SetAge() { cout << "A set age..." << endl; }
    void SetEducation() { cout << "A set education..." << endl; }
};

class ResumeB : public Resume
{
protected:
    void SetName() { cout << "B set name..." << endl; }
    void SetAge() { cout << "B set age..." << endl; }
    void SetEducation() { cout << "B set education..." << endl; }
};

static void test_template_method()
{
    cout << "######################################\n" \
            "##  this is template method demo... ##\n" \
            "##                                  ##\n" \
            "######################################"   \
        << endl;
   Resume *resume_a = new ResumeA(); 
   resume_a->FillResume();

   Resume *resume_b = new ResumeB();
   resume_b->FillResume();

    delete resume_a;
    resume_a = NULL;

    delete resume_b;
    resume_b = NULL;

}
};

#endif //template_method_h
