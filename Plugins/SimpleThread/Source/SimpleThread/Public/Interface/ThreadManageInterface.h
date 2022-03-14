#pragma once
#include "CoreMinimal.h"
#include "Windows/WindowsCriticalSection.h"
#include "ProxyInterface.h"
#include "Core/ThreadCoreMacro.h"
#include "Runnable/ThreadRunnableProxy.h"
#include "Core/SimpleThreadType.h"
#include "Containers/Queue.h"

/// 本类用作IThreadProxyContainer的继承来源.
class IThreadContainer
{
public:
	IThreadContainer() {};
	virtual ~IThreadContainer() {};

protected:
	FCriticalSection Mutex;// 互斥锁.
};

/// 本类多继承自 1个线程代理集合类、1个线程容器类.
/// 创建完的线程不会立马销毁,如有需要可复盘使用.
/// 支持异步和同步,应用于小范围场景.
class IThreadProxyContainer
	: public TArray<TSharedPtr<IThreadProxy>>// 线程池.
	, public IThreadContainer// 公共容器接口.
{
public:
	/** 操作符<<重载. 创建线程并注册到内存池. */
	FWeakThreadHandle operator<<(const TSharedPtr<IThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCL;// 使用作用域锁.
		ThreadProxy->CreateSafeThread();// 利用线程代理创建1个线程.
		this->Add(ThreadProxy);// 线程代理注册到本尊类里.

		return ThreadProxy->GetThreadHandle();//返回线程代理里的弱句柄.
	};

	/** 操作符^重载. 找闲置线程并new一个. 挂起版*/
	FWeakThreadHandle operator^(const FSimpleDelegate& Delegate)
	{
		FWeakThreadHandle ThreadHandle = nullptr;
		{
			MUTEX_LOCL;
			for (auto& Tmp : *this) {// 遍历线程池.找闲置线程.
				if (Tmp->IsSuspend()) {
					Tmp->GetThreadDelegate() = Delegate;// 设置闲置线程的代理.
					ThreadHandle = Tmp->GetThreadHandle();// 找到了符合的线程并保存他的句柄.
					break;
				}
			}
		}// 离开作用域才会解锁.

		if (!ThreadHandle.IsValid()) {
			ThreadHandle = *this << MakeShareable(new FThreadRunnable(true));
		}

		return ThreadHandle;
	};

	/**  操作符<<重载. 找闲置线程并new一个. 不挂起版*/
	FWeakThreadHandle operator<<(const FSimpleDelegate& Delegate)
	{
		FWeakThreadHandle ThreadHandle = nullptr;
		{
			MUTEX_LOCL;
			for (auto& Tmp : *this) {
				if (Tmp->IsSuspend()) {
					Tmp->GetThreadDelegate() = Delegate;// 设置闲置线程的代理.
					ThreadHandle = Tmp->GetThreadHandle();// 找到了符合的线程并保存他的句柄.
					break;
				}
			}
		}// 离开作用域才会解锁.

		if (!ThreadHandle.IsValid()) {
			ThreadHandle = *this << MakeShareable(new FThreadRunnable(false));
		}

		return ThreadHandle;
	};

	/** 重载>>; 依据句柄查询对应线程.. */
	TSharedPtr<IThreadProxy> operator>>(const FWeakThreadHandle& Handle)
	{
		MUTEX_LOCL;

		for (auto& Tmp : *this) {
			if (Tmp->GetThreadHandle() == Handle) {
				return Tmp;
			}
		}
		return nullptr;
	};

};

/// IThreadTaskContainer同时具备任务队列,线程池,公共容器接口.
/// 本类负责线程的任务自动管理,自动配置闲置的线程池,实现较为高效地利用线程池.
class IThreadTaskContainer
	: public TQueue<FSimpleDelegate>// 任务队列.
	, public TArray<TSharedPtr<IThreadProxy>>// 线程池.
	, public IThreadContainer// 公共容器接口.
{
public:
	/** 把外部任务存到任务队列. */
	bool operator<<(const FSimpleDelegate& deleGate)
	{
		MUTEX_LOCL;
		return this->Enqueue(deleGate);// 检查是否添加到队列成功.
	};

	/** 从队列里取一个任务并将其从尾部移除. */
	bool operator>>(FSimpleDelegate& deleGate)
	{
		MUTEX_LOCL;
		return this->Dequeue(deleGate);
	};

	/** 往自己容器里添加线程. */
	IThreadTaskContainer& operator<<(const TSharedPtr<IThreadProxy>& threadProxy)
	{
		MUTEX_LOCL;
		this->Add(threadProxy);
		return *this;
	};

	/** 查询闲置线程,若未找到则将外部任务注入任务队列. 支持链式编程. */
	IThreadTaskContainer& operator^(const FSimpleDelegate& deleGate)
	{
		bool bSuccessful = false;
		{
			MUTEX_LOCL;
			for (auto& Tmp : *this) {
				if (Tmp->IsSuspend()) {
					Tmp->GetThreadDelegate() = deleGate;// 获取该闲置线程任务.
					Tmp->WakeupThread();// 唤醒该闲置线程.

					bSuccessful = true;
					break;
				}
			}
		}
		// 若未找到闲置线程,则考虑把外部任务加入任务队列.
		if (!bSuccessful) {
			*this << deleGate;
		}

		return *this;
	};
};