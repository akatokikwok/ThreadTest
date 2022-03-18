#include "Coroutines/SimpleCoroutines.h"

ICoroutinesObject::ICoroutinesObject()
	: bAwaken(false)
{

}

TArray<TSharedPtr<ICoroutinesObject>> ICoroutinesObject::mArray;

void FCoroutinesObject::Update(FCoroutinesRequest& CoroutinesRequest)
{
	if (mTotalTime != INDEX_NONE) {
		mRunningTime += CoroutinesRequest.IntervalTime;
		if (mRunningTime >= mTotalTime) {
			mSimpleDelegate.ExecuteIfBound();
			CoroutinesRequest.bCompleteRequest = true;// 表明协程请求处理完了.
		}
	}
	else {// Else则表明只有一个代理.
		if (bAwaken == true) {
			mSimpleDelegate.ExecuteIfBound();// 唤醒(执行)任务.
			CoroutinesRequest.bCompleteRequest = true;// 表明协程请求处理完了.
		}
	}
}

FCoroutinesRequest::FCoroutinesRequest(float InIntervalTime)
	: bCompleteRequest(false)
	, IntervalTime(InIntervalTime)
{

}

FCoroutinesObject::FCoroutinesObject(const FSimpleDelegate& InSimpleDelegate)
	: ICoroutinesObject()// 调接口构造器.
	, mSimpleDelegate(InSimpleDelegate)
	, mTotalTime(INDEX_NONE)// INDEX_NONE为-1
	, mRunningTime(0.0f)
{
	mArray.Add(this->AsShared());
}

FCoroutinesObject::FCoroutinesObject(float InTotalTime, const FSimpleDelegate& InSimpleDelegate)
	: ICoroutinesObject()// 调接口构造器.
	, mSimpleDelegate(InSimpleDelegate)
	, mTotalTime(InTotalTime)
	, mRunningTime(0.0f)
{
	mArray.Add(this->AsShared());

}
