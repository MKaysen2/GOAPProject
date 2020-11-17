#include "..\Public\GOAPDecorator.h"
#include "..\Public\WorldState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "AIController.h"
#include "..\Public\PlannerComponent.h"

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

UGOAPDec_IsKeyOfType::UGOAPDec_IsKeyOfType(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UGOAPDec_IsKeyOfType::CalcRawConditionValue(AAIController& AIOwner, const FWorldState& WS)
{
	UObject* Object = AIOwner.GetBlackboardComponent()->GetValueAsObject(BBTargetName);
	if (!Object)
	{
		return false;
	}
	for (auto& TestClass : AllowedTypes)
	{
		if (Object->GetClass()->IsChildOf(TestClass))
		{
			return true;
		}
	}
	return false;
}

UGOAPDec_DamagedSince::UGOAPDec_DamagedSince(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UGOAPDec_DamagedSince::CalcRawConditionValue(AAIController& AIOwner, const FWorldState& WS)
{
	FAISenseID DamageID = UAISense::GetSenseID(UAISense_Damage::StaticClass());

	const FActorPerceptionInfo* Info = AIOwner.GetPerceptionComponent()->GetFreshestTrace(DamageID);

	if (!Info)
	{
		return false;
	}

	for (int32 StimID = 0; StimID < Info->LastSensedStimuli.Num(); ++StimID)
	{

		if (Info->LastSensedStimuli[StimID].Type == DamageID)
		{
			
			bool bSensed = Info->LastSensedStimuli[StimID].WasSuccessfullySensed();
			float fAge = Info->LastSensedStimuli[StimID].GetAge();
			return (bSensed && fAge < AgeThreshold);
		}
	}
	return false;
}

UGOAPDec_TagCooldown::UGOAPDec_TagCooldown(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UGOAPDec_TagCooldown::CalcRawConditionValue(AAIController& AIOwner, const FWorldState& WS)
{
	UPlannerComponent* PlannerComp = AIOwner.FindComponentByClass<UPlannerComponent>();
	if (!PlannerComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("No plannerComp found"));
		return false;
	}

	float EndTime = PlannerComp->GetTagCooldownEndTime(CooldownTag);
	if (EndTime == 0.f)
	{

		return true;
	}
	
	return (GetWorld()->GetTimeSeconds() >= EndTime);
}

void UGOAPDec_TagCooldown::OnTaskDeactivated(UPlannerComponent& OwnerComp)
{
	OwnerComp.AddTagCooldownDuration(CooldownTag, Duration, false);
}