#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "../SimpleThreadType.h"

/// 关联线程池.
class SIMPLETHREAD_API FThreadProxyManage : public IThreadProxyContainer
{


public:
	~FThreadProxyManage();

public:
	// 同步,仿std命名.
	bool Join(FWeakThreadHandle Handle);
	// 异步,仿std命名.
	bool Detach(FWeakThreadHandle Handle);
	// 判断是否执行好.
	EThreadState Joinable(FWeakThreadHandle Handle);

public:
	/************************************************************************/
	/* 绑定,但是不执行, 可以依据Join/Detach 来自由选择执行线程.
	* 使用操作符重载.
	/************************************************************************/

	//
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle BindRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::BindRaw(TargetClass, InMethod, Vars...);
	};

	//
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle BindUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::BindUFunction(TargetClass, InMethod, Vars...);
	};

	//
	template<typename FunctorType, typename... VarTypes>
	FWeakThreadHandle BindLambda(FunctorType&& InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::BindLambda(InMethod, Vars...);
	};

	//
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle BindSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::BindSP(TargetClass, InMethod, Vars...);
	};

	//
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle BindUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::BindUObject(TargetClass, InMethod, Vars...);
	};

public:
	/************************************************************************/
	/* 创建线程后立刻执行,无需等待,属于异步操作.
	* 使用操作符重载.
	/************************************************************************/

	//
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle CreateRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::BindRaw(TargetClass, InMethod, Vars...);
	};

	//
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle CreateUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::BindUFunction(TargetClass, InMethod, Vars...);
	};

	//
	template<typename FunctorType, typename... VarTypes>
	FWeakThreadHandle CreateLambda(FunctorType&& InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::BindLambda(InMethod, Vars...);
	};

	//
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle CreateSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::BindSP(TargetClass, InMethod, Vars...);
	};

	//
	template<typename UserClass, typename... VarTypes>
	FWeakThreadHandle CreateUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this ^ FSimpleDelegate::BindUObject(TargetClass, InMethod, Vars...);
	};

};

