#pragma once
#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "AIAct_Attack.generated.h"

class AAIController;

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_Attack : public UGOAPAction
{
	GENERATED_BODY()
public:
	UAIAct_Attack();
	bool VerifyContext(AAIController* controller) override;
	void StartAction(AAIController* Controller) override;
};
