/****************************************************************
 **
 **	Copyright (C) 2017 635672377@qq.com
 ** All rights reserved.
 **
 ***************************************************************/

//测试环境 Linux raspberrypi 4.4.13-v7+ #894 SMP Mon Jun 13 13:13:27 BST 2016 
//armv7l GNU/Linux
//gcc version 4.9.2 (Raspbian 4.9.2-10) 

/*
 * 内存管理学习 Tue Aug 29 23:13:56 CST 2017
 *
 * */

#include <stdlib.h>

#include <iostream>
#include <string>
#include <complex>
#include <memory>

using namespace std;

/*!
 *
 * 测试，内存耗尽，是否调用自定义的_new_handler函数
 * */
namespace demo00
{
	typedef void(*new_handler)();

	void _new_handler()
	{
		cout << "memory out..." << endl;
		abort();
	}

	void test_set_new_handle()
	{
		new_handler old_handler = set_new_handler(_new_handler); 

		//分配海量内存，造成内存耗尽
		int *array = new int[1000000000];
		//result:	
			//memory out...
			//abort
		cout << "end...." << endl;
	}
}


//----------------------------------------------------------------------------
namespace demo01
{
void test_primitives()
{
	cout << "\ntest_primitives()..........\n";
	void *p1 = malloc(512);	//512 bytes
	free(p1);
	p1 = NULL;

	complex<int> *p2 = new complex<int>;	//one object
	delete p2;
	p2 = NULL;

	void *p3 = ::operator new(512);	//512 bytes
	::operator delete(p3);
	p3 = NULL;

//以下使用c++标准库提供的allocators.
//其接口虽有标准规范，但实现厂商并未完全遵守：下面三者形式略异
#ifdef _MSC_VER
	//以下两个函数都是non-static，定要通过object调用。以下分配3个ints
	int *p4 = allocator<int>().allocate(3, (int*)0);
	allocator<int>().deallocate(p4, 3);
#endif

#ifdef __BORLANDC__
	//以下两个函数都是non-static，定要通过object调用。以下分配3个ints
	int *p5 = allocator<int>().allocate(3);
	allocator<int>().deallocate(p5, 3);
#endif

#ifdef __GUNC__
	//以下兩函數都是 static，可通過全名調用之。以下分配 512 bytes.
	//void *p6 = alloc::allocate(512);
	//alloc::deallocate(p4, 512);

	//以下兩函數都是 non-static，定要通過 object 調用。以下分配 3 個 ints.    
	int *p7 = allocator<int>().alllocate(3);
	allocator<int>().deallocate(p7, 3);

	//以下兩函數都是 non-static，定要通過 object 調用。以下分配 3 個 ints.    
	void *p8 = __gun_cxx::__pool_alloc<int>().allocate(3);
	__gun_cxx::__pool_alloc<int>().deallocate(p8, 3);
#endif
}
}

//----------------------------------------------------------------------------
namespace demo02
{
class A
{
public:
	int id;

	A():id(0) {cout << "default ctor. this=" << this << " id=" << id << endl;}
	A(int i):id(i){cout << "ctor. this=" << this << " id=" << id << endl;}
	~A() {cout << "dtor, this=" << this << " id=" << id << endl;}
};

void test_call_ctor_directory()
{
	cout << "\ntest_call_ctor_directory().......\n";
	string *pstr = new string("liushixiong like program!");
	cout << "str= " << *pstr << endl;
	//! pstr->string::string("demo2"); 
	//[Error] 'class std::basic_string<char>' has no member named 'string'
 
	pstr->~string(); //crash ----其语法语义是正确的，
					 //crash只因为上一行被remark起来[侯捷]，
					 //但是实际情况在pi环境下测试并没有crash，此处灰色地带

	A *pA = new A(1); //ctor. this=0x16d8048 id=1
	cout << pA->id << endl; // 1
	//! pA->A::A(3); //in VC6 : ctor, this0013FF60 id 3
					 //in vs2013 : ctor, this0013FF60 id 3
		 //in GCC : [Error] cannot call constructor ‘demo02::A::A’ directly

	cout << pA->id << endl; 	//in VC6 : 3
								//in GCC : 1
	pA->~A(); 					//dtor, this=0xd37048 id=1，手动调用析构函数
	cout << pA->id << endl; 	//测试内存是否被手动调用析构函数释放，\
								//但其实内存并没有释放

	delete pA;					// dtor, this=0x16d8048 id=1

	//simulate new
	void *p = ::operator new(sizeof(A));
	cout << "p=" << p << endl;		//this=000307A8
	pA = static_cast<A *>(p);
	//! pA->A(2); 					//[Error]: invalid use of ‘demo02::A::A’
	//!	pA->A::A(2);				//in VC6 : ctor. this=000307A8 id=2
									//in GCC : [Error] cannot call constructor 'demo02::A::A' directly 

	cout << pA->id << endl;			//in VC6 : 2
									//in GCC : 0

	//simulate delete
	pA->~A(); 						//dtor. this=000307A8, id=0
	::operator delete(pA);			//free();	
}
}

