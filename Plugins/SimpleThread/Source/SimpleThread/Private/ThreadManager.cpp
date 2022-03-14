#include "ThreadManager.h"
#include "Interface/ThreadManageInterface.h"

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
		//ThreadManagement->CleanAllThread();
		ThreadManagement = nullptr;
	}
}

#pragma region ThreadProxyManage.
FThreadProxyManage::~FThreadProxyManage()
{
	// 析构的时候保证线程全部完成.
	for (auto& Tmp : *this) {
		Tmp->WaitAndCompleted();// 线程完成后逻辑.
	}
	this->Empty();
}

bool FThreadProxyManage::Join(FWeakThreadHandle Handle)
{
	TSharedPtr<IThreadProxy> ThreadProxy = *this >> Handle;// 先查出这个线程.
	if (ThreadProxy.IsValid()) {
		ThreadProxy->BlockingAndCompletion();// 该线程阻塞主线程.
		return true;
	}
	return false;
}

bool FThreadProxyManage::Detach(FWeakThreadHandle Handle)
{
	TSharedPtr<IThreadProxy> ThreadProxy = *this >> Handle;// 先查出这个线程.
	if (ThreadProxy.IsValid()) {
		ThreadProxy->WakeupThread();// 该线程被唤醒.
		return true;
	}
	return false;
}

EThreadState FThreadProxyManage::Joinable(FWeakThreadHandle Handle)
{
	TSharedPtr<IThreadProxy> ThreadProxy = *this >> Handle;// 先查出这个线程.
	if (ThreadProxy.IsValid()) {
		if (ThreadProxy->IsSuspend()) {// 检查此线程是不是挂起来了.
			return EThreadState::LEISURELY;
		}
		else {
			return EThreadState::WORKING;
		}
	}
	return EThreadState::ERROR;
}
#pragma endregion ThreadProxyManage.


#pragma region ThreadTaskManage.

FThreadTaskManagement::FThreadTaskManagement()
{
	FThreadTaskManagement::Init(CPUThreadNumber);
}

FThreadTaskManagement::~FThreadTaskManagement()
{
	for (auto& Tmp : *this) {
		Tmp->WaitAndCompleted();
	}
	((TEventQueue*)this)->Empty();
	((TProxyArray*)this)->Empty();
}

void FThreadTaskManagement::Init(int32 ThreadNum)
{
	for (int32 i = 0; i < ThreadNum; ++i) {
		*this << MakeShareable(new FThreadRunnable);
	}
}

void FThreadTaskManagement::Tick(float DeltaTime)
{
	TSharedPtr<IThreadProxy> ThreadProxy = nullptr;// 某闲置线程.
	{
		MUTEX_LOCL;
		for (auto& Tmp : *this) {
			if (Tmp->IsSuspend()) {
				ThreadProxy = Tmp;
				break;
			}
		}
	}
	if (ThreadProxy.IsValid()) {// 确实有闲置线程.
		if (! ( (TEventQueue*)this )->IsEmpty()) {// 检查任务队列不为空
			FSimpleDelegate SimpleDelegate;
			if (*this >> SimpleDelegate) // 任务队列队尾弹出一个任务.
			{
				MUTEX_LOCL;
				ThreadProxy->GetThreadDelegate() = SimpleDelegate;// 刷新闲置线程的任务为 任务队列的那个任务.
				ThreadProxy->WakeupThread();// 唤醒该闲置线程.
			}
		}
	}
}

TStatId FThreadTaskManagement::GetStatId() const
{
	return TStatId();
}
#pragma endregion ThreadTaskManage.



// void FThreadManagement::Tick(float DeltaTime)
// {
// 	FScopeLock ScopeLock(&Mutex);
// 	TSharedPtr<IThreadProxy> ThreadProxy = nullptr;
// 
// 	for (auto& Tmp : Pool) {
// 		if (Tmp->IsSuspend()) {
// 			ThreadProxy = Tmp;
// 			break;
// 		}
// 	}
// 	// 进一步处理那些挂起的闲置线程.
// 	if (ThreadProxy.IsValid()) {
// 		// FSimpleDelegate队列非空.
// 		if (!TaskQueue.IsEmpty()) {
// 			// 取出队列尾部代理
// 			FSimpleDelegate SimpleDelegate;
// 			TaskQueue.Dequeue(SimpleDelegate);
// 
// 			ThreadProxy->GetThreadDelegate() = SimpleDelegate;// 尾部代理记录下来
// 			ThreadProxy->WakeupThread();// 并唤醒线程.
// 		}
// 	}
// }

