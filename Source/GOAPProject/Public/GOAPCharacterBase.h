#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WorldProperty.h"
#include "AIController.h"
#include "GOAPGoal.h"
#include "GOAPCharacterBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GOAPPROJECT_API AGOAPCharacterBase : public ACharacter 
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

	UFUNCTION( BlueprintCallable)
		void OnAnimState(const EAnimProps& AnimToPlay);

	UFUNCTION(BlueprintImplementableEvent)
		void Attack();

	UFUNCTION(BlueprintImplementableEvent)
		void Reload();

	UFUNCTION(BlueprintImplementableEvent)
		void Equip();
};