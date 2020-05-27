#include "../Public/MontageMapComponent.h"


UMontageMapComponent::UMontageMapComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UMontageMapComponent::AddMontage(FName ActName, UAnimMontage* Montage)
{
	if (!Montage)
	{
		return;
	}
	MontageMap.Add(ActName, Montage);
}

void UMontageMapComponent::RemoveMontage(FName ActName, UAnimMontage* Montage)
{
	MontageMap.Remove(ActName);
}

UAnimMontage* UMontageMapComponent::GetMontageByName(FName ActName)
{
	TWeakObjectPtr<UAnimMontage> Montage = MontageMap.FindRef(ActName);
	if (Montage.IsValid())
	{
		return Montage.Get();
	}
	return nullptr;
}
