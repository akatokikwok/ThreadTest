#include "Runnable/ThreadRunnableProxy.h"
#include "Windows/WindowsPlatformProcess.h"
#include "HAL/RunnableThread.h"

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable()
	:IThreadProxy()
	, bRun(false)
	, bSuspend(true)// 默认挂起.
	, bImplement(false)// 不执行
	, Thread(nullptr)// 线程实例置空
	, ThreadEvent(FPlatformProcess::GetSynchEventFromPool())
	, StartUpEvent(FPlatformProcess::GetSynchEventFromPool())
{

}

FThreadRunnable::~FThreadRunnable()
{
	// 释放事件对象
	FPlatformProcess::ReturnSynchEventToPool(ThreadEvent);
	// 释放事件对象
	FPlatformProcess::ReturnSynchEventToPool(StartUpEvent);

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
	bImplement = true;// 打开执行开关
	ThreadEvent->Trigger();// 借助线程事件, 让别的线程来唤醒本线程.
}

void FThreadRunnable::CreateSafeThread()
{
	bImplement = true;// 创建的时候,确认要执行.
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
	bImplement = false;

	ThreadEvent->Trigger();// 激活原有的线程.
	StartUpEvent->Wait();// 阻塞我们的启动线程.

	FPlatformProcess::Sleep(0.03f);// 目的是让运行的线程有足够的时间返回.
}

#pragma region override FRunable4个虚方法.
//
uint32 FThreadRunnable::Run()
{
	while (bRun) {
		if (bSuspend) { // 若命中挂起flag.
			ThreadEvent->Wait();// 如果检查确认是挂起标志启用的话, 就执行把线程挂起来
		}

		if (bImplement) {
			bImplement = false;// 这一步是为了仅执行一次,不执行多次
			/// 业务逻辑
			/* 检查字段 简单代理是不是绑了函数,绑了就执行,否则就强制执行lambda*/
			if (ThreadDelegate.IsBound()) {
				ThreadDelegate.Execute();
				ThreadDelegate.Unbind();// 执行完了就解绑定
			}
			else {
				ThreadLambda();// 只要上面的简单代理没有绑定函数,就强制执行lambda
				ThreadLambda = nullptr;// 结束了就把函数指针置空
			}

 			// 若上面步骤都做了一遍,那么就激活挂起flag.
			bSuspend = true;
		}

	}

	// 如果某线程跳出来了while,一定要执行一次激活主线程.
	StartUpEvent->Trigger();

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
	bRun = false;
}
#pragma endregion override FRunable4个虚方法.

