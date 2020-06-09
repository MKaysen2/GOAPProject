#pragma once

#pragma once
#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "AIAct_ThrowGrenade.generated.h"

class AAIController;
class UAnimMontage;
class UAITask_AnimMontage;

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_ThrowGrenade : public UGOAPAction
{
	GENERATED_BODY()
protected:

	virtual void InitEffects() override;

public:
	UAIAct_ThrowGrenade();
	
};