#include "ThreadManager.h"

TSharedPtr<FThreadManagement> FThreadManagement::ThreadManagement = nullptr;

TSharedRef<FThreadManagement> FThreadManagement::Get()
{
	if (!ThreadManagement.IsValid()) {// 如果智能指针为空,就给他创建1个
		ThreadManagement = MakeShareable(new FThreadManagement());
	}
	return ThreadManagement.ToSharedRef();
}

void FThreadManagement::Destroy()
{
	if (ThreadManagement.IsValid()) {
		ThreadManagement->CleanAllThread();
		ThreadManagement = nullptr;
	}
}

void FThreadManagement::CleanAllThread()
{
	FScopeLock ScopeLock(&Mutex);// 加作用域锁.
	for (auto& ThreadProxy : Pool) {
		ThreadProxy->WaitAndCompleted();
	}
	Pool.Empty();
}

void FThreadManagement::CleanThread(FWeakThreadHandle Handle)
{
	FScopeLock ScopeLock(&Mutex);// 加作用域锁.
	int32 RemoveIndex = INDEX_NONE;
	for (int32 i = 0; i < Pool.Num(); ++i) {
		if (Pool[i]->GetThreadHandle() == Handle) {
			Pool[i]->WaitAndCompleted();
			break;
		}
	}

	if (RemoveIndex != INDEX_NONE) {
		Pool.RemoveAt(RemoveIndex);
	}
}

EThreadState FThreadManagement::ProceduralProgress(FWeakThreadHandle Handle)
{
	if (!Handle.IsValid()) {
		return EThreadState::ERROR;
	}
	FScopeLock ScopeLock(&Mutex);// 加作用域锁.
	for (auto& ThreadProxy : Pool) {
		if (ThreadProxy->IsSuspend()) {
			if (Handle == ThreadProxy->GetThreadHandle()) {
				return EThreadState::LEISURELY;
			}
		}
	}

	return EThreadState::WORKING;
}

bool FThreadManagement::Do(FWeakThreadHandle Handle)
{
	FScopeLock ScopeLock(&Mutex);// 加作用域锁.

	// 查到当前线程句柄是闲置的,就把遍历出来的线程唤醒.
	for (auto& ThreadProxy : Pool) {
		if (ThreadProxy->IsSuspend()) {
			if (Handle == ThreadProxy->GetThreadHandle()) {
				ThreadProxy->WakeupThread();

				return true;
			}
		}
	}

	return false;
}

bool FThreadManagement::DoWait(FWeakThreadHandle Handle)
{
	FScopeLock ScopeLock(&Mutex);// 加作用域锁.

	// 查到当前线程句柄是闲置的,就把遍历出来的线程唤醒.
	for (auto& ThreadProxy : Pool) {
		if (ThreadProxy->IsSuspend()) {
			if (Handle == ThreadProxy->GetThreadHandle()) {
				ThreadProxy->BlockingAndCompletion();

				return true;
			}
		}
	}

	return false;
}

// FWeakThreadHandle FThreadManagement::CreatetThread(const FThradLambda& ThreadLambda)
// {
// 	// 创建线程代理并更新内部的lambda
// 	TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable());
// 	ThreadProxy->GetThreadLambda() = ThreadLambda;
// 
// 	/* 使用目标线程代理创建线程实例并注册线程池,最后返回1根弱句柄*/
// 	return UpdateThreadPool(ThreadProxy);
// }

FWeakThreadHandle FThreadManagement::UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy)
{
	FScopeLock ScopeLock(&Mutex);// 加作用域锁.

	ThreadProxy->CreateSafeThread();// 利用线程代理创建1个线程.
	Pool.Add(ThreadProxy);// 线程代理池里存一个新的线程代理.

	return ThreadProxy->GetThreadHandle();//返回线程代理里的弱句柄.
}

