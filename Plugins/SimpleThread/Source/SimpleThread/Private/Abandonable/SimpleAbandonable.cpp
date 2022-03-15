#include "Abandonable/SimpleAbandonable.h"

FSimpleAbandonable::FSimpleAbandonable(const FSimpleDelegate& InThreadDelegate)
	: ThreadDelegate(InThreadDelegate)
{

}

void FSimpleAbandonable::DoWork()
{
	ThreadDelegate.ExecuteIfBound();
}

FORCEINLINE TStatId FSimpleAbandonable::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FSimpleAbandonable, STATGROUP_ThreadPoolAsyncTasks);
}

