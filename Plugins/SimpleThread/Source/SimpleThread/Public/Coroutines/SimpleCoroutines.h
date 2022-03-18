#pragma once
#include "CoreMinimal.h"
#include "Core/SimpleThreadType.h"

/// 协程请求.
struct FCoroutinesRequest
{
public:
	FCoroutinesRequest(float InIntervalTime);

	bool bCompleteRequest;
	float IntervalTime;// 类似于deltaTime.
};

/// 协程接口.
class ICoroutinesObject : public TSharedFromThis<ICoroutinesObject>
{
	friend class ICoroutinesContainer;

public:
	ICoroutinesObject();
	virtual ~ICoroutinesObject() {};
	//
	bool operator==(const ICoroutinesObject& SimpleThreadHandle)
	{
		return this->Handle == SimpleThreadHandle.Handle;
	};

	//
	FORCEINLINE void Awaken() { bAwaken = true; }
protected:
	virtual void Update(FCoroutinesRequest& CoroutinesRequest) = 0;

protected:
	static TArray<TSharedPtr<ICoroutinesObject>> mArray;
	uint8 bAwaken : 1;// 用以唤醒当前的协程.
	FSimpleThreadHandle Handle;
};

typedef TWeakPtr<ICoroutinesObject> FCoroutinesHandle;// 协程接口的弱句柄.

/// 协程对象.
class FCoroutinesObject : public ICoroutinesObject
{
public:
	FCoroutinesObject(const FSimpleDelegate& InSimpleDelegate);
	FCoroutinesObject(float InTotalTime, const FSimpleDelegate& InSimpleDelegate);

protected:
	virtual void Update(FCoroutinesRequest& CoroutinesRequest) final;

protected:
	FSimpleDelegate mSimpleDelegate;
	const float mTotalTime;// 达到多长时间才执行函数,类似于delay.
	float mRunningTime;

};