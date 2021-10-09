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
		TSharedPtr<IThreadProxy> ThreadProxy = MakeShareable(new FThreadRunnable()); // 这一步实例化1个线程代理

		ThreadProxy->GetThreadDelegate().BindRaw(TargetClass, InMethod, Vars...);// 这一步拿到IThreadProxy对象里的 简单委托,在此委托上 给待定的目标类对象 绑定C++函数
		
		return UpdateThreadPool(ThreadProxy);// 这里业务层拿到了1个FSimpleThreadHandle,通过此句柄来查询当前线程的情况
	};

	/* 同上, 只不过这次绑定的是lambda而非上面泛型的多参*/
	FWeakThreadHandle CreatetThread(const FThradLambda& ThreadLambda);

protected:
	/* 使用目标线程代理创建线程实例并注册线程池,最后返回1根弱句柄*/
	FWeakThreadHandle UpdateThreadPool(TSharedPtr<IThreadProxy> ThreadProxy);

private:
	// 线程池
	TArray<TSharedPtr<IThreadProxy>> Pool;
private:
	static TSharedPtr<FThreadManagement> ThreadManagement;// 静态单例指针
};