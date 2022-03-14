#include "Core/SimpleSemaphore.h"
#include "Windows/WindowsPlatformProcess.h"
#include "HAL/Event.h"
#include "SimpleThreadPlatform.h"

FSimpleSemaphore::FSimpleSemaphore()
	:Event(FPlatformProcess::GetSynchEventFromPool())
	, bWait(false)
{

}

FSimpleSemaphore::~FSimpleSemaphore()
{
	FPlatformProcess::ReturnSynchEventToPool(Event);
}

void FSimpleSemaphore::Wait()
{
	bWait = true;
	Event->Wait();
}

void FSimpleSemaphore::Wait(uint32 WaitTime, const bool bIgnoreThreadIdleStats /*= false*/)
{
	bWait = true;
	Event->Wait(WaitTime, bIgnoreThreadIdleStats);
}

void FSimpleSemaphore::Trigger()
{
	Event->Trigger();
	bWait = false;
}

