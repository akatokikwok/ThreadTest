#pragma once
#include "CoreMinimal.h"

/// 模板类,统一代理的类.
template<class FTask, typename ReturnType = void>
class FThreadTemplate : public FTask
{
public:/** 同步 << */

	template<typename UserClass, typename... VarTypes>
	ReturnType BindRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	};

	template<typename UserClass, typename... VarTypes>
	ReturnType BindUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);

	};

	template<typename UserClass, typename... VarTypes>
	ReturnType BindSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	};

	template<typename FunctorType, typename... VarTypes>
	ReturnType BindLambda(FunctorType&& InMethod, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateLambda(InMethod, Vars...);
	};

	template<typename UserClass, typename... VarTypes>
	ReturnType BindUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		return *this << FSimpleDelegate::CreateUFunction(TargetClass, InMethod, Vars...);
	};

public:/** 异步 >> */
	
	template<typename UserClass, typename... VarTypes>
	ReturnType CreateRaw(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateRaw(TargetClass, InMethod, Vars...);
	};

	template<typename UserClass, typename... VarTypes>
	ReturnType CreateUObject(UserClass* TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateUObject(TargetClass, InMethod, Vars...);

	};

	template<typename UserClass, typename... VarTypes>
	ReturnType CreateSP(const TSharedRef<UserClass>& TargetClass, typename TMemFunPtrType<false, UserClass, void(VarTypes...)>::Type InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateSP(TargetClass, InMethod, Vars...);
	};

	template<typename FunctorType, typename... VarTypes>
	ReturnType CreateLambda(FunctorType&& InMethod, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateLambda(InMethod, Vars...);
	};

	template<typename UserClass, typename... VarTypes>
	ReturnType CreateUFunction(UserClass* TargetClass, const FName& InMethodName, VarTypes... Vars)
	{
		return *this >> FSimpleDelegate::CreateUFunction(TargetClass, InMethod, Vars...);
	};
};