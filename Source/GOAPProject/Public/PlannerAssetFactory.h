#pragma once
#include "CoreMinimal.h"
#include "Factories/Factory.h"

#include "PlannerAssetFactory.generated.h"

UCLASS()
class GOAPPROJECT_API UPlannerAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UPlannerAssetFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};