#pragma once

#include "CoreMinimal.h"
#include "Containers/Union.h"
#include "WorldProperty.generated.h"

UENUM(BlueprintType)
enum class EWorldKey : uint8
{
	kIdle = 0,
	kAtLocation,
	kWeaponLoaded,
	kHasWeapon,
	kTargetDead,
	kDisturbanceHandled,
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
		kVar
	} DataType = Type::kBool;

	union
	{
		bool bValue;
		uint8 nValue;
	} Data = { false };
	//I believe this should only ever be marked in the planner
	bool bUnsatisfied = false;


	FWorldProperty() = default;
	FWorldProperty(EWorldKey _key, bool _bValue) : key(_key) 
	{
		DataType = Type::kBool;
		Data.bValue = _bValue;
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
	bool Equals(const FWorldProperty& rhs)
	{
		if(rhs.DataType != this->DataType)
		{
			return false;
		}
		switch (DataType)
		{
		case Type::kBool:
			return rhs.Data.bValue == this->Data.bValue;
			break;

		default:
			return false;
		}
			
	}

	bool Equals(const FWorldProperty& rhs) const
	{
		if (rhs.DataType != this->DataType)
		{
			return false;
		}
		switch (DataType)
		{
		case Type::kBool:
			return rhs.Data.bValue == this->Data.bValue;
			break;

		default:
			return false;
		}

	}

	void Apply(const FWorldProperty& Other);

	void MarkSatisfied(bool bNewSatisfied)
	{
		bUnsatisfied = !bNewSatisfied;
	}
};