#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GOAPGoal.h"
#include "GOAPCharacterBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AGOAPCharacterBase : public ACharacter 
{
	GENERATED_BODY()
protected:
	UPROPERTY()
		TArray<UGOAPGoal*> goals;
public:
	AGOAPCharacterBase();
	UFUNCTION()
		virtual void RegisterGoals(TArray<UGOAPGoal*>& OutArray);
	UFUNCTION()
	void BeginPlay() override;
	UFUNCTION()
		void PostInitProperties() override;
	UFUNCTION()
	int idlenum() {
		return 1;
	}

	UFUNCTION(BlueprintImplementableEvent)
		void Attack();

	UFUNCTION(BlueprintImplementableEvent)
		void Reload();

	UFUNCTION(BlueprintImplementableEvent)
		void Equip();
};