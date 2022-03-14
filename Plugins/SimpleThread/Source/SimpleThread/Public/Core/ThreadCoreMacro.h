/************************************************************************/
/* 此头文件用来存放通用的一些宏                                            */
/************************************************************************/

#if PLATFORM_WINDOWS
	#include <iostream>
	#include <thread>
	using namespace std;
	#define CPUThreadNumber std::thread::hardware_concurrency()
#else
	#define CPUThreadNumber 12
#endif


#define MUTEX_LOCL FScopeLock ScopeLock(&Mutex)// 作用域锁.