#pragma once
#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "Core/SimpleThreadType.h"

/**
 * 线程代理类.
 * 借助此接口来实现 真正线程里应该具备的一些功能
 * TSharedFromThis<>类,携带弱指针
 * IThreadProxy();构造器负责 创建一个线程句柄, 用以检索线程ID
 */
class SIMPLETHREAD_API IThreadProxy : public TSharedFromThis<IThreadProxy>
{
public:
	// 创建一个线程句柄, 句柄里有GUID检索,用以检索线程ID.
	IThreadProxy();
	// 虚析构,派生类重写.
	virtual ~IThreadProxy();

	// 唤醒某线程.
	virtual void WakeupThread() = 0;
	// 创建某线程.
	virtual void CreateSafeThread() = 0;
	// 线程是否挂起.
	virtual bool IsSuspend() = 0;
	// 线程执行后的逻辑.
	virtual void WaitAndCompleted() = 0; 
	// 专门用以阻塞主线程.
	virtual void BlockingAndCompletion() = 0;// 阻塞并完成.

public:
	FORCEINLINE FSimpleDelegate& GetThreadDelegate() { return ThreadDelegate; }
	FORCEINLINE FThradLambda& GetThreadLambda() { return ThreadLambda; }
	FORCEINLINE FWeakThreadHandle GetThreadHandle() { return (SimpleThreadHandle); }// 拿句柄的弱引用版,注意此接口返回的是弱指针,弱指针声明在Type.h里

protected:
	FSimpleDelegate ThreadDelegate;// UE4内建单播代理的一种,UE引擎自己的单播代理
	FThradLambda ThreadLambda;// 一根函数指针,线程Lambda

private:
	TSharedPtr<FSimpleThreadHandle> SimpleThreadHandle;// 线程句柄,这里是强引用!!!
};