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

	enum class Type
	{
		kBool,
		kVariable
	} DataType = Type::kBool;

	uint8 nValue;
	//I believe this should only ever be marked by the planner
	bool bUnsatisfied = false;


	FWorldProperty() = default;
	FWorldProperty(EWorldKey _Key, bool _bValue) : Key(_Key), DataType(Type::kBool)
	{
		nValue = _bValue;
	}

	//Planner assumes const properties in world states - only use variable types in actions for now
	FWorldProperty(EWorldKey _Key, EWorldKey varLookup) : Key(_Key), DataType(Type::kVariable)
	{
		nValue = (uint8)varLookup;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FWorldProperty& Prop) 
	{
		return (uint32)Prop.Key;
	}

	uint32 GetValueTypeHash() const
	{
		return HashCombine(GetTypeHash(DataType), GetTypeHash(nValue));
	}
	friend bool operator==(const FWorldProperty& lhs, const FWorldProperty& rhs) 
	{
		return lhs.Key == rhs.Key;
	}

	//eventually this will be moved into == operator after I make Keyfuncs
	bool Equals(const FWorldProperty& rhs);

	bool Equals(const FWorldProperty& rhs) const;

	void Apply(const FWorldProperty& Other);

	void MarkSatisfied(bool bNewSatisfied)
	{
		bUnsatisfied = !bNewSatisfied;
	}

	FString ToString() const;

};

//Copyright Epic Games, Inc.All Rights Reserved.
static uint32 GetTypeHash(const TArray<FWorldProperty>& Array)
{
	uint32 Seed = 0;
	for (const auto& Elem : Array)
		Seed ^= Elem.GetValueTypeHash() + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
	return Seed;
}