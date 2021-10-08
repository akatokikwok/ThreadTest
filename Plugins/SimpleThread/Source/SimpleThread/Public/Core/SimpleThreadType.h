/*
此文件用以定义那些杂项的变量、类、函数、函数指针
*/
#pragma once
#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"

/* 用以查询并控制线程状态的 线程句柄*/
struct SIMPLETHREAD_API FSimpleThreadHandle : public TSharedFromThis<FSimpleThreadHandle>
{
	FSimpleThreadHandle();

	bool operator==(const FSimpleThreadHandle& simpleThreadHandle)
	{
		return
			this->GUIDTypeA == simpleThreadHandle.GUIDTypeA &&
			this->GUIDTypeB == simpleThreadHandle.GUIDTypeB &&
			this->GUIDTypeC == simpleThreadHandle.GUIDTypeC &&
			this->GUIDTypeD == simpleThreadHandle.GUIDTypeD;
	}
protected:
	uint64 GUIDTypeA;
	uint64 GUIDTypeB;
	uint64 GUIDTypeC;
	uint64 GUIDTypeD;

};

typedef TWeakPtr<FSimpleThreadHandle> FWeakThreadHandle; // 注意!!这里是弱指针 线程句柄!!把这个线程句柄同时也声明为1根弱指针
typedef TFunction<void()> FThradLambda;// 一根函数指针, 用于线程的lambda