//----------------------------------------------------------------------------
//placement new ----> call copy constructor
#include <new>
namespace demo03
{
using demo02::A;
void test_array_new_and_placement_new()
{
	cout << "\ntest_placement_new()............... \n";
	size_t size = 3;
{
//case 1
//模拟memory pool的做法，array new + placement new，崩溃
	A *buf  = (A *)(new char[sizeof(A)*size]);
	A *temp = buf;
	cout << "buf=" << buf << " temp=" << temp << endl;

	for (int i=0; i<size; ++i) {
		new(temp++) A(i);	//3次 copy ctor
	}
	//! delete [] buf; //crash . why?
					   //因为这其实个char array, 看到 delete [] buf; 编译器会企图唤起多次A::~A()
					   //但array memory layout中找到与array元素（本例3）相关的信息
					   //--整个格局都是错乱（从我对VC的认识而言），于是崩溃
					   //error message:*** Error in `./memory_manager': free(): invalid next size (fast): 0x01735040 ***
	delete buf;		   //dtor just one time3, ~[0]
}
{
//case 2
//测试单纯的array new
	A *buf = new A[size]; //default ctor 3 times, [0]->[1]->[2]
						  //A必须有default ctor，否则[Error] no matching function for call to 'demo02::A::A()'
	A *temp = buf;
	cout << "buf=" << buf << " temp=" << temp << endl;

	for (int i=0; i<size; ++i) {
		new(temp++) A(i);	//3次 ctor
	}
	cout << "buf=" << buf << " temp=" << temp << endl;
	delete [] buf;		 //dtor three times (次序逆反，[2]->[1]->[0])
}
{
//case 3
//掌握崩溃的原因，再次模拟memory pool做法，array new + placement new.
//不用做了，因为memory pool只是提供memory，它并不管construction，
//也不管destruction，它只负责回收memory.
//所以它是以void *或char *取得memory，释放的也是void * or char *.
//不像本例case 1 释放的是A*
//
//事实上 memory pool 形式如demo04::test
}
}
//result:
	//test_placement_new()............... 
	//buf=0x9e5d008 temp=0x9e5d008
	//ctor. this=0x9e5d008 id=0
	//ctor. this=0x9e5d00c id=1
	//ctor. this=0x9e5d010 id=2
	//dtor, this=0x9e5d008 id=0
	//default ctor. this=0x9e5d01c id=0
	//default ctor. this=0x9e5d020 id=0
	//default ctor. this=0x9e5d024 id=0
	//buf=0x9e5d01c temp=0x9e5d01c
	//ctor. this=0x9e5d01c id=0
	//ctor. this=0x9e5d020 id=1
	//ctor. this=0x9e5d024 id=2
	//buf=0x9e5d01c temp=0x9e5d028
	//dtor, this=0x9e5d024 id=2
	//dtor, this=0x9e5d020 id=1
	//dtor, this=0x9e5d01c id=0
}

//------------------------------------------------------------------------------------------------
namespace demo04
{
//ref. C++Primer 3/e, p.765
//per-class allocator
class Screen{
public:
	Screen(int x) : i(x) { };
	int get() { return i; }

