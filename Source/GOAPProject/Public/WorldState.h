#pragma once
#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "UObject/NoExportTypes.h"
#include "WorldProperty.h"
#include "WorldState.generated.h"

struct FStateNode;
class FGameplayDebuggerCategory;

DECLARE_LOG_CATEGORY_EXTERN(LogWS, Warning, All);

USTRUCT(BlueprintType)
struct GOAPPROJECT_API FWorldState
{
	
	GENERATED_BODY()

private: 
	UPROPERTY(VisibleAnywhere)
		TArray<uint8> State;

	uint32 CachedTypeHash;
public:
	FWorldState();

	void SetProp(EWorldKey Key, uint8 Value);

	const uint8& GetProp(EWorldKey Key) const;
	
	//Hamming distance by default, o.w. the absolute value of the difference
	uint8 HeuristicDist(EWorldKey Key, uint8 Value, bool bHamming=true) const;

	void CacheArrayTypeHash();

	uint32 GetArrayTypeHash()
	{
		return CachedTypeHash;
	}

	void LogWS() const;
#if WITH_GAMEPLAY_DEBUGGER
	void DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const;
#endif //WITH_GAMEPLAY_DEBUGGER

};

