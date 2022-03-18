#pragma once
#include "CoreMinimal.h"
#include "Interface/ProxyInterface.h"
#include "HAL/Runnable.h"
#include "Windows/WindowsCriticalSection.h"
#include "Core/SimpleSemaphore.h"

/* 自定义类: FThreadRunnable-- 多继承自1个线程代理, 1个FRunnable
 * FThreadRunnable(bool IsSuspendAtFirst = false);// 显式的构造器.false创建即挂起, true则不挂起.
 */
class SIMPLETHREAD_API FThreadRunnable : public FRunnable, public IThreadProxy// 多继承1个线程代理, 1个FRunnable
{
public:
// 	FThreadRunnable();// 默认构造器.创建的时候默认激活挂起符号.

	FThreadRunnable(bool IsSuspendAtFirst = false);// 显式的构造器.创建的时候动态指定是否挂起线程.
	virtual ~FThreadRunnable();

public:/// Override IThreadProxy
// 	virtual void SuspendThread() override;// 挂起线程

	virtual void WakeupThread() override;// 唤醒线程
	virtual void CreateSafeThread() override;// 创建线程实例
	virtual bool IsSuspend() override;// 线程是否挂起.
	virtual void WaitAndCompleted() override; // 在线程执行后的逻辑.
	virtual void BlockingAndCompletion() override;// 阻塞并完成.

private:/// override FRunnable
	virtual uint32 Run() override;
	virtual bool Init() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	bool bRun;// 是否允许允许线程
	bool bSuspendAtFirst;// 第一次是否允许挂起.
//  	bool bImplement;// 是否执行

	FRunnableThread* Thread;//线程实例
	FName RunnableName;//线程名字

	/** FSimpleSemaphore类封装了FEvent信号量. */
	FSimpleSemaphore ThreadEvent;// "某线程"信号量. 该事件专门负责挂起,谁执行就把挂起,再使用别的线程把当前唤醒
	FSimpleSemaphore StartUpEvent;// "启动线程"的信号量.
	FSimpleSemaphore WaitExecuteEvent;// "已挂起"的信号量.

	FCriticalSection Mutex;

	static int32 ThreadCount;// 静态,本线程计数.
};