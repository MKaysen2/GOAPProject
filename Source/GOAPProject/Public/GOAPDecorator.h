#pragma once

#include "CoreMinimal.h"
#include "WorldProperty.h"
#include "GameplayTagContainer.h"

#include "GOAPDecorator.generated.h"

class AAIController;
class UPlannerComponent;
struct FWorldState;

UCLASS(abstract, EditInlineNew)
class GOAPPROJECT_API UGOAPDecorator : public UObject
{
	GENERATED_BODY()

public:
	UGOAPDecorator(const FObjectInitializer& ObjectInitializer);

	virtual bool CalcRawConditionValue(AAIController& AIOwner, const FWorldState& WS) { return false; } //default behavior
	virtual void OnTaskDeactivated(UPlannerComponent& OwnerComp) {}
};

UCLASS()
class GOAPPROJECT_API UGOAPDec_ShouldFlushOut : public UGOAPDecorator
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		FName BBTargetName;
	
	UPROPERTY(EditAnywhere)
		float AgeThreshold;
public:
	UGOAPDec_ShouldFlushOut(const FObjectInitializer& ObjectInitializer);

	virtual bool CalcRawConditionValue(AAIController& AIOwner, const FWorldState& WS) override;
};

UCLASS()
class GOAPPROJECT_API UGOAPDec_IsKeyOfType : public UGOAPDecorator
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		FName BBTargetName;

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<UObject>> AllowedTypes;

public:
	UGOAPDec_IsKeyOfType(const FObjectInitializer& ObjectInitializer);

	virtual bool CalcRawConditionValue(AAIController& AIOwner, const FWorldState& WS) override;
};

UCLASS()
class GOAPPROJECT_API UGOAPDec_DamagedSince : public UGOAPDecorator
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		float AgeThreshold;
public:
	UGOAPDec_DamagedSince(const FObjectInitializer& ObjectInitializer);

	virtual bool CalcRawConditionValue(AAIController& AIOwner, const FWorldState& WS) override;
};

UCLASS()
class GOAPPROJECT_API UGOAPDec_TagCooldown : public UGOAPDecorator
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		FGameplayTag CooldownTag;
		
	UPROPERTY(EditAnywhere)
		float Duration;
public:
	UGOAPDec_TagCooldown(const FObjectInitializer& ObjectInitializer);
	virtual bool CalcRawConditionValue(AAIController& AIOwner, const FWorldState& WS) override;

	virtual void OnTaskDeactivated(UPlannerComponent& OwnerComp) override;
};