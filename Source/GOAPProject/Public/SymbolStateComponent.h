#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Templates/SharedPointer.h"
#include "WorldProperty.h"
#include "WorldState.h"

#include "SymbolStateComponent.generated.h"

UCLASS()
class USymbolStateComponent : public UActorComponent
{
	GENERATED_BODY()

private:
		TSharedRef<FWorldState> WorldState;

public:
	USymbolStateComponent();
	UFUNCTION()
		void SetProp(const EWorldKey& Key, const uint8& Value);

	UFUNCTION()
		uint8 GetProp(const EWorldKey& Key) const;
};