/************************************************************************/
/* 此头文件用来存放通用的一些宏                                            */
/************************************************************************/
#include "Misc/ScopeLock.h"

#if PLATFORM_WINDOWS
	#include <iostream>
	#include <thread>
	using namespace std;
	#define CPUThreadNumber std::thread::hardware_concurrency()
#else
#if PLATFORM_LINUX
	#define CPUThreadNumber 12
#else
#if  PLATFORM_MAC
	#define CPUThreadNumber 12
#endif
#endif
#endif

#define MUTEX_LOCL FScopeLock ScopeLock(&Mutex)// 作用域锁.