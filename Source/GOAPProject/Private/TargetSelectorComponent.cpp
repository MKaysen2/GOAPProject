// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/TargetSelectorComponent.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "BehaviorTree\BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

// Sets default values for this component's properties
UTargetSelectorComponent::UTargetSelectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = .1f;
	// ...
}


// Called when the game starts
void UTargetSelectorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UTargetSelectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!AIOwner)
	{
		return;
	}
	//(extremely) basic implementation: look through perceived actors, select closest and set blackboard
	UAIPerceptionComponent* PerceptionComp = AIOwner->GetPerceptionComponent();
	if (!PerceptionComp)
	{
		return;
	}
	float MaxDistance = TNumericLimits<float>::Max();
	APawn* ControlledPawn = AIOwner->GetPawn();
	if (!ControlledPawn || !PerceptionComp)
	{
		return;
	}

	TArray<AActor*> PerceivedActors;
	PerceptionComp->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
	AActor* Target = nullptr;
	FVector Location(ControlledPawn->GetActorLocation());
	float MinDistance = TNumericLimits<float>::Max();
	for (auto* Actor : PerceivedActors)
	{
		if (!Actor)
		{
			continue;
		}
		float TestDistance = ControlledPawn->GetDistanceTo(Actor);
		if (TestDistance < MinDistance)
		{
			Target = Actor;
			MinDistance = TestDistance;
		}
	}
	UBlackboardComponent* BBComp = AIOwner->GetBlackboardComponent();
	BBComp->SetValueAsObject(FName("CombatTarget"), Target);
}

void UTargetSelectorComponent::OnRegister()
{
	Super::OnRegister();

	AIOwner = Cast<AAIController>(GetOuter());
}