	void *operator new(size_t);
	void operator delete(void *, size_t);	//(2)
	void operator delete(void *);			//(1) 二选一
	//(1),(2)同时并存，在其他编译器下可能会爆出奇怪的错误信息，在pi os下没有出现

private:
	Screen *next;
	static Screen *freeStore;
	static const int screenChunk;
private:
	int i;
};	
Screen *Screen::freeStore = NULL;
const int Screen::screenChunk = 24;

void *Screen::operator new(size_t size)
{
	Screen *p;
	if (!freeStore) {
		//linked list 是空的，所以获取一大块memory
		//以下调用的是global operator new
		size_t chunk = screenChunk*size;
		freeStore = p = reinterpret_cast<Screen *>(new char[chunk]);
		//将分配得来的一大块memory，当做linked list将小块小块串联起来
		for (; p != &freeStore[screenChunk-1]; ++p) {
			p->next = p+1;
		}
		p->next = NULL;
	}
	p = freeStore;
	freeStore = freeStore->next;
	return p;
}

void Screen::operator delete(void *p, size_t)
{
	//将deleted object 收回插入linked list前端
	(static_cast<Screen *>(p))->next = freeStore;
	freeStore = static_cast<Screen *>(p);
}

void Screen::operator delete(void *p)
{
	//将deleted object 收回插入linked list前端
	(static_cast<Screen *>(p))->next = freeStore;
	freeStore = static_cast<Screen *>(p);
}

//---------------------
void test_per_class_allocator_1()
{
	cout << "\ntest_per_class_allocator_1()................ \n";
	cout << sizeof(Screen) << endl;
	//pi 下N太大会造成crash
	size_t const N = 20;
	Screen *p[N];
	for (int i=0; i<N; ++i) {
		p[i] = new Screen(i);
	}
	
	//输出前10个pointers，用以比较其间隔
	for (int i=0; i<10; ++i) {
		cout << p[i] << endl;
	}
	//result: 间隔为8，中间没有cookie
		//0x114010
		//0x114018
		//0x114020
		//0x114028
		//0x114030
		//0x114038
		//0x114040
		//0x114048
		//0x114050
		//0x114058

	for (int i=0; i<N; ++i) {
		delete p[i];
	}
}
}


//----------------------------------------------------------------------------
namespace demo05
{
//ref. Effective C++ 2e, item 10
class AirPlane
{
private:
	struct AirPlaneRep {
		unsigned long miles;
		char type;
	};

private:
	union {
		AirPlaneRep rep;	//此针对 used object
		AirPlane *next;		//此针对 free list
	};

public:	
	unsigned long getMiles() { return rep.miles; }
	char getType() { return rep.type; }

	void set(const unsigned long m, char t)
	{
		rep.miles = m;
		rep.type = t;
	}

public:
	static void *operator new(size_t size);
	static void operator delete(void *deadObject, size_t size);

private:
	static const int BLOCK_SIZE;
	static AirPlane *headOfFreeList;
};

AirPlane *AirPlane::headOfFreeList = NULL;
const int AirPlane::BLOCK_SIZE = 512;

void *AirPlane::operator new(size_t size)
{
	//如果大小错误，转交给::operator new()
	if (size != sizeof(AirPlane)) {
		return ::operator new(size);
	}

	AirPlane *p = headOfFreeList;
	//如果p有效，就把list头部往下移一个元素
	if (NULL != p) {
		headOfFreeList = p->next;
	} else {
		//free list 已空，配置一块内存
		//令足够容纳BLOCK_SIZE个AirPlane
		AirPlane *newBlock = static_cast<AirPlane *>(::operator new(BLOCK_SIZE*sizeof(AirPlane)));
		for (size_t i=0; i<BLOCK_SIZE; ++i) {
			newBlock[i].next = &newBlock[i+1];
		}
		//设置最后一个为NULL
		newBlock[BLOCK_SIZE-1].next = NULL;
		//让p指向第一个元素
		p = newBlock;
		//headOfFreeList指向下一个元素
		headOfFreeList = p->next;
	}
	return p;
}

void AirPlane::operator delete(void *deadObject, size_t size)
{
	if (NULL == deadObject) { return; }

	if (size != sizeof(AirPlane)) {
		::operator delete(deadObject);
		return;
	}

	//插入headOfFreeList之前，回收内存
	AirPlane *plane = static_cast<AirPlane *>(deadObject);
	plane->next = headOfFreeList;
	headOfFreeList = plane;
}

//---------------------
void test_per_class_allocator_2()
{
	cout << "\ntest_per_class_allocator_2\n";

	cout << sizeof(AirPlane) << endl;	//8

	size_t const N = 100;
	AirPlane *p[N];

	for (size_t i=0; i<N; ++i) {
		p[i] = new AirPlane();
	}

	//随机测试object是否正常
	p[1]->set(100, 'A');
	p[5]->set(10000, 'B');
	p[9]->set(1000000, 'C');

	cout << p[1] << ' ' << p[1]->getType() << ' ' << p[1]->getMiles() << endl;
	cout << p[5] << ' ' << p[5]->getType() << ' ' << p[5]->getMiles() << endl;
	cout << p[9] << ' ' << p[9]->getType() << ' ' << p[9]->getMiles() << endl;
	//result:
		//	0x85dc010 A 100
		//	0x85dc030 B 10000
		//	0x85dc050 C 1000000

	//输出前10个pointers，用以比较其间隔
	for (size_t i=0; i<10; ++i) {
		cout << p[i] << endl;
	}
	//result:	间隔为8，中间没有cookie
		//	0x85dc008
		//	0x85dc010
		//	0x85dc018
		//	0x85dc020
		//	0x85dc028
		//	0x85dc030
		//	0x85dc038
		//	0x85dc040
		//	0x85dc048
		//	0x85dc050

	for (size_t i=0; i<N; ++i) {
		delete p[i];
	}

}
};

