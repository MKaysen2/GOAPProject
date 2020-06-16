#pragma once

#include "CoreMinimal.h"
#include "Tasks/AITask.h"

#include "AITask_Operator.generated.h"

class AIController;


DECLARE_MULTICAST_DELEGATE_OneParam(FOpTaskEndedSignature, bool);

UCLASS(abstract, EditInlineNew)
class GOAPPROJECT_API UAITask_Operator : public UAITask
{
	GENERATED_BODY()
public:

	FOpTaskEndedSignature OnOperatorEnded;
};