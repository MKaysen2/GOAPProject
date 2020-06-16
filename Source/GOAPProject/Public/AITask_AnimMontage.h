// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/AITask.h"
#include "Engine/EngineTypes.h"
#include "AITask_Operator.h"

#include "AITask_AnimMontage.generated.h"

DECLARE_MULTICAST_DELEGATE(FAnimTaskEndedSignature);

class UAnimMontage;
class UAnimInstance;
class AAIController;
/**
 * Used by the controller to run AnimMontages
 * Looping Montages need a start|loop|end structure
 */
UCLASS()
class GOAPPROJECT_API UAITask_AnimMontage : public UAITask_Operator
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
		UAnimMontage* MontageHandle;

	UPROPERTY()
		UAnimInstance* AnimInstance;
	UPROPERTY()
		int32 LoopMax = 0;

	UPROPERTY()
		int32 LoopCount = 0;

	UPROPERTY()
		bool bLooping = false;

	FTimerHandle MontageSectionTimerHandle;

public:

	static UAITask_AnimMontage* AIAnimMontage(AAIController* Controller, UAnimMontage* MontageToPlay, float InPlayRate = 1.0f, int32 LoopMax = 0);

	void SetUp();

	virtual void Activate() override;

	virtual void ExternalCancel() override;
	FAnimTaskEndedSignature OnMontageTaskEnded;
	
protected:

	void MontageLoop();

	void FinishMontageTask();

	void OnMontageEnded();
};
