#pragma once
#include "CoreMinimal.h"
#include "Windows/WindowsCriticalSection.h"
#include "ProxyInterface.h"
#include "Core/ThreadCoreMacro.h"
#include "Runnable/ThreadRunnableProxy.h"
#include "Core/SimpleThreadType.h"

/// 本类用作IThreadProxyContainer的继承来源.
class IThreadContainer
{
public:
	IThreadContainer() {};
	virtual ~IThreadContainer() {};

protected:
	FCriticalSection Mutex;// 互斥锁.
};

class IThreadProxy;

/// 本类多继承自 1个线程代理集合类、1个线程容器类.
class IThreadProxyContainer : public TArray< TSharedPtr<IThreadProxy> >, public IThreadContainer
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
			for (auto& Tmp : *this) {
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