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
	kTargetDead
};

UENUM(BlueprintType)
enum class EAnimProps : uint8
{
	kAct_Attack,
	kAct_Equip,
	kAct_Reload,
};

USTRUCT(BlueprintType)
struct GOAPPROJECT_API FWorldProperty 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		EWorldKey key;

	TUnion<bool, AActor*> value;

	FWorldProperty() : key(), value() {}
	FWorldProperty(EWorldKey _key, bool bValue) : key(_key), value(bValue) {}
	FWorldProperty(EWorldKey _key, AActor* objValue) : key(_key), value(objValue) {}

	friend FORCEINLINE uint32 GetTypeHash(const FWorldProperty& prop) 
	{
		return (uint32)prop.key;
	}

	friend bool operator==(const FWorldProperty& lhs, const FWorldProperty& rhs) 
	{
		return lhs.key == rhs.key;
	}
};