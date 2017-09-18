/****************************************************************************
**
** Copyright (C) 2017 liushixiongcpp@163.com
** All rights reserved.
**
****************************************************************************/

/*
* ���ģʽ����װϵͳ�ڲ����ӵ�ϸ�ڣ��Կͻ��ṩ�����õĽӿڡ��ͻ�ֻ��Ҫʹ�ýӿھͿ���ʹ������ϵͳ
*
*/

#ifndef facade_h
#define facade_h

#include <iostream>

using std::cout;
using std::endl;

namespace facade
{
class Scaner    
{
public:
    void Scan() { cout << "Lexical analysis." << endl; } 
};

class Parser
{
public:
    void Parse() { cout << "Grammar analysis." << endl; }
};

class IntermediateCode 
{
public:
    void GeneralCode() { cout << "Generate intermediate code." << endl; }
};

class Compiler
{
public:
    void Run()
    {
        Scaner scaner;
        Parser parser;
        IntermediateCode generalcode;

        scaner.Scan();
        parser.Parse();
        generalcode.GeneralCode();
    }
};

static void test_facade()
{
    cout << "######################################\n" \
            "##  this is facade demo...          ##\n" \
            "##                                  ##\n" \
            "######################################"   \
        << endl;

    Compiler compiler;
    compiler.Run();
}

};

#endif //facade_h