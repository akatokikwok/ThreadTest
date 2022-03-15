#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "../SimpleThreadType.h"
#include "Tickable.h"
#include "Interface/ProxyInterface.h"
#include "Delegates/Delegate.h"
#include "Containers/Queue.h"
class IThreadProxy;

/// 关联线程池、任务队列
class SIMPLETHREAD_API FThreadTaskManagement
	: public IThreadTaskContainer
	, public FTickableGameObject
{
public:
	FThreadTaskManagement();
	~FThreadTaskManagement();

	// 初始化一些线程; 这些线程数量都是固定的,若你想创建自己的,则推荐使用FThreadProxyManage类
	void Init(int32 ThreadNum);

private:/// 重写父函数.

	virtual void Tick(float DeltaTime) override;// Tick发生在主线程.
	virtual TStatId GetStatId() const override;

public:
	/************************************************************************/
	/* 创建线程后立刻执行,无需等待,属于异步操作.
	 * 使用操作符重载.
	/************************************************************************/

	//
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle CreateRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	};

	//
	template<typename UserClass, typename... VarTypes>
	void CreateUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::CreateUFunction(TargetClass, InMethod, Vars...);
	};

	//
	template<typename FunctorType, typename... VarTypes>
	void CreateLambda(FunctorType&& InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::CreateLambda(InMethod, Vars...);
	};

	//
	template<typename UserClass, typename... VarTypes>
	void CreateSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	};

	//
	template<typename UserClass, typename... VarTypes>
	void CreateUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);
	};
	
};	