/************************************************************************/
/* 此头文件用来存放通用的一些宏                                            */
/************************************************************************/
#include "Misc/ScopeLock.h"
#include "Abandonable/SimpleAbandonable.h"
#include "Async/AsyncWork.h"

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

/// 异步操作. //意为:使用UE线程池里的异步线程.
#define USE_UE_THREAD_POOL_ASYNCTASK(ThreadDelegate) \
{\
	new FAutoDeleteAsyncTask<FSimpleAbandonable>(ThreadDelegate)->StartBackgroundTask();\
}
	

/// 同步操作.
#define USE_UE_THREAD_POOL_SYNCTASK(ThreadDelegate) \
{\
	FAsyncTask<FSimpleAbandonable>* SimpleAbandonable = new FAsyncTask<FSimpleAbandonable>(ThreadDelegate);\
	SimpleAbandonable->StartBackgroundTask();\
	SimpleAbandonable->EnsureCompletion();\
	delete SimpleAbandonable;\
}

//////////////////////////////////////////////////////////////////////////
#define ASYNCTASK_UOBJECT(Object, Method, ...) \
	USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateUObject(Object, Method, __VA_ARGS__))

#define ASYNCTASK_Raw(Object, Method, ...) \
	USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateRaw(Object, Method, __VA_ARGS__))

#define ASYNCTASK_SP(Object, Method, ...) \
	USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateSP(Object, Method, __VA_ARGS__))

#define ASYNCTASK_Lambda(Method, ...) \
	USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateLambda(Method, __VA_ARGS__))

#define ASYNCTASK_UFunction(Object, Method, ...) \
	USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateLambda(Object, Method, __VA_ARGS__))


//////////////////////////////////////////////////////////////////////////
#define SYNCTASK_UOBJECT(Object, Method, ...) \
	USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateUObject(Object, Method, __VA_ARGS__))

#define SYNCTASK_Raw(Object, Method, ...) \
	USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateRaw(Object, Method, __VA_ARGS__))

#define SYNCTASK_SP(Object, Method, ...) \
	USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateSP(Object, Method, __VA_ARGS__))

#define SYNCTASK_Lambda(Method, ...) \
	USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateLambda(Method, __VA_ARGS__))

#define SYNCTASK_UFunction(Object, Method, ...) \
	USE_UE_THREAD_POOL_SYNCTASK(FSimpleDelegate::CreateLambda(Object, Method, __VA_ARGS__))

