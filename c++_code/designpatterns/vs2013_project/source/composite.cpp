/****************************************************************************
**
** Copyright (C) 2017 liushixiongcpp@163.com
** All rights reserved.
**
****************************************************************************/

/*
* ���ģʽ����������ϳɡ����Ρ��ṹ���Ա�ʾ����Ͳ���
* 
*
*/

#ifndef composite_h
#define composite_h

#include <iostream>
#include <string>
#include <list>

using std::cout;
using std::endl;
using std::list;
using std::string;

namespace composite
{
class Company
{
public:
    Company(const string &pName) : mName(pName) {}

    virtual ~Company() {}

    virtual void Add(Company *pCompany) {}

    virtual void Show(const int pDepth) {}

protected:
    string mName;
};

//���幫˾
class ConcreteCompany : public Company
{
public:
    ConcreteCompany(const string &pName) : Company(pName) {}

    virtual ~ConcreteCompany() {}

    void Add(Company *pCompany)
    {
        mCompanyList.push_back(pCompany);
    }

    void Show(const int pDepth)
    {
        for (int i=0; i<pDepth; i++) {
            cout << "-";
        }
        cout << mName << endl;

        list<Company *>::iterator ibegin = mCompanyList.begin();
        for (; ibegin != mCompanyList.end(); ++ibegin) {
            (*ibegin)->Show(pDepth+2);
        }
    }

private:
    list<Company *> mCompanyList;

};

//����Ĳ��ţ�����
class FinanceDepartment : public Company
{
public:
    FinanceDepartment(const string &pName) : Company(pName) {}

    virtual ~FinanceDepartment() {}

    virtual void Show(const int pDepth) //ֻ��Ҫ��ʾ������Ҫ��Ӻ���
    {
        for (int i=0; i<pDepth; ++i) {
            cout << "-";
        }
        cout << mName << endl;
    }
};

//�����������Դ��
class HRDepartment : public Company
{
public:
    HRDepartment(const string &pName) : Company(pName) {}

    virtual ~HRDepartment() {}

    virtual void Show(const int pDepth) //ֻ��Ҫ��ʾ������Ҫ��Ӻ���
    {
        for (int i=0; i<pDepth; ++i) {
            cout << "-";
        }
        cout << mName << endl;
    }
};

    static void test_composite()
    {
        cout << "######################################\n" \
                "##  this is composite demo...       ##\n" \
                "##                                  ##\n" \
                "######################################"   \
            << endl;

        Company *rootcompany = new ConcreteCompany("parent-company");
        Company *left1 = new FinanceDepartment("finance department.");
        Company *left2 = new HRDepartment("hr department.");

        rootcompany->Add(left1);
        rootcompany->Add(left2); 

        Company *childcompany = new ConcreteCompany("child-company");
        Company *left3 = new FinanceDepartment("child finance department.");
        Company *left4 = new HRDepartment("child hr department.");

        childcompany->Add(left3);
        childcompany->Add(left4); 

        rootcompany->Add(childcompany);

        rootcompany->Show(1);

        delete left1;
        delete left2;
        delete left3;
        delete left4;
        delete childcompany;
        delete rootcompany;
    }

};

#endif //composite_h