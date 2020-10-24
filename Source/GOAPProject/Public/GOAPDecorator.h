#pragma once

#include "CoreMinimal.h"
#include "WorldProperty.h"

#include "GOAPDecorator.generated.h"

class AAIController;
struct FWorldState;

UCLASS(abstract, EditInlineNew)
class GOAPPROJECT_API UGOAPDecorator : public UObject
{
	GENERATED_BODY()

public:
	UGOAPDecorator(const FObjectInitializer& ObjectInitializer);

	virtual bool CalcRawConditionValue(AAIController& AIOwner, const FWorldState& WS) { return false; } //default behavior
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