1.单例模式
	饿汉模式 
		使用静态的方式初始化，自己被加载的时候就将自己实例化。
	懒汉模式
		第一次调用自己的时候，才会进行初始化的动作。

2.两者的区别
	饿汉模式，是使用静态的初始化的方式，所以需要提前占用系统资源。饿汉模式对于访问量很高的话，效率比较高的。
	懒汉模式，第一次调用自己的时候，进行初始化动作。所以存在多线程的访问问题。因为有锁来保证多线程访问的唯一性，所以锁是效率的瓶颈。
