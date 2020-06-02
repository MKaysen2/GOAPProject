#pragma once

#include "CoreMinimal.h"
#include "Containers/Union.h"
#include "WorldProperty.generated.h"

#define GETENUMSTRING(etype, evalue) ( (FindObject<UEnum>(nullptr, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(nullptr, TEXT(etype), true)->GetNameStringByIndex((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )

UENUM(BlueprintType)
enum class EWorldKey : uint8
{
	kIdle = 0,
	kAtLocation,
	kWeaponLoaded,
	kHasWeapon,
	kTargetDead,
	kDisturbanceHandled,
	kUsingObject,
	kTargetSuppressed,
	kDead,
	kInDanger,
	//kPathBlocked
	SYMBOL_MAX
};


USTRUCT(BlueprintType)
struct GOAPPROJECT_API FWorldProperty 
{
	GENERATED_BODY()
public:

	EWorldKey Key = EWorldKey::kIdle;

	uint8 Value;


	FWorldProperty() = default;
	FWorldProperty(EWorldKey _Key, bool bValue) : Key(_Key), Value(bValue) {}
	FWorldProperty(const EWorldKey& Key, const uint8& nValue) : Key(Key), Value(nValue) {}

	friend FORCEINLINE uint32 GetTypeHash(const FWorldProperty& Prop) 
	{
		return (uint32)Prop.Key;
	}

	uint32 GetValueTypeHash() const
	{
		return GetTypeHash(Value);
	}
	friend bool operator==(const FWorldProperty& lhs, const FWorldProperty& rhs) 
	{
		return lhs.Value == rhs.Value;
	}

};

/** 
*/
USTRUCT(BlueprintType)
struct GOAPPROJECT_API FAISymEffect
{
	GENERATED_BODY()
public:
	EWorldKey Key;
	uint8 Value;

	FAISymEffect() : Key(EWorldKey::kIdle), Value(0) {}
	FAISymEffect(const EWorldKey& Key, const uint8& Value) : Key(Key), Value(Value) {}

	uint8 Forward(uint8 Pre) const
	{
		return Value;
	}

	//the set operation's inverse changes the value back to the "Ground truth", i.e. the value in 
	//the goal state, so that the hash of the world state will remain consistent
	//However, the actual value of the property could technically have been anything
	uint8 Backward(uint8 Post, uint8 Pre) const
	{
		return Pre;
	}
};