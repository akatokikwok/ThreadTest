#include "Runnable/ThreadRunnableProxy.h"
#include "Windows/WindowsPlatformProcess.h"
#include "HAL/RunnableThread.h"

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable()
	:IThreadProxy()
	, bRun(false)
	, bSuspend(true)// 默认挂起.
// 	, bImplement(false)// 不执行
	, Thread(nullptr)// 线程实例置空
	, ThreadEvent(FPlatformProcess::GetSynchEventFromPool())
	, StartUpEvent(FPlatformProcess::GetSynchEventFromPool())
	, WaitExecuteEvent(FPlatformProcess::GetSynchEventFromPool())
{

}

FThreadRunnable::~FThreadRunnable()
{
	// 释放事件对象
	FPlatformProcess::ReturnSynchEventToPool(ThreadEvent);
	FPlatformProcess::ReturnSynchEventToPool(StartUpEvent);
	FPlatformProcess::ReturnSynchEventToPool(WaitExecuteEvent);

	if (Thread != nullptr) {
		delete Thread;
		Thread = nullptr;
	}
}

// void FThreadRunnable::SuspendThread()
// {
// 	bSuspend = true;// 只要置为TRUE,则自然挂起.
// }

/// 是由其他线程 来调用FThreadRunnable::WakeupThread()这个函数
void FThreadRunnable::WakeupThread()
{
// 	bImplement = true;// 打开执行开关
	ThreadEvent->Trigger();// 借助线程事件, 让别的线程来唤醒本线程.
}

void FThreadRunnable::CreateSafeThread()
{
// 	bImplement = true;// 创建的时候,确认要执行.
	RunnableName = *FString::Printf(TEXT("SimpleThread--%i"), ThreadCount);// 做个线程名字

	Thread = FRunnableThread::Create(this, *RunnableName.ToString(), 0, TPri_BelowNormal);// 用线程名创建线程实例并计数增加
	ThreadCount++;
}

bool FThreadRunnable::IsSuspend()
{

	return bSuspend;
}

void FThreadRunnable::WaitAndCompleted()
{
	// 刷新为不激活不运行.
	bRun = false;
// 	bImplement = false;

	ThreadEvent->Trigger();// 激活原有的线程.
	StartUpEvent->Wait();// 阻塞我们的启动线程.

	FPlatformProcess::Sleep(0.03f);// 目的是让运行的线程有足够的时间返回.
}

void FThreadRunnable::BlockingAndCompletion()
{
	WaitExecuteEvent->Wait();// 让该信号量执行等待.
}

#pragma region override FRunable4个虚方法.
//
uint32 FThreadRunnable::Run()
{
	while (bRun) {
		if (bSuspend) { // 若命中挂起flag.
			ThreadEvent->Wait();// 如果检查确认是挂起标志启用的话, 就执行把线程挂起来
		}

		bSuspend = false;// 关闭挂起flag.
		/// 业务逻辑
		/* 检查字段 简单代理是不是绑了函数,绑了就执行,否则就强制执行lambda*/
		if (ThreadDelegate.IsBound()) {
			ThreadDelegate.Execute();
			ThreadDelegate.Unbind();// 执行完了就解绑定
		}

		// 若上面步骤都做了一遍,那么就激活挂起flag.
		bSuspend = true;
		// 激活 阻塞主线程的信号量.
		WaitExecuteEvent->Trigger();
	}

	return 0;
}

bool FThreadRunnable::Init()
{
	if (!FPlatformProcess::SupportsMultithreading()) {
		bRun = false;
	}
	else {
		bRun = true;
	}
	return bRun;
}

void FThreadRunnable::Stop()
{

}

void FThreadRunnable::Exit()
{
	// 退出的时候,唤醒主线程.
	StartUpEvent->Trigger();
}
#pragma endregion override FRunable4个虚方法.

