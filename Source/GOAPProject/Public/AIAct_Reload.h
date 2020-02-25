#pragma once
#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "UObject/NoExportTypes.h"
#include "AIAct_Reload.generated.h"

class AAIController;

UCLASS(BlueprintType)
class GOAPPROJECT_API UAIAct_Reload : public UGOAPAction
{
	GENERATED_BODY()
public:
	UAIAct_Reload();

	bool VerifyContext(AAIController* controller) override;
	void StartAction(AAIController* controller) override;

};