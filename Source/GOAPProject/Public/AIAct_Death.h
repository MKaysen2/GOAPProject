#pragma once
#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "AIAct_Death.generated.h"


class AAIController;

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_Death : public UGOAPAction
{
	GENERATED_BODY()
public:
	UAIAct_Death();
	bool VerifyContext() override;
	EActionStatus StartAction() override;
};
