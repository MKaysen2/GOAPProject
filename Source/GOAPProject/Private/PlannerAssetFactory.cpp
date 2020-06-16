#include "..\Public\PlannerAssetFactory.h"
#include "..\Public\PlannerAsset.h"

UPlannerAssetFactory::UPlannerAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UPlannerAsset::StaticClass();
}

UObject* UPlannerAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UPlannerAsset* NewPlanner = NewObject<UPlannerAsset>(InParent, Class, Name, Flags);
	return NewPlanner;
}
