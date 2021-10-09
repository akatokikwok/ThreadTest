#include "Runnable/ThreadRunnableProxy.h"
#include "Windows/WindowsPlatformProcess.h"
#include "HAL/RunnableThread.h"

int32 FThreadRunnable::ThreadCount = 0;

FThreadRunnable::FThreadRunnable()
	:IThreadProxy()
	, bRun(false)
	, bSuspend(false)// 不挂起
	, bImplement(false)// 不执行
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
	bImplement = true;// 打开执行开关
	ThreadEvent->Trigger();
}

void FThreadRunnable::CreateSafeThread()
{
	bImplement = true;// 当且仅当创建线程实例时候开启执行
	RunnableName = *FString::Printf( TEXT("SimpleThread--%i"), ThreadCount );// 做个线程名字

	Thread = FRunnableThread::Create(this, *RunnableName.ToString(), 0, TPri_BelowNormal);// 用线程名创建线程实例并计数增加
	ThreadCount++;
}

uint32 FThreadRunnable::Run()
{
	while (bRun) {
		if (bSuspend) { // 若不想把线程释放掉,则可以选择将其挂起来
			ThreadEvent->Wait();// 如果检查确认是挂起标志启用的话, 就执行把线程挂起来
		}

		if (bImplement) {
			bImplement = false;// 这一步是为了仅执行一次,不执行多次
			/// 业务逻辑
			/* 检查字段 简单代理是不是绑了函数,绑了就执行,否则就强制执行lambda*/
			if (ThreadDelegate.IsBound()) {
				ThreadDelegate.Execute();
				ThreadDelegate.Unbind();// 执行完了就解绑定
			} else {
				ThreadLambda();// 只要上面的简单代理没有绑定函数,就强制执行lambda
				ThreadLambda = nullptr;// 结束了就把函数指针置空
			}

			// 若上面步骤都做了一遍,那么就挂起线程
			SuspendThread();
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

