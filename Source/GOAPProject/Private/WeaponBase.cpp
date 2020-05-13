#include "..\Public\WeaponBase.h"

UAnimMontage* AWeaponBase::GetFireMontage()
{
	return FireMontage;
}

UAnimMontage* AWeaponBase::GetReloadMontage()
{
	return ReloadMontage;
}

UAnimSequenceBase* AWeaponBase::GetFireSequence()
{
	return FireSequence;
}
