﻿/************************************************************************/
/* 此头文件主要定义那些特殊的线程类型 
* 如单播代理、 LAMBDA、
   杂项的变量、类、函数、函数指针、结构体*/
/************************************************************************/

#pragma once
#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"

/* 用以查询并控制线程状态的 线程句柄*/
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

typedef TWeakPtr<FSimpleThreadHandle> FWeakThreadHandle; // 弱指针包装的线程句柄(携带线程ID); 注意!!这里是弱指针 线程句柄!!把这个线程句柄同时也声明为1根弱指针
typedef TFunction<void()> FThradLambda;// 一根函数指针, 用于线程的lambda
