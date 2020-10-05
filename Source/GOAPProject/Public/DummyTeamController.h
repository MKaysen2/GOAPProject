#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "DummyTeamController.generated.h"

UCLASS(BlueprintType)
class GOAPPROJECT_API ADummyTeamController : public AAIController
{
	GENERATED_BODY()
public:

	ADummyTeamController(const FObjectInitializer& ObjectInitializer);
};