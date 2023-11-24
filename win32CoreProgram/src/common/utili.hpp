/****************************************************************************
**
** Copyright (C) 2018 635672377@qq.com
** All rights reserved.
**
****************************************************************************/

#include <memory>
#include <cmath>

using std::shared_ptr;

namespace std {

// �Զ�������shared_ptr
template <typename T>
shared_ptr<T> make_shared_array(size_t size) {
  // default_delete��STL�е�Ĭ��ɾ����
  return shared_ptr<T>(new T[size], default_delete<T[]>());
}

// ��ָ������ý����жϴ�С
//template <typename T>
//struct LessDerefrence : public std::binary_function<const T*, const T*, bool> {
//  bool operator()(const T* param1, const T* param2) const {
//    return *param1 < *param2;
//  }
//};

// ��ָ�������н�����
struct Derefrence {
  template <typename T>
  const T& operator()(const T* ptr) const {
    return *ptr;
  }
};

// ��mapִ�и�Ч�ʵĲ�����ѡ��operator[]����ѡ��insert
template <typename MapType, typename KeyArgType, typename ValueArgType>
typename MapType::iterator efficentAddOrUpdate(
    MapType& mapType,
    const KeyArgType& keyArgType,
    const ValueArgType& valueArgType) {
  typename MapType::iterator ifind = mapType.lower_bound(keyArgType);

  // ���key�Ѿ����ھ��Ǹ��²���
  if (ifind != mapType.end() &&
      !(mapType.key_comp()(keyArgType, ifind->first))) {
    ifind->second = valueArgType;

    return ifind;
  } else {  // �����ھ�ִ��insert����
    typedef typename MapType::value_type MVT;

    return mapType.insert(ifind, MVT(keyArgType, valueArgType));
  }
}

bool isEqual(double a, double b) {
  return fabs(a - b) < 1e-9; // ʹ��һ��С���ݲ�ֵ���бȽ�
}

};  // namespace std