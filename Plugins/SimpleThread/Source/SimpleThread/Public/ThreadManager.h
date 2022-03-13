#pragma once
#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "Interface/ProxyInterface.h"
#include "Runnable/ThreadRunnableProxy.h"
#include "Core/SimpleThreadType.h"
#include "Windows/WindowsCriticalSection.h"

enum class EThreadState
{
	LEISURELY,
	WORKING,
	ERROR
};


/* 线程管理类, 主要负责维护线程池和 对指定线程代理的一些绑定函数
 * 该类线程安全.所有使用到线程池的操作均引入了作用域锁.
 */
class SIMPLETHREAD_API FThreadManagement : public TSharedFromThis<FThreadManagement>
{
public:
	static TSharedRef<FThreadManagement> Get();// 单例模式:拿取本类引用
	static void Destroy();// 单例模式:置空静态单例

private:/// 这些逻辑都不应该暴露,而是由封装的插件自己来清除线程.
	// 清除所有线程.
	void CleanAllThread();
	// 清除单个线程.
	void CleanThread(FWeakThreadHandle Handle);

public:
	// 查询指定句柄的线程是否闲置.
	EThreadState ProceduralProgress(FWeakThreadHandle Handle);

	// 激活指定线程, 但不阻塞激活线程的持有者.
	// 异步;
	bool Do(FWeakThreadHandle Handle);

	// 开启线程; 且阻塞激活线程的持有者,即主线程(亦或是启动线程)直至子线程的任务完成.
	// 同步; 多应用于CPU GPU之间管线.
	bool DoWait(FWeakThreadHandle Handle);

public:/// 从线程代理池里查空闲线程,然后仅用作绑定

	/** Bind Raw.*/
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle BindRaw(
		UserClass* TargetClass,
		typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod,
		VarTypes... Vars
	)
	{
		FWeakThreadHandle handle;// 假设1个闲置线程句柄.
		for (auto& ThreadProxy : Pool) {// 遍历线程池查询挂起的线程.
			if (ThreadProxy->IsSuspend()) {
				ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...);// 给闲置线程绑 C++函数.
				handle = ThreadProxy->GetThreadHandle();// cache闲置线程代理 的句柄.
				break;// 若线程代理是闲置的,就跳出for.
			}
		}
		// 若所有线程都在忙,不存在挂起的那个,
		if (!handle.IsValid()) {// 若未查到闲置线程代理,则直接调用API创一个.
			handle = CreateThreadRaw<UserClass, VarTypes...>(TargetClass, InMethod, Vars...);
		}