/* !
 *
 *	测试 重载new和delete，以及new[]，以及delete[]
 * */
namespace demo06
{
class Foo
{
public:
	int _id;
	long _data;
	string _str;

public:
	static void *operator new(size_t size);
	static void operator delete(void *deadObject, size_t size);
	static void *operator new[](size_t size);
	static void operator delete[](void *deadObject, size_t size);

	Foo():_id(0) { cout << "default ctor, this=" << this << " id=" << _id << endl; }
	Foo(int i):_id(i) { cout << "ctor, this=" << this << " id=" << _id << endl; }
	//virtual
	~Foo() { cout << "dtor, this=" << this << " id=" << _id << endl; }
	//不加 virtual dtor, sizeof=12, new Foo[5] => operator new[]()的size参数是64(在pi为68)
	//加了virtual dtor, sizeof=16, new Foo[5] => operator new[]()的size参数是84(在pi为88)
	//上述二例，多出来的4(在pi下为8)可能是 size_t 用来放置array size

};

	void *Foo::operator new(size_t size)
	{
		Foo *p = (Foo *)malloc(size);
		cout << "Foo::operator new(), size=" << size << "\t return: " << p << endl;
		return p;
	}

	void Foo::operator delete(void *deadObject, size_t size)
	{
		if (NULL == deadObject) { return; }

		cout << "Foo::operator delete(), pdead=" << deadObject << " size= " << size << endl;
		free(deadObject);
	}

	void *Foo::operator new[](size_t size)
	{
		Foo *p = (Foo *) malloc(size);
		cout << "Foo::operator new[](), size=" << size << "\t return: " << p << endl;
		return p;
	}

	void Foo::operator delete[](void *deadObject, size_t size)
	{
		if (NULL == deadObject) { return; }

		cout << "Foo::operator delete[](), pdead=" << deadObject << " size= " << size << endl;
		free(deadObject);
	}

//---------------------------------
void test_overload_operator_new_and_array_new()
{
	cout << "\ntest_overload_operator_new_and_array_new().......\n";

	cout << "sizeof(Foo)= " << sizeof(Foo) << endl;
	{
		Foo *p = new Foo(7);
		delete p;


		Foo *pArray = new Foo[5];	//无法给array elements 以initializer
		delete []pArray;
	}

	{
		cout << "testing global expression ::new and ::new[] \n";
		//这回绕过overload new(), delete(), new[](), delete[]();
		//当然ctor，dtor都会被正常调用
		Foo *p = ::new Foo(7);
		::delete p;

		Foo *pArray = ::new Foo[5];
		::delete [] pArray;
	}
}

}

/* !
 *
 *	测试：重载placement new
 * */
