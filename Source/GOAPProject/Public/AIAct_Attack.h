#pragma once
#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "AIAct_Attack.generated.h"

class AAIController;
class UAnimMontage;
class UAITask_AnimMontage;

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_Attack : public UGOAPAction
{
	GENERATED_BODY()
protected:

	UPROPERTY()
		AAIController* AIOwner;
	UPROPERTY()
		UAITask_AnimMontage* TaskHandle;
public:
	UAIAct_Attack();
	bool VerifyContext(AAIController* controller) override;
	EActionStatus StartAction(AAIController* Controller) override;
	void StopAction(AAIController* Controller) override;
	void OnMontageEnded();
	void OnMontageLoop();
	void AbortAction(AAIController* Controller) override;
};
