#pragma once

#include "CoreMinimal.h"
#include "Containers/Union.h"
#include "WorldProperty.generated.h"

//using uint8 breaks this for whatever reason
UENUM(BlueprintType)
enum class EWorldKey : uint8
{
	kAtLocation,
	kIdle,
	kWeaponLoaded,
	kHasWeapon,
	kTargetDead,
	kDisturbanceHandled
};

USTRUCT(BlueprintType)
struct GOAPPROJECT_API FWorldProperty 
{
	GENERATED_BODY()
public:
	
	UPROPERTY()
	EWorldKey key;

	TUnion<bool, AActor*> value;
	UPROPERTY()
	bool bSatisfied;


	FWorldProperty() : key(), value(), bSatisfied(false) {}
	FWorldProperty(const FWorldProperty& Copy) : key(Copy.key), value(Copy.value), bSatisfied(Copy.bSatisfied) {}
	FWorldProperty(EWorldKey _key, bool bValue) : key(_key), value(bValue), bSatisfied(false) {}
	FWorldProperty(EWorldKey _key, AActor* objValue) : key(_key), value(objValue), bSatisfied(false) {}

	friend FORCEINLINE uint32 GetTypeHash(const FWorldProperty& prop) 
	{
		return (uint32)prop.key;
	}

	friend bool operator==(const FWorldProperty& lhs, const FWorldProperty& rhs) 
	{
		return lhs.key == rhs.key;
	}
	
	void MarkSatisfied(bool bNewSatisfied)
	{
		bSatisfied = bNewSatisfied;
	}
};