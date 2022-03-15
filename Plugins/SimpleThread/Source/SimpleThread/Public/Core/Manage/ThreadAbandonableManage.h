#pragma once
#include "CoreMinimal.h"
#include "Interface/ThreadManageInterface.h"

struct SIMPLETHREAD_API FThreadAbandonablePManage : public IAbandonableContainer
{
public:

	// 异步>>,  Raw.
	template<typename UserClass, typename... VarTypes>
	void CreateASynchRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	};

	// 异步>>, UObject.
	template<typename UserClass, typename... VarTypes>
	void CreateAsynchUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);
	};

	// 异步>>, SP.
	template<typename UserClass, typename... VarTypes>
	void CreateASynchSP(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	};

	// 异步>>, Lambda.
	template<typename FunctorType, typename... VarTypes>
	void CreateASyncLambda(FunctorType&& InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateLambda(InMethod, Vars...);
	};

	// 异步>>, UFunction.
	template<typename UserClass, typename... VarTypes>
	void CreateASyncUFunction(UserClass* TargetClass, const FName& InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateUFunction(TargetClass, InMethod, Vars...);
	};

	//////////////////////////////////////////////////////////////////////////

	// 同步<<, Raw.
	template<typename UserClass, typename... VarTypes>
	void CreateSynchRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	};

	// 同步<<, UObejct.
	template<typename UserClass, typename... VarTypes>
	void CreateSynchUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);
	};

	// 同步<<, SP.
	template<typename UserClass, typename... VarTypes>
	void CreateSynchSP(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	};

	// 同步<<, Lambda.
	template<typename FunctorType, typename... VarTypes>
	void CreateSyncLambda(FunctorType&& InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateLambda(InMethod, Vars...);
	};

	// 同步<<, UFunction.
	template<typename UserClass, typename... VarTypes>
	void CreateSyncUFunction(UserClass* TargetClass, const FName& InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateUFunction(TargetClass, InMethod, Vars...);
	};

};