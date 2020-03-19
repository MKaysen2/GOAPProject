#pragma once

#include "CoreMinimal.h"
#include "Containers/Union.h"
#include "WorldProperty.generated.h"

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
	
	EWorldKey key = EWorldKey::kIdle;
	//I have elected to only use booleans for now as TUnions do not currently work with UE4 reflection
		bool bValue = false;

	//I believe this should only ever be marked in the planner
	bool bSatisfied = false;

	typedef union 
	{
		float fValue;
		int iValue;

	} datatype; 
	datatype MyVar = { 3.0f };

	FWorldProperty() = default;
	FWorldProperty(EWorldKey _key, bool _bValue) : key(_key), bValue(_bValue) {}

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