namespace demo07
{
class Bad {};

class Foo
{
public:
	Foo() { cout << "Foo:Foo()" << endl; }
	
	Foo(int)
	{
		cout << "Foo:Foo(int)" << endl;
		throw Bad();
	}

	//(1) 这个就是一般的operator new()的重载
	void *operator new(size_t size)
	{
		cout << "operator new(size_t size), size=" << size << endl;
		return malloc(size);
	}

	//(2) 这个就是标准库已经提供的placement new()的重载(形式)
	//	(所以我也模拟standard placement new的动作，just return ptr)
	void *operator new(size_t size, void *start)
	{
		cout << "operator new(size_t size, void *start), size= " << size << " start=" << start << endl;
		return start;
	}

	//(3) 这个才是崭新的placement new
	void *operator new(size_t size, long extra)
	{
		cout << "operator new(size_t size, long extra) size=" << size << " extra:" << extra <<endl;
		return malloc(size+extra);
	}

	//(4) 这又是一个placement new
	void *operator new(size_t size, long extra, char init)
	{
		cout << "operator new(size_t size, long extra, char init) size=" << size << " extra= " << extra << " init=" << init << endl;
		return malloc(size+extra);
	}

	//(5) 这又是一个placement new,但故意写错第一个参数的type(它必须是 size_t以满足正常的operator new)
//! 	void *operator new(long extra, char init)
//! 	{
//! 		cout << "operator new(long extra, char init) extra=" << extra << " init=" << init;
//! 		return malloc(extra);
//! 	}
//! [Error]: ‘operator new’ takes type ‘size_t’ (‘unsigned int’) as first parameter [-fpermissive]
//  void *operator new(long extra, char init)


	//以下是搭配上述 placement new的各个called placement delete
	//当ctor发出异常，这儿对应operator (placement) delete就会被唤起
	//应该是要负责释放其搭档兄弟(placement new)分配所得memory
	//(1) 这个就是一般的operator delete()的重载
	void operator delete(void *, size_t)
	{ cout << "operator delete(void *, size_t)  " << endl; }

	//(2) 对应上述的2
	void operator delete(void *, void *)
	{ cout << "operator delete(void *, void *)  " << endl; }

	//(3) 对应上述的3
	void operator delete(void *, long)
	{ cout << "operator delete(void *, long)  " << endl; }
	
	//(4) 对应上述的4
	//如果没有一一对应，也不会有任何编译报错
	void operator delete(void *, long, char)
	{ cout << "operator delete(void *, long, char)  " << endl; }

private:
	int m_i;

};

//------------------
void test_overload_placement_new()
{
	cout << "\n\n\ntest_overload_placement_new()..........\n";

	Foo start; 							//Foo:Foo
	cout << "Foo start" << endl;

	Foo *p1 = new Foo;					//op-new(size_t)
	cout << "new Foo" << endl;

	Foo *p2 = new (&start)Foo;			//op-new(size_t, void *)
	cout << "new (&start)Foo" << endl;

	Foo *p3 = new (100)Foo;				//op-new(size_t, long)
	cout << "new (100)Foo" << endl;

  	Foo *p4 = new (100, 'a')Foo;		//op-new(size_t, long, char)
	cout << "new (100, 'a')Foo" << endl;

	Foo *p5 = new (100) Foo(1);			//op-new(size_t, long) op-del(void *,long)
	cout << "new (100) Foo(1)" << endl;

	Foo *p6 = new (100, 'a') Foo(2);
	cout << "new (100, 'a') Foo(2)" << endl;

	Foo *p7 = new (&start) Foo(1);
	cout << "new (&start) Foo(1)" << endl;

	Foo *p8 = new Foo(1);
	cout << "new Foo(1)" << endl;
	//result:
		// test_overload_placement_new()..........
		// Foo:Foo()
		// Foo start
		// operator new(size_t size), size=4
		// Foo:Foo()
		// new Foo
		// operator new(size_t size, void *start), size= 4 start=0x7e95e58c
		// Foo:Foo()
		// new (&start)Foo
		// operator new(size_t size, long extra) size=4 extra:100
		// Foo:Foo()
		// new (100)Foo
		// operator new(size_t size, long extra, char init) size=4 extra= 100 init=a
		// Foo:Foo()
		// new (100, 'a')Foo
		// operator new(size_t size, long extra) size=4 extra:100
		// Foo:Foo(int)
		// new (100) Foo(1)
		// operator new(size_t size, long extra, char init) size=4 extra= 100 init=a
		// Foo:Foo(int)
		// new (100, 'a') Foo(2)
		// operator new(size_t size, void *start), size= 4 start=0x7e95e58c
		// Foo:Foo(int)
		// new (&start) Foo(1)
		// operator new(size_t size), size=4
		// Foo:Foo(int)
		// new Foo(1)
		// [notice:在4.9上当ctor爆出异常时G4.9没有调用对应的operator delete(void *, long)]
		// [但在G2.9确实是可以的]
		// terminate called after throwing an instance of 'demo07::Bad'
		// Aborted
}

}

