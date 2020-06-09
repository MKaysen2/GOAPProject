// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIAct_DodgeShuffle.generated.h"

class UAITask_MoveTo;
/**
 * 
 */
UCLASS()
class GOAPPROJECT_API UAIAct_DodgeShuffle : public UGOAPAction
{
	GENERATED_BODY()
	
protected:

	UPROPERTY()
		UAITask_MoveTo* MoveTask;

	UPROPERTY()
		FVector GoalLocation;
public:
	UAIAct_DodgeShuffle();
	bool VerifyContext() override;
	
};
