﻿#pragma once
#include "CoreMinimal.h"
#include "Interface/ProxyInterface.h"
#include "HAL/Runnable.h"

/* 自定义类: FThreadRunnable-- 多继承自1个线程代理, 1个FRunnable*/
class SIMPLETHREAD_API FThreadRunnable : public FRunnable, public IThreadProxy// 多继承1个线程代理, 1个FRunnable
{
public:
	FThreadRunnable();
	virtual ~FThreadRunnable();

public:/// Override IThreadProxy
// 	virtual void SuspendThread() override;// 挂起线程

	virtual void WakeupThread() override;// 唤醒线程
	virtual void CreateSafeThread() override;// 创建线程实例
	virtual bool IsSuspend() override;// 线程是否挂起.
	virtual void WaitAndCompleted() override; // 在线程执行后的逻辑.

private:/// override FRunnable
	virtual uint32 Run();
	virtual bool Init();
	virtual void Stop();
	virtual void Exit();

private:
	bool bRun;// 是否允许允许线程
	bool bSuspend;// 是否挂起本线程
	bool bImplement;// 是否执行
	FRunnableThread* Thread;//线程实例
	FName RunnableName;//线程名字
	FEvent* ThreadEvent;// 本线程事件. 该事件专门负责挂起,谁执行就把挂起,再使用别的线程把当前唤醒

	FEvent* StartUpEvent;// 专供主线程用的事件.

	static int32 ThreadCount;// 静态,本线程计数.
};