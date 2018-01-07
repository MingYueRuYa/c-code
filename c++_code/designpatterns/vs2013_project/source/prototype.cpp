/****************************************************************************
**
** Copyright (C) 2017 liushixiongcpp@163.com
** All rights reserved.
**
****************************************************************************/

/*
 * ԭ��ģʽ�� ��ԭ��ʵ��ָ��������������࣬����ͨ��������Щԭ�ʹ����µĶ���
 * Prototype ���������ڣ����õ�һ�� Base* ����ָ��ĳ�� Derived ���������¡�� Derived ����
 * �������в�д�� Derived �ľ������ͣ���Ϊ�кܶ������࣬������������ù��캯���Ǹ㲻���ģ�type-switch �� bad smells
 * 1��Ϊʲô����newֱ���½����󣬶�Ҫ��ԭ��ģʽ��
 * ���ȣ���new�½������ܻ�ȡ��ǰ��������ʱ��״̬����ξ���new���¶����ڽ���ǰ�����ֵ���Ƹ��¶���Ч��Ҳ����ԭ��ģʽ�ߡ�
 * 2��Ϊʲô��ֱ��ʹ�ÿ������캯������Ҫʹ��ԭ��ģʽ��
 * ԭ��ģʽ�뿽�����캯���ǲ�ͬ�ĸ���������캯���漰��������֪�ģ�ԭ��ģʽ�漰���������δ֪�ģ�����Ŀ������캯��ֻ�ܸ��Ƶõ�����Ķ��󣩡�
 * ԭ��ģʽ���ɵ��¶��������һ�������ࡣ�������캯�����ɵ��¶���ֻ�����౾��
 * ԭ��ģʽ��������һ��ͨ�÷���(�����)�������������ʵ�ֵģ���������������������һ������ʵ�ַ�����
 * ����ʹ�ó���
    1����Դ�Ż��������ʼ����Ҫ�����ǳ������Դ�������Դ�������ݡ�Ӳ����Դ�ȡ�
    2�����ܺͰ�ȫҪ��ĳ���ͨ��new����һ��������Ҫ�ǳ�����������׼�������Ȩ�ޣ������ʹ��ԭ��ģʽ��
    3��һ���������޸��ߵĳ���һ��������Ҫ�ṩ������������ʣ����Ҹ��������߿��ܶ���Ҫ�޸���ֵʱ�����Կ���ʹ��ԭ��ģʽ����������󹩵�����ʹ�á�
    4�����ʹ����ʵ����Ŀ�У�ԭ��ģʽ���ٵ������֣�һ���Ǻ͹�������ģʽһ����֣�ͨ��clone�ķ�������һ������Ȼ���ɹ��������ṩ�������ߡ�ԭ��ģʽ�Ѿ���Java��Ϊ��Ȼһ�壬��ҿ�����������ʹ�á�
   �ġ�ȱ��
    1���䱸��¡������Ҫ����Ĺ��ܽ���ͨ�̿��ǣ������ȫ�µ��಻�Ǻ��ѣ����������е��಻һ�������ף��ر�һ�������ò�֧�ִ��л��ļ�Ӷ��󣬻������ú���ѭ���ṹ��ʱ�� 
    2��ʵ��ԭ��ģʽÿ�������඼����ʵ�� Clone�ӿڡ� 
    3���ӱܹ��캯����Լ����
 */

#ifndef prototype_h
#define prototype_h

#include <string.h>

#include <iostream>

using std::cout;
using std::endl;

namespace prototype 
{
class Resume
{
public:
    Resume() {}

    Resume(const Resume &pResume) = delete;

    Resume &operator=(const Resume &pResume) = delete;

    virtual ~Resume() 
    {
        cout << "resume ctor..." << endl;
        if (NULL == name) {
            return;
        }
        delete []name;
        name = NULL;
    }

    virtual Resume *Clone() = 0;

    char *GetName()
    {
        return name;
    }

protected:
    char *name;

};

class ResumeA : public Resume
{
public:
    ResumeA(const char *pName)
    {
        if (NULL == pName) {
            name = new char[1];
            *name = '\0';
        } else {
            int length = strlen(pName) + 1;
            name = new char[length];
            strncpy(name, pName, length);
        }
    }

    ResumeA(const ResumeA &pResume)
    {
        if (NULL == pResume.name) {
            name = new char[1];
            *name = '\0';
        } else {
            int length = strlen(pResume.name) + 1;
            name = new char[length];
            strncpy(name, pResume.name, length);
        }
    }

    ResumeA &operator=(const ResumeA &pResume) = delete;

    ~ResumeA() 
    {
        cout << name << " ctor..." << endl;
        if (NULL == name) {
            return;
        }
        delete []name;
        name = NULL;
    }

    ResumeA *Clone()
    {
        return new ResumeA(*this);
    }

};

class ResumeB : public Resume
{
public:
    ResumeB(const char *pName)
    {
        if (NULL == pName) {
            name = new char[1];
            *name = '\0';
        }
        else {
            int length = strlen(pName) + 1;
            name = new char[length];
            strncpy(name, pName, length);
        }
    }

    ResumeB(const ResumeB &pResume)
    {
        if (NULL == pResume.name) {
            name = new char[1];
            *name = '\0';
        }
        else {
            int length = strlen(pResume.name) + 1;
            name = new char[length];
            strncpy(name, pResume.name, length);
        }
    }

    ResumeB &operator=(const ResumeB &pResume) = delete;

    ~ResumeB()
    {
        cout << name << " ctor..." << endl;

        if (NULL == name) {
            return;
        }
        delete[]name;
        name = NULL;
    }

    ResumeB *Clone()
    {
        return new ResumeB(*this);
    }

};

static void test_prototype()
{
    cout << "######################################\n" \
            "##  this is prototype demo..        ##\n" \
            "##                                  ##\n" \
            "######################################"   \
        << endl;

    Resume *resume_a = new ResumeA("resume_a");
    cout << "resume_a name:" << resume_a->GetName() << endl;    

    Resume *resume_a2 = resume_a->Clone();
    cout << "resume_a2 name:" << resume_a2->GetName() << endl;    

    delete resume_a;
    resume_a = NULL;
    delete resume_a2;
    resume_a2 = NULL;

    Resume *resume_b = new ResumeB("resume_b");
    cout << "resume_b name:" << resume_b->GetName() << endl;    

    Resume *resume_b2 = resume_b->Clone();
    cout << "resume_b2 name:" << resume_b2->GetName() << endl;    

    delete resume_b;
    resume_b = NULL;
    delete resume_b2;
    resume_b2 = NULL;
}
};

#endif //prototype_h
