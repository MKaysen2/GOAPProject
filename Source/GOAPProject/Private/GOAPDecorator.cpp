#include "..\Public\GOAPDecorator.h"
#include "..\Public\WorldState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIController.h"

UGOAPDecorator::UGOAPDecorator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UGOAPDec_ShouldFlushOut::UGOAPDec_ShouldFlushOut(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UGOAPDec_ShouldFlushOut::CalcRawConditionValue(AAIController& AIOwner, const FWorldState& WS)
{
	AActor* EnemyActor = Cast<AActor>(AIOwner.GetBlackboardComponent()->GetValueAsObject(BBTargetName));
	if (!EnemyActor)
	{
		return false;
	}
	FActorPerceptionBlueprintInfo Info;
	AIOwner.GetPerceptionComponent()->GetActorsPerception(EnemyActor, Info);

	return true;
}
