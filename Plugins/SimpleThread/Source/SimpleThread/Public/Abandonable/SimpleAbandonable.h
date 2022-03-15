#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"

struct SIMPLETHREAD_API FSimpleAbandonable : public FNonAbandonableTask
{
public:
	FSimpleAbandonable(const FSimpleDelegate& InThreadDelegate);

	//
	void DoWork();
	//
	FORCEINLINE TStatId GetStatId() const;

protected:
	FSimpleDelegate ThreadDelegate;

};