/* ! 
 *
 * 重载全局的new, delete, new[], delete[]
 * */
namespace global_new_delete
{
//define 没有作用域的概念
#define test_global_operator_new_delete

class Foo
{
public:
	Foo()
	{
		cout << "Foo ctor..." << this <<  endl;
	}

	~Foo()
	{
		cout << "Foo dctor..." << this <<  endl;
	}
};

void test_global_new_delete()
{
	Foo *foo = new Foo();
	delete foo;
	foo = NULL;

	Foo *arrayfoo = new Foo[10];
	delete []arrayfoo;
	arrayfoo = NULL;
}

#undef test_global_operator_new_delete

};

#ifdef test_global_operator_new_delete

//重载全局的new,delete不能放在namespace中
void *operator new(size_t size)
{
	cout << "global operator new..." << endl;
	return malloc(size);
}

void *operator new[](size_t size)
{
	cout << "global operator new[]..." << endl;
	return malloc(size);
}

void operator delete(void *start)
{
	cout << "global operator delete..." << endl;
	free(start);
}

void operator delete[](void *start)
{
	cout << "global operator delete[]..." << endl;
	free(start);
}
#endif //test_global_operator_new_delete

namespace custom_allocator
{
class allocator
{
public:
	typedef struct object {
		object *next;
	} object;

public:
	allocator() {}

	~allocator() {}

	void *alloc(const size_t size)
	{
		object *p = NULL;
		if (NULL == freeHead) {
			p = freeHead = (object *)malloc(chunksize*size);
			for (int i=0; i<chunksize-1; ++i) {
				p->next = (object *)((char *)p+size);
				p = p->next;
			}
			p->next = NULL; //last
		}
		p = freeHead;
		freeHead = freeHead->next;
		return p;
	}

	void dealloc(void *dead, const size_t size)
	{
		((object *)dead)->next = freeHead;
		freeHead = (object *)dead;
	}

private:
	const int chunksize = 5;

	object *freeHead = NULL;

};

class Foo	//attention: 怪异，如果Foo的为空类，则自定义的allocator会shutdown
{
public:
	Foo() { cout << "address:" << this << endl; }

	~Foo() {}

	static allocator cus_alloc;

	static void *operator new(size_t size)
	{
		return cus_alloc.alloc(size);
	}

	static void operator delete(void *start, size_t size)
	{
		cus_alloc.dealloc(start, size);
	}
private:
	int i = 0;
};

allocator Foo::cus_alloc;

class Goo
{
public:
	Goo() { cout << "address:" << this << endl; }

	~Goo() {}

	static allocator cus_alloc;

	static void *operator new(size_t size)
	{
		return cus_alloc.alloc(size);
	}

	static void operator delete(void *start, size_t size)
	{
		cus_alloc.dealloc(start, size);
	}
private:
	int i = 0;
};

allocator Goo::cus_alloc;

//上面的allocator写法太麻烦，每个类都要写一遍
//将固定部分写成宏定义的形式
#define DECLARE_POOL_ALLOC() \
	public:	\
		void *operator new(size_t size) { return myAlloc.alloc(size); } \
		void operator delete(void *p) { myAlloc.dealloc(p, 0); }	\
	protected: \
		static allocator myAlloc;		

#define IMPLEMENT_POOL_ALLOC(class_name)	\
	allocator class_name::myAlloc;		

class Hoo
{
	DECLARE_POOL_ALLOC()

public:
	Hoo() { cout << "address:" << this << endl; }

