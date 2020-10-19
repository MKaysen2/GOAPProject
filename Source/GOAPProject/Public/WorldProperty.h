#pragma once

#include "CoreMinimal.h"
#include "WorldProperty.generated.h"

#define GETENUMSTRING(etype, evalue) ( (FindObject<UEnum>(nullptr, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(nullptr, TEXT(etype), true)->GetNameStringByIndex((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )

struct FWorldProperty;
struct FAISymEffect;

UENUM(BlueprintType)
enum class EWorldKey : uint8
{
	kIdle = 0,
	kAtLocation,
	kTargetDead,
	kDisturbanceHandled,
	kUsingObject,
	kTargetSuppressed,
	kDead,
	kInDanger,
	SYMBOL_MAX	UMETA(Hidden)
};

//maybe should remove Neq
UENUM()
enum class ESymbolTest : uint8
{
	Eq,
	Gt,
	Lt,
	Geq,
	Leq,

	MAX UMETA(Hidden)
};

UENUM()
enum class ESymbolOp : uint8
{
	Set,
	Inc,
	Dec,

	MAX UMETA(Hidden)
};
UENUM()
namespace EPlannerTaskFinishedResult
{
	enum Type
	{
		Success,
		Failure,
		Aborted
	};
}

UENUM()
enum class EActionStatus : uint8
{
	Active,
	Inactive,
	Aborting
};

//most of this is from the HTN module and I'd like to switch over to using that if I can, eventually
namespace FPlannerWSOperations
{
	typedef bool(*FConditionFunctionPtr)(const uint8&, const uint8& RHS);
	typedef void(*FOperationFunctionPtr)(uint8&, const uint8& RHS);
	typedef bool(*FOpCheckFunctionPtr)(const uint8&, const uint8& RHS);

}

//TODO: should update the naming for this
USTRUCT(BlueprintType)
struct GOAPPROJECT_API FWorldProperty 
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
	EWorldKey Key = EWorldKey::kIdle;

	//TODO: edit condition
	UPROPERTY(EditAnywhere)
		ESymbolTest Comparator = ESymbolTest::Eq;

	UPROPERTY(EditAnywhere)
		EWorldKey KeyRHS = EWorldKey::SYMBOL_MAX;

	UPROPERTY(EditAnywhere)
	uint8 Value;


	FWorldProperty() = default;
	FWorldProperty(EWorldKey _Key, bool bValue) : Key(_Key), Value(bValue) {}
	FWorldProperty(const EWorldKey& Key, const uint8& nValue) : Key(Key), Value(nValue) {}

	bool IsRHSAbsolute() const
	{
		return KeyRHS >= EWorldKey::SYMBOL_MAX;
	}

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

	bool Eval(uint8 Pre) const
	{
		//is there a better way to do this?
		switch (Comparator)
		{
		case ESymbolTest::Eq:
			return Pre == Value;
			//break;
		case ESymbolTest::Gt:
			return Pre > Value;
		case ESymbolTest::Geq:
			return Pre >= Value;
		case ESymbolTest::Lt:
			return Pre < Value;
		case ESymbolTest::Leq:
			return Pre <= Value;
		default:
			return false;
		}
	}

	bool Evaluate(const uint8* Values) const
	{
		uint8 RHSValue = (IsRHSAbsolute()) ? Value : Values[uint8(KeyRHS)];

		uint8 LHSValue = Values[uint8(Key)];
		switch (Comparator)
		{
		case ESymbolTest::Eq:
			return LHSValue == Value;
			//break;
		case ESymbolTest::Gt:
			return LHSValue > Value;
		case ESymbolTest::Geq:
			return LHSValue >= Value;
		case ESymbolTest::Lt:
			return LHSValue < Value;
		case ESymbolTest::Leq:
			return LHSValue <= Value;
		default:
			return false;
		}
	}
	//Haven't figured out Neq, may have to remove it.
	uint8 MinSatisfyVal(uint8* Values) const
	{
		uint8 RHSValue = (IsRHSAbsolute()) ? Value : Values[uint8(Key)];
		switch (Comparator)
		{
		case ESymbolTest::Eq:
			//fallthrough
		case ESymbolTest::Geq:
			//fallthrough
		case ESymbolTest::Leq:
			return Value;
		// Must constrain designer somehow
		case ESymbolTest::Gt:
			return (Value < 255) ? Value + 1 : 255;
		case ESymbolTest::Lt:
			return Value > 0 ? Value - 1 : 0;
		default:
			return 0;
		}
	}
};

/** 
*/
USTRUCT(Atomic, BlueprintType)
struct GOAPPROJECT_API FAISymEffect
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	EWorldKey Key;

	UPROPERTY(EditAnywhere)
		ESymbolOp Op = ESymbolOp::Set;

	UPROPERTY(EditAnywhere)
		EWorldKey KeyRHS = EWorldKey::SYMBOL_MAX;
	UPROPERTY(EditAnywhere)
	uint8 Value;

	//Whether or not the effect is expected as a result of a sensor update
	UPROPERTY(EditAnywhere)
		bool bExpected = false;

	FAISymEffect() : Key(EWorldKey::kIdle), Value(0) {}
	FAISymEffect(const EWorldKey& Key, const uint8& Value) : Key(Key), Value(Value) {}
	FAISymEffect(const EWorldKey& KeyLHS, const EWorldKey& KeyRHS) : Key(KeyLHS), KeyRHS(KeyRHS) {}

	bool IsRHSAbsolute() const
	{
		return KeyRHS >= EWorldKey::SYMBOL_MAX;
	}

	//Returns whether effect was applied successfully
	bool Apply(uint8* Values) const;

	//the set operation's inverse changes the value back to the "Ground truth", i.e. the value in 
	//the goal state, so that the hash of the world state will remain consistent
	//However, the actual value of the property could technically have been anything
	uint8 Backward(uint8 Post, uint8 Pre) const
	{
		switch (Op)
		{
		case ESymbolOp::Set:
			return Pre;
		case ESymbolOp::Inc:
			return (Post >= Value) ? Post - Value : 0;
		case ESymbolOp::Dec:
			return (Post <= (255 - Value)) ? Post + Value : 255;
		default:
			return Pre;
		}
	}

	//GroundValue is whatever the value in the GoalState is
	bool Revert(uint8* Values) const;
};