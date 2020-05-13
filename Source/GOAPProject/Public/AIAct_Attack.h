#pragma once
#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "AIAct_Attack.generated.h"

class AAIController;
class UAnimMontage;

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_Attack : public UGOAPAction
{
	GENERATED_BODY()
public:
	UAIAct_Attack();
	bool VerifyContext(AAIController* controller) override;
	EActionStatus StartAction(AAIController* Controller) override;
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, AAIController* Controller);
	void AbortAction(AAIController* Controller) override;
};
