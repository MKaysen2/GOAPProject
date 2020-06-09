#include "..\Public\AIAct_Attack.h"
#include "..\Public\WeaponBase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "..\Public\WorldProperty.h"
#include "..\Public\AITask_AnimMontage.h"
#include "..\Public\MontageMapComponent.h"

UAIAct_Attack::UAIAct_Attack() :
	Super(5)
{
}

void UAIAct_Attack::InitEffects()
{
	Effects.Reserve(1);
	AddEffect(EWorldKey::kTargetDead, FAISymEffect(EWorldKey::kTargetDead, true));
}
