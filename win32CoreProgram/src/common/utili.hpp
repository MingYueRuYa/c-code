/****************************************************************************
**
** Copyright (C) 2018 635672377@qq.com
** All rights reserved.
**
****************************************************************************/

#include <memory>

using std::shared_ptr;

namespace std
{

// �Զ�������shared_ptr
template <typename T>
shared_ptr<T> make_shared_array(size_t size)
{
    //default_delete��STL�е�Ĭ��ɾ����
    return shared_ptr<T>(new T[size], default_delete<T[]>());
}


// ��ָ������ý����жϴ�С
template <typename T>
struct LessDerefrence : 
    public std::binary_function<const T *, const T *, bool>
{
    bool operator()(const T * param1, const T *param2) const
    {
        return *param1 < *param2;
    }
};


// ��ָ�������н�����
struct Derefrence
{
    template <typename T>
    const T& operator()(const T *ptr) const
    {
        return *ptr;
    }
};


}; // std