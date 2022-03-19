#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"
#include "Coroutines/SimpleCoroutines.h"

/// 协程管理类.
class SIMPLETHREAD_API FCoroutinesManage : FThreadTemplateBase<ICoroutinesContainer, FCoroutinesHandle>
{
public:
	FCoroutinesManage();

	void Tick(float);// 负责Tick协程.

public:
	//
	template<class UserClass, typename... VarTypes>
	void BindRaw(float InTotalTime, UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		// << 均重载自ICoroutinesContainer类成员方法 1. operator<<(float TotalTime) 2. operator<<(const FSimpleDelegate& InDelegate).
		*this << InTotalTime << FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	};
	//
	template<class UserClass, typename... VarTypes>
	void BindUObject(float InTotalTime, UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		* this << InTotalTime << FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);
	};
	//
	template<class UserClass, typename... VarTypes>
	void BindSP(float InTotalTime,const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		*this << InTotalTime << FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	};
	//
	template<typename FunctorType, typename... VarTypes>
	void BindLambda(float InTotalTime, FunctorType&& InMethod, VarTypes... Vars)
	{
		*this << InTotalTime << FSimpleDelegate::CreateLambda(InMethod, Vars...);
	};
	//
	template<class UserClass, typename... VarTypes>
	void BindUFunction(float InTotalTime, UserClass* TargetClass, const FName& InMethod, VarTypes... Vars)
	{
		*this << InTotalTime << FSimpleDelegate::CreateUFunction(TargetClass, InMethod, Vars...);
	};
};