	~Hoo() {}

private:
	int i = 0;
};
	IMPLEMENT_POOL_ALLOC(Hoo);


void test_custom_allocator()
{
	cout << "Foo size:" << sizeof(Foo) << endl;

	Foo  *fooarray[100];
	for (int i=0; i<13; ++i) {
		fooarray[i] = new Foo();
	}

	cout << "Goo size:" << sizeof(Foo) << endl;
	Goo  *gooarray[100];
	for (int i=0; i<13; ++i) {
		gooarray[i] = new Goo();
	}

	cout << "Hoo size:" << sizeof(Foo) << endl;
	Hoo  *hooarray[100];
	for (int i=0; i<13; ++i) {
		hooarray[i] = new Hoo();
	}

//result:
	//	Foo size:4
	//	address:0x1b17008
	//	address:0x1b1700c
	//	address:0x1b17010
	//	address:0x1b17014
	//	address:0x1b17018
	//	address:0x1b17020
	//	address:0x1b17024
	//	address:0x1b17028
	//	address:0x1b1702c
	//	address:0x1b17030
	//	address:0x1b17038
	//	address:0x1b1703c
	//	address:0x1b17040
	//	Goo size:4
	//	address:0x1b17050
	//	address:0x1b17054
	//	address:0x1b17058
	//	address:0x1b1705c
	//	address:0x1b17060
	//	address:0x1b17068
	//	address:0x1b1706c
	//	address:0x1b17070
	//	address:0x1b17074
	//	address:0x1b17078
	//	address:0x1b17080
	//	address:0x1b17084
	//	address:0x1b17088
	//	Hoo size:4
	//	address:0x1b17098
	//	address:0x1b1709c
	//	address:0x1b170a0
	//	address:0x1b170a4
	//	address:0x1b170a8
	//	address:0x1b170b0
	//	address:0x1b170b4
	//	address:0x1b170b8
	//	address:0x1b170bc
	//	address:0x1b170c0
	//	address:0x1b170c8
	//	address:0x1b170cc
	//	address:0x1b170d0
}

};

#include <ext/pool_allocator.h>

using std::cout;
using std::endl;

namespace ext_pool_allocator
{
	template<typename Alloc>
	void cookie_test(Alloc alloc, size_t n)
	{
		typename Alloc::value_type *p1, *p2, *p3;

		p1 = alloc.allocate(n);
		p2 = alloc.allocate(n);
		p3 = alloc.allocate(n);

		std::cout << "p1=" << p1 << "\t" << "p2=" << p2 << "\t" << "p3=" << p3 << "\t" << std::endl;

		alloc.deallocate(p1, sizeof(typename Alloc::value_type));
		alloc.deallocate(p2, sizeof(typename Alloc::value_type));
		alloc.deallocate(p3, sizeof(typename Alloc::value_type));
	}

	void test_pool_alloc()
	{
		std::cout << sizeof(__gnu_cxx::__pool_alloc<int>) << std::endl;
		cookie_test(__gnu_cxx::__pool_alloc<double>(), 1);

		std::cout << sizeof(std::allocator<int>) << std::endl;
		cookie_test(std::allocator<double>(), 1);
	}
	//result:
	//1
	//p1=0x8e2e008	p2=0x8e2e010	p3=0x8e2e018	//相距为8，表示没有cookie
	//1
	//p1=0x8e2e150	p2=0x8e2e160	p3=0x8e2e170	//相距为16，表示存在cookie	
};


int main(int argc, char *argv[])
{
//	demo00::test_set_new_handle();
	
//	demo01::test_primitives();
//	demo02::test_call_ctor_directory();
	demo03::test_array_new_and_placement_new();
//	demo04::test_per_class_allocator_1();
//	demo05::test_per_class_allocator_2();
//	demo06::test_overload_operator_new_and_array_new();
//	demo07::test_overload_placement_new();
//	global_new_delete::test_global_new_delete();
//	custom_allocator::test_custom_allocator();
//	ext_pool_allocator::test_pool_alloc();
	return 0;
}
