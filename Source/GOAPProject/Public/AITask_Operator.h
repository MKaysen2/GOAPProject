#pragma once

#include "CoreMinimal.h"
#include "Tasks/AITask.h"

#include "AITask_Operator.generated.h"

class AIController;

UENUM()
enum class EOpStatus
{
	InProgress,
	Succeeded,
	Failed,
	Aborted
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOpTaskEndedSignature, bool);

UCLASS(abstract, EditInlineNew)
class GOAPPROJECT_API UAITask_Operator : public UAITask
{
	GENERATED_BODY()
		EOpStatus OpStatus;
public:
	void SetOpStatus(EOpStatus NewStatus) { OpStatus = NewStatus; }
	EOpStatus GetOpStatus() const { return OpStatus; }
	FOpTaskEndedSignature OnOperatorEnded;
};