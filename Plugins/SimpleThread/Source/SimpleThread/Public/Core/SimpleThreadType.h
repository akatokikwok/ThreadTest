/************************************************************************/
/* 此头文件主要定义那些特殊的线程类型 
* 如单播代理、 LAMBDA、
   杂项的变量、类、函数、函数指针、结构体*/
/************************************************************************/

#pragma once
#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"

/** 使用此类去检查线程到底处于何种状态.*/
struct SIMPLETHREAD_API FSimpleThreadHandle : public TSharedFromThis<FSimpleThreadHandle>
{
	/* 构造器用以生成随机线程ID*/
	FSimpleThreadHandle();

	/* 重载操作符用以检索是否匹配外部句柄*/
	bool operator==(const FSimpleThreadHandle& simpleThreadHandle)
	{
		return
			this->GUIDTypeA == simpleThreadHandle.GUIDTypeA &&
			this->GUIDTypeB == simpleThreadHandle.GUIDTypeB &&
			this->GUIDTypeC == simpleThreadHandle.GUIDTypeC &&
			this->GUIDTypeD == simpleThreadHandle.GUIDTypeD;
	}
protected:
	/* 通过这4个不同的ID来寻找我们的线程*/
	uint64 GUIDTypeA;
	uint64 GUIDTypeB;
	uint64 GUIDTypeC;
	uint64 GUIDTypeD;

};

enum class EThreadState
{
	LEISURELY,
	WORKING,
	ERROR
};

typedef TWeakPtr<FSimpleThreadHandle> FWeakThreadHandle; // 线程句柄,弱引用版本.
typedef TFunction<void()> FThradLambda;// 一根函数指针, 用于线程的lambda