		return handle;
	};

	/** Bind UObject. */
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle BindUObject(
		UserClass* TargetClass,
		typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod,
		VarTypes... Vars
	)
	{
		FWeakThreadHandle handle;// 假设1个闲置线程句柄.
		for (auto& ThreadProxy : Pool) {// 遍历线程池查询闲置线程
			if (ThreadProxy->IsSuspend()) {
				ThreadProxy->GetThreadDelegate().BindUObject(TargetClass, InMethod, Vars...);
				handle = ThreadProxy->GetThreadHandle();// cache闲置线程代理 的句柄.
				break;// 若线程代理是闲置的,就跳出for.
			}
		}

		if (!handle.IsValid()) {// 若未查到闲置线程代理,则直接调用API创一个.
			handle = CreateThreadUObject<UserClass, VarTypes...>(TargetClass, InMethod, Vars...);
		}

		return handle;
	};

	/**  Bind UFunction.*/
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle BindUFunction(
		UserClass* TargetClass,
		const FName& InMethod,
		VarTypes... Vars
	)
	{
		FWeakThreadHandle handle;// 假设1个闲置线程句柄.
		for (auto& ThreadProxy : Pool) {// 遍历线程池查询闲置线程
			if (ThreadProxy->IsSuspend()) {
				ThreadProxy->GetThreadDelegate().BindUFunction(TargetClass, InMethod, Vars...);
				handle = ThreadProxy->GetThreadHandle();// cache闲置线程代理 的句柄.
				break;// 若线程代理是闲置的,就跳出for.
			}
		}

		if (!handle.IsValid()) {// 若未查到闲置线程代理,则直接调用API创一个.
			handle = CreateThreadUFunction<UserClass, VarTypes...>(TargetClass, InMethod, Vars...);
		}

		return handle;
	};

	/** Bind Lambda. */
	template<typename FunctorType, typename... VarTypes>
	FWeakThreadHandle BindLambda(
		FunctorType&& InMethod,/* 使用右值引用是为了防止传进来的方法内存拷贝.*/
		VarTypes... Vars
	)
	{
		FWeakThreadHandle handle;// 假设1个闲置线程句柄.
		for (auto& ThreadProxy : Pool) {// 遍历线程池查询闲置线程
			if (ThreadProxy->IsSuspend()) {
				ThreadProxy->GetThreadDelegate().BindLambda(InMethod, Vars...);
				handle = ThreadProxy->GetThreadHandle();// cache闲置线程代理 的句柄.
				break;// 若线程代理是闲置的,就跳出for.
			}
		}

		if (!handle.IsValid()) {// 若未查到闲置线程代理,则直接调用API创一个.
			handle = CreateThreadLambda<VarTypes...>(InMethod, Vars...);
		}

		return handle;
	};

	/** Bind SP. */
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle BindSP(
		const TSharedRef<UserClass>& TargetClass,
		typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod,
		VarTypes... Vars
	)
	{
		FWeakThreadHandle handle;// 假设1个闲置线程句柄.
		for (auto& ThreadProxy : Pool) {// 遍历线程池查询闲置线程
			if (ThreadProxy->IsSuspend()) {
				ThreadProxy->GetThreadDelegate().BindSP(TargetClass, InMethod, Vars...);
				handle = ThreadProxy->GetThreadHandle();// cache闲置线程代理 的句柄.
				break;// 若线程代理是闲置的,就跳出for.
			}
		}

		if (!handle.IsValid()) {// 若未查到闲置线程代理,则直接调用API创一个.
			handle = CreateThreadSP<UserClass, VarTypes...>(TargetClass, InMethod, Vars...);
		}

		return handle;
	};

protected:
	/* 使用目标线程代理创建线程实例并注册线程池,最后返回1根弱句柄*/
	FWeakThreadHandle UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy);

public:
	/* 泛型方法:
	* 实例化1个线程代理,并拿取线程代理后访问到里面的简单委托
	* 借助简单委托, 给目标类对象绑定泛型多参C++函数
	* 再次利用此线程代理, 注册进线程池并返回1个弱指针句柄
	 */
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle CreateThreadRaw(
		UserClass* TargetClass,
		typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod,
		VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable()); // 这一步实例化1个线程代理; new一个子类赋给基类.

		ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...);// 这一步拿到IThreadProxy对象里的 简单委托,在此委托上 给待定的目标类对象 绑定C++函数

		return UpdateThreadPool(ThreadProxy);// 这里业务层拿到了1个线程句柄,通过此句柄来查询当前线程的情况
	};

	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle CreateThreadUFunction(
		UserClass* TargetClass,
		const FName& InMethodName,
		VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable());
		ThreadProxy->GetThreadDelegate().BindUFunction(TargetClass, InMethodName, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	template<typename FunctorType, typename... VarTypes>
	FWeakThreadHandle CreateThreadLambda(
		FunctorType&& InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable());
		ThreadProxy->GetThreadDelegate().BindLambda(InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle CreateThreadSP(const TSharedRef<UserClass>& TargetClass,
									 typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod,
									 VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable());
		ThreadProxy->GetThreadDelegate().BindSP(TargetClass, InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle CreateThreadUObject(UserClass* TargetClass,
										  typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod,
										  VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable());
		ThreadProxy->GetThreadDelegate().BindUObject(TargetClass, InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

// 	/* 同上面那几种模板函数, 只不过这次绑定的是lambda而非上面泛型的多参*/
// 	FWeakThreadHandle CreatetThread(const FThradLambda& ThreadLambda);

private:
	static TSharedPtr<FThreadManagement> ThreadManagement;// 静态单例指针.
	TArray<TSharedPtr<IThreadProxy>> Pool;// 线程池.

	FCriticalSection Mutex;// 作用域锁; 为了防止多个线程进行资源争夺.

};