/*
 * ί�й��캯��
 * */

#include <iostream>

using std::cout;
using std::cin;
using std::endl;

class A
{
public:
  A()
  {
	age = 0;
	name = "linux";
	cout << "A() " << endl;
  }
	
  //A(int pAge) ί��ΪA()�޲ι��캯�����й��죬����A()��ִ�У���ִ��A(int pAge)
  //ע�⣺
  //1.����д��A(int pAge) : A() , age(pAge) �����˵ί�й��캯��������ʹ�ó�ʼ���б��ʼ��������Ա��
  //2.����д�ɵݹ�ί�У��������ֶδ�������
  A(int pAge) : A()
  {
	cout << "A(int pAge)" << endl;
  }

  void show() const
  {
	cout << age << " " << name << endl;
  }
private:
  int age;
  char *name;

};


int main(int argc, char *argv[])
{
  A a(10);
  a.show();
  return 0;
}
