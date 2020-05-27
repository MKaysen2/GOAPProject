#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MontageMapComponent.generated.h"

class AAIController;
class UAnimMontage;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GOAPPROJECT_API UMontageMapComponent : public UActorComponent
{
	GENERATED_BODY()

		//Can make this a multimap too 
		TMap<FName, TWeakObjectPtr<UAnimMontage>> MontageMap;

public:
	// Sets default values for this component's properties
	UMontageMapComponent();

	UFUNCTION(BlueprintCallable)
	void AddMontage(FName ActName, UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable)
		void RemoveMontage(FName ActName, UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable)
		UAnimMontage* GetMontageByName(FName ActName);

};