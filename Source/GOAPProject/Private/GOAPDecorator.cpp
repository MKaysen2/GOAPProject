#include "..\Public\GOAPDecorator.h"
#include "..\Public\WorldState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
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
		UE_LOG(LogTemp, Warning, TEXT("No actor found for BB key name"));
		return false;
	}
	
	FAISenseID SightID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
	const FActorPerceptionInfo* Info = AIOwner.GetPerceptionComponent()->GetActorInfo(*EnemyActor);
	if (!Info)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Info found for Actor"));
		return false;
	}

	for (int32 StimID = 0; StimID < Info->LastSensedStimuli.Num(); ++StimID)
	{
		
		if (Info->LastSensedStimuli[StimID].Type == SightID)
		{
			bool bSensed = Info->LastSensedStimuli[StimID].WasSuccessfullySensed();
			float fAge = Info->LastSensedStimuli[StimID].GetAge();
			return (!bSensed && fAge < AgeThreshold);
		}
	}

	return false;
}
