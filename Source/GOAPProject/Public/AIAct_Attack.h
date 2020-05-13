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
protected:
	FTimerHandle MontageTimerHandle;

	UPROPERTY()
		AAIController* AIOwner;
public:
	UAIAct_Attack();
	bool VerifyContext(AAIController* controller) override;
	EActionStatus StartAction(AAIController* Controller) override;
	void StopAction(AAIController* Controller) override;
	void OnMontageEnded();
	void AbortAction(AAIController* Controller) override;
};
