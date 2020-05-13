#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WorldProperty.h"
#include "WeaponBase.generated.h"

//not sure if this should maybe be an interface instead?
UCLASS(BlueprintType, Blueprintable)
class AWeaponBase : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
		UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere)
		UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere)
		UAnimSequenceBase* FireSequence;
public:
	UFUNCTION()
		UAnimMontage* GetFireMontage();

	UFUNCTION()
		UAnimMontage* GetReloadMontage();

	UFUNCTION()
		UAnimSequenceBase* GetFireSequence();
};