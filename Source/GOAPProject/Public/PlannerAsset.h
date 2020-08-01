#pragma once

#include "CoreMinimal.h"

#include "PlannerAsset.generated.h"

class UGOAPAction;
class UGOAPGoal;
class UPlannerComponent;
class UPlannerService;
class UBlackboardData;

UENUM()
enum class EWSValueType
{
	//WSKey, I will use this eventually to configure effects/preconditions
	Absolute,
	BBKey,
	MAX UMETA(Hidden)
};

USTRUCT(atomic)
struct GOAPPROJECT_API FWSKeyConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		EWorldKey KeyLHS;
	UPROPERTY(EditAnywhere)
		EWSValueType Type = EWSValueType::Absolute;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EWSValueType::BBKey"))
		FName BBKeyName = NAME_None;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Type == EWSValueType::Absolute"))
		uint8 Value;
};

UCLASS(BlueprintType, Blueprintable)
class GOAPPROJECT_API UPlannerAsset : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
		UBlackboardData* BlackboardData;

	UPROPERTY(EditDefaultsOnly)
		TArray<FWSKeyConfig> WSKeyDefaults;

	UPROPERTY(EditDefaultsOnly, Instanced)
		TArray<UGOAPAction*> Actions;

	UPROPERTY(EditDefaultsOnly)
		TArray<TSubclassOf<UPlannerService>> Services;

	UPROPERTY(EditDefaultsOnly, Instanced)
		TArray<UGOAPGoal*> Goals;
	//Ring buffer for running tasks only needs to be max plan size + 1 (for cancelling actions)
	UPROPERTY(EditDefaultsOnly)
		uint32 MaxPlanSize = 5;
	friend class UPlannerComponent;
};