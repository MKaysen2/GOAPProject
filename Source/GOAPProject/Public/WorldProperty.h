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
	SYMBOL_MAX
};


USTRUCT(BlueprintType)
struct GOAPPROJECT_API FWorldProperty 
{
	GENERATED_BODY()
public:

	EWorldKey key = EWorldKey::kIdle;

	enum class Type
	{
		kBool,
		kVariable,
		kObj
	} DataType = Type::kBool;

	union
	{
		bool bValue;
		EWorldKey kValue;
		UObject* objValue; //should make this an actor pointer, not object ptr
	} Data = { false };
	//I believe this should only ever be marked in the planner
	bool bUnsatisfied = false;


	FWorldProperty() = default;
	FWorldProperty(EWorldKey _key, bool _bValue) : key(_key), DataType(Type::kBool)
	{
		
		Data.bValue = _bValue;
	}
	FWorldProperty(EWorldKey _key, EWorldKey varLookup) : key(_key), DataType(Type::kVariable)
	{
		Data.kValue = varLookup;
	}
	FWorldProperty(EWorldKey _key, UObject* objValue) : key(_key), DataType(Type::kObj)
	{
		Data.objValue = objValue;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FWorldProperty& Prop) 
	{
		return (uint32)Prop.key;
	}

	friend bool operator==(const FWorldProperty& lhs, const FWorldProperty& rhs) 
	{
		return lhs.key == rhs.key;
	}

	//eventually this will be moved into == operator after I make keyfuncs
	bool Equals(const FWorldProperty& rhs);

	bool Equals(const FWorldProperty& rhs) const;

	void Apply(const FWorldProperty& Other);

	void MarkSatisfied(bool bNewSatisfied)
	{
		bUnsatisfied = !bNewSatisfied;
	}

	FString ToString() const;

};