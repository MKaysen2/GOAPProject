#pragma once

#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "AIAct_ReactDamage.generated.h"

class AAIController;
class UAnimMontage;
class UAITask_AnimMontage;

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_ReactDamage : public UGOAPAction
{
	GENERATED_BODY()
protected:

	UPROPERTY()
		UAnimMontage* CachedMontage;

	UPROPERTY()
		UAITask_AnimMontage* MontageTaskHandle;


public:
	UAIAct_ReactDamage();
	bool VerifyContext() override;
	EActionStatus StartAction() override;
	void StopAction() override;
	void AbortAction() override;
};