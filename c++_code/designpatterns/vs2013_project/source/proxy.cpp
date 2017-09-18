/****************************************************************************
**
** Copyright (C) 2017 liushixiongcpp@163.com
** All rights reserved.
**
****************************************************************************/

/*
* ����ģʽ��Ϊ���������ṩ�����Կ����������ķ��ʡ�Զ�̴������������������������
* ����һ���������ĵ���Ƕ��ͼ�ζ�����ĵ��༭������Щͼ�ζ���Ĵ��������ܴ󡣵��Ǵ��ĵ������Ѹ�٣�
* ��������ڴ��ĵ�ʱӦ����һ���Դ������п����ܴ�Ķ���
* ����Ϳ������ô���ģʽ���ڴ��ĵ�ʱ��������ͼ�ζ��󣬶��Ǵ�ͼ�ζ���Ĵ����������ʵ��ͼ�Ρ�����������Ҫ��ͼ��ʱ�����ɴ������
*/

#ifndef proxy_h
#define proxy_h

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

namespace proxy
{
class Image
{
public:
    Image(const string &pName) : mName(pName) {}

    virtual ~Image() {}

    virtual void Show() {}

protected:
    string mName;
};

class BigImage : public Image
{
public:
    BigImage(const string &pName) : Image(pName) {}

    ~BigImage() {}

    void Show()
    {
        cout << "show big image, " << this->mName << endl;
    }
};

class ImageProxy : public Image
{
public:
    ImageProxy(const string &pName) : Image(pName) 
    {
        mImage = new BigImage(pName);
    }

    virtual ~ImageProxy() 
    {
        if (NULL == mImage) { return; }
        delete mImage;
        mImage = NULL;
    }

    void Show()
    {
        if (NULL == mImage) { return; }
        mImage->Show();
    }

private:
    Image *mImage;
};


    static void test_proxy()
    {
        cout << "######################################\n" \
                "##  this is proxy demo...           ##\n" \
                "##                                  ##\n" \
                "######################################"   \
             << endl;

        Image *test_image = new ImageProxy("big image proxy.");
        test_image->Show();
    }

};

#endif //proxy_h