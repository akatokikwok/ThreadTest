#pragma once
#include "CoreMinimal.h"

class FEvent;

struct FSimpleSemaphore
{
public:
	FSimpleSemaphore();
	~FSimpleSemaphore();

	void Wait();// 等待信号量.
	void Wait(uint32 WaitTime, const bool bIgnoreThreadIdleStats = false);// 等待信号量.
	void Trigger();// 激活信号量.

	FORCEINLINE bool IsWait() { return bWait; }
private:
	FEvent* Event;
	uint8 bWait : 1;// 用以标识信号量是否挂起.
};