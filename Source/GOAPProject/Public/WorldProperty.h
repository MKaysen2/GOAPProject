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
	
	UPROPERTY(BlueprintReadWrite)
	EWorldKey key;
	//I have elected to only use booleans for now as TUnions do not currently work with UE4 reflection
	UPROPERTY(BlueprintReadWrite)
		bool bValue;

	//I believe this should only ever be marked in the planner
	UPROPERTY()
	bool bSatisfied;


	FWorldProperty() : key(), bValue(), bSatisfied(false) {}
	FWorldProperty(const FWorldProperty& Copy) : key(Copy.key), bValue(Copy.bValue), bSatisfied(Copy.bSatisfied) {}
	FWorldProperty(EWorldKey _key, bool bValue) : key(_key), bValue(bValue), bSatisfied(false) {}

	friend FORCEINLINE uint32 GetTypeHash(const FWorldProperty& Prop) 
	{
		return (uint32)Prop.key;
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