#include "../Public/AIAct_ThrowGrenade.h"
#include "../Public/AITask_AnimMontage.h"
#include "AIController.h"
#include "../Public/MontageMapComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UAIAct_ThrowGrenade::UAIAct_ThrowGrenade() :
	Super(2)
{
}

void UAIAct_ThrowGrenade::InitEffects()
{
	Effects.Reserve(1);
	AddEffect(EWorldKey::kTargetDead, FAISymEffect(EWorldKey::kTargetDead, true));
}
