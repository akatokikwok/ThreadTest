#include "Runnable/ThreadRunnableProxy.h"
#include "Windows/WindowsPlatformProcess.h"
#include "HAL/RunnableThread.h"

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable()
	:IThreadProxy()
	, bRun(false)
	, bSuspend(false)// 不挂起
	, Thread(nullptr)// 线程实例置空
	, ThreadEvent(FPlatformProcess::GetSynchEventFromPool())
{

}

FThreadRunnable::~FThreadRunnable()
{
	// 释放事件对象
	FPlatformProcess::ReturnSynchEventToPool(ThreadEvent);

	if (Thread != nullptr) {

	}
}

void FThreadRunnable::SuspendThread()
{
	bSuspend = true;
}

/// 是由其他线程 来调用FThreadRunnable::WakeupThread()这个函数
void FThreadRunnable::WakeupThread()
{
	bSuspend = false;// 确认激活唤醒线程的时候要阻止它再次挂起
	ThreadEvent->Trigger();
}

void FThreadRunnable::CreateSafeThread()
{
	RunnableName = *(TEXT("SimpleThread-") + FString::Printf(TEXT("%i"), ThreadCount));// 做个线程名字

	Thread = FRunnableThread::Create(this, *RunnableName.ToString(), 0, TPri_BelowNormal);// 用线程名创建线程实例并计数增加
	ThreadCount++;
}

uint32 FThreadRunnable::Run()
{
	while (bRun) {
		if (bSuspend) { // 若不想把线程释放掉,则可以选择将其挂起来
			ThreadEvent->Wait();// 如果检查确认是挂起标志启用的话, 就执行把线程挂起来
		}

		/// 业务逻辑
		/* 检查字段 简单代理是不是绑了函数,绑了就执行,否则就强制执行lambda*/
		if (ThreadDelegate.IsBound()) {
			ThreadDelegate.Execute();
		} else {
			ThreadLambda();// 只要上面的简单代理没有绑定函数,就强制执行lambda
		}
	}

	return 0;
}

bool FThreadRunnable::Init()
{
	if (!FPlatformProcess::SupportsMultithreading()) {
		bRun = false;
	} else {
		bRun = true;
	}
	return bRun;
}

void FThreadRunnable::Stop()
{

}

void FThreadRunnable::Exit()
{
	bRun = false;
}