// TStatId FThreadManagement::GetStatId() const
// {
// 
// 	return TStatId();
// }

// void FThreadManagement::Init(int32 ThreadNum)
// {
// 	for (int32 i = 0; i < ThreadNum; ++i) {
// 		// 		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable());// 使用的是默认构造器.
// 		UpdateThreadPool(MakeShareable(new FThreadRunnable()));
// 	}
// }
// 
// void FThreadManagement::CleanAllThread()
// {
// 	FScopeLock ScopeLock(&Mutex);// 加作用域锁.
// 	for (auto& ThreadProxy : Pool) {
// 		ThreadProxy->WaitAndCompleted();
// 	}
// 	Pool.Empty();
// }
// 
// void FThreadManagement::CleanThread(FWeakThreadHandle Handle)
// {
// 	FScopeLock ScopeLock(&Mutex);// 加作用域锁.
// 	int32 RemoveIndex = INDEX_NONE;
// 	for (int32 i = 0; i < Pool.Num(); ++i) {
// 		if (Pool[i]->GetThreadHandle() == Handle) {
// 			Pool[i]->WaitAndCompleted();
// 			break;
// 		}
// 	}
// 
// 	if (RemoveIndex != INDEX_NONE) {
// 		Pool.RemoveAt(RemoveIndex);
// 	}
// }
// 
// EThreadState FThreadManagement::ProceduralProgress(FWeakThreadHandle Handle)
// {
// 	if (!Handle.IsValid()) {
// 		return EThreadState::ERROR;
// 	}
// 	FScopeLock ScopeLock(&Mutex);// 加作用域锁.
// 	for (auto& ThreadProxy : Pool) {
// 		if (ThreadProxy->IsSuspend()) {
// 			if (Handle == ThreadProxy->GetThreadHandle()) {
// 				return EThreadState::LEISURELY;
// 			}
// 		}
// 	}
// 
// 	return EThreadState::WORKING;
// }
// 
// bool FThreadManagement::Do(FWeakThreadHandle Handle)
// {
// 	FScopeLock ScopeLock(&Mutex);// 加作用域锁.
// 
// 	// 查到当前线程句柄是闲置的,就把遍历出来的线程唤醒.
// 	for (auto& ThreadProxy : Pool) {
// 		if (ThreadProxy->IsSuspend()) {
// 			if (Handle == ThreadProxy->GetThreadHandle()) {
// 				ThreadProxy->WakeupThread();
// 
// 				return true;
// 			}
// 		}
// 	}
// 
// 	return false;
// }
// 
// bool FThreadManagement::DoWait(FWeakThreadHandle Handle)
// {
// 	FScopeLock ScopeLock(&Mutex);// 加作用域锁.
// 
// 	// 查到当前线程句柄是闲置的,就把遍历出来的线程唤醒.
// 	for (auto& ThreadProxy : Pool) {
// 		if (ThreadProxy->IsSuspend()) {
// 			if (Handle == ThreadProxy->GetThreadHandle()) {
// 				ThreadProxy->BlockingAndCompletion();
// 
// 				return true;
// 			}
// 		}
// 	}
// 
// 	return false;
// }
// 
// // FWeakThreadHandle FThreadManagement::CreatetThread(const FThradLambda& ThreadLambda)
// // {
// // 	// 创建线程代理并更新内部的lambda
// // 	TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable());
// // 	ThreadProxy->GetThreadLambda() = ThreadLambda;
// // 
// // 	/* 使用目标线程代理创建线程实例并注册线程池,最后返回1根弱句柄*/
// // 	return UpdateThreadPool(ThreadProxy);
// // }
// 
// FWeakThreadHandle FThreadManagement::UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy)
// {
// 	FScopeLock ScopeLock(&Mutex);// 加作用域锁.
// 
// 	ThreadProxy->CreateSafeThread();// 利用线程代理创建1个线程.
// 	Pool.Add(ThreadProxy);// 线程代理池里存一个新的线程代理.
// 
// 	return ThreadProxy->GetThreadHandle();//返回线程代理里的弱句柄.
// }

