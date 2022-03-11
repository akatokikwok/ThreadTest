#pragma once
#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "Interface/ProxyInterface.h"
#include "Runnable/ThreadRunnableProxy.h"
#include "Core/SimpleThreadType.h"

/* 线程管理类, 主要负责维护线程池和 对指定线程代理的一些绑定函数*/
class SIMPLETHREAD_API FThreadManagement : public TSharedFromThis<FThreadManagement>
{
public:
	static TSharedRef<FThreadManagement> Get();// 单例模式:拿取本类引用
	static void Destroy();// 单例模式:置空静态单例

private:/// 这些逻辑都不应该暴露,而是由封装的插件自己来清除线程.
	void CleanAllThread();// 清除所有线程
	void CleanThread(FWeakThreadHandle Handle);// 清除指定句柄的线程.

public:
	bool ProceduralProgress(FWeakThreadHandle Handle);// 利用句柄查询判断该线程是否闲置
	bool Do(FWeakThreadHandle Handle);

public:/// 从线程池里揪出空闲线程,然后仅用作绑定
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle BindRaw(
		UserClass* TargetClass,
		typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod,
		VarTypes... Vars)
	{
		FWeakThreadHandle handle;// 假设1个闲置线程句柄.
		for (auto& ThreadProxy : Pool) {// 遍历线程池查询闲置线程
			if (ProceduralProgress(ThreadProxy->GetThreadHandle())) {
				ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...);// 给闲置线程绑 C++函数.
				handle = ThreadProxy->GetThreadHandle();// cache闲置线程代理 的句柄.
				break;// 若线程代理是闲置的,就跳出for.
			}
		}

		if ( !handle.IsValid() ) {// 若未查到闲置线程代理,则直接调用API创一个.
			CreateThreadRaw<UserClass, VarTypes...>(TargetClass, InMethod, Vars...);
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

	template<typename... VarTypes>
	FWeakThreadHandle CreateThreadLambda(
		TFunction<void(VarTypes...)> InMethod, VarTypes... Vars)
	{
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable());
		ThreadProxy->GetThreadDelegate().BindLambda(InMethod, Vars...);
		return UpdateThreadPool(ThreadProxy);
	}

	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle CreateThreadSP(UserClass* TargetClass,
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

	/* 同上面那几种模板函数, 只不过这次绑定的是lambda而非上面泛型的多参*/
	FWeakThreadHandle CreatetThread(const FThradLambda& ThreadLambda);

private:
	static TSharedPtr<FThreadManagement> ThreadManagement;// 静态单例指针.
	TArray<TSharedPtr<IThreadProxy>> Pool;// 线程池.

};