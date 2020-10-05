#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "DummyTeamCharacter.generated.h"

UCLASS(Blueprintable)
class ADummyTeamCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

		ADummyTeamCharacter(const FObjectInitializer& Initializer);
private:
	FGenericTeamId TeamId;
public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
};