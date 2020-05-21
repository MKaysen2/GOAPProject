// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\AITask_AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

UAITask_AnimMontage* UAITask_AnimMontage::AIAnimMontage(AAIController* Controller, UAnimMontage* MontageToPlay, float InPlayRate, int32 LoopMax)
{
	ACharacter* ControlledPawn = Controller ? Cast<ACharacter>(Controller->GetPawn()) : nullptr;
	UAITask_AnimMontage* Task = (ControlledPawn ? UAITask::NewAITask<UAITask_AnimMontage>(*Controller, EAITaskPriority::High) : nullptr);

	if (Task)
	{
		Task->LoopCount = 0;
		Task->LoopMax = LoopMax;
		Task->MontageHandle = MontageToPlay;
		Task->bLooping = (LoopMax > 0);
		Task->AnimInstance = ControlledPawn->GetMesh()->GetAnimInstance();
	}
	return Task;
}

void UAITask_AnimMontage::Activate()
{
	Super::Activate();
	float MontageDuration = AnimInstance->Montage_Play(MontageHandle);
	if (bLooping)
	{
		float SectionDuration = MontageHandle->GetSectionLength(0);
		OwnerController->GetWorldTimerManager().SetTimer(MontageSectionTimerHandle, this, &UAITask_AnimMontage::MontageLoop, SectionDuration, false);
	}
	else
	{
		OwnerController->GetWorldTimerManager().SetTimer(MontageSectionTimerHandle, this, &UAITask_AnimMontage::OnMontageEnded, MontageDuration, false);
	}
}

void UAITask_AnimMontage::ExternalCancel()
{
	EndTask();
	OwnerController->GetWorldTimerManager().ClearTimer(MontageSectionTimerHandle);
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.2f, MontageHandle);
	}
	AnimInstance = nullptr;
	MontageHandle = nullptr;
}

void UAITask_AnimMontage::MontageLoop()
{
	FName CurrentSection = AnimInstance->Montage_GetCurrentSection(MontageHandle);
	int32 CurrentSectionID = MontageHandle->GetSectionIndex(CurrentSection);
	++LoopCount;
	if (LoopCount > LoopMax)
	{
		AnimInstance->Montage_SetNextSection(CurrentSection, FName("end"), MontageHandle);
		int32 EndSectionID = AnimInstance->Montage_GetNextSectionID(MontageHandle, CurrentSectionID);
		float Duration = MontageHandle->GetSectionLength(EndSectionID);
		OwnerController->GetWorldTimerManager().SetTimer(MontageSectionTimerHandle, this, &UAITask_AnimMontage::OnMontageEnded, Duration, false);
	}
	else
	{
		float Duration = MontageHandle->GetSectionLength(CurrentSectionID);
		OwnerController->GetWorldTimerManager().SetTimer(MontageSectionTimerHandle, this, &UAITask_AnimMontage::MontageLoop, Duration, false);
	}
}

void UAITask_AnimMontage::FinishMontageTask()
{
	EndTask();
	//Fire delegate
	OnMontageTaskEnded.Broadcast();
}

void UAITask_AnimMontage::OnMontageEnded()
{
	FinishMontageTask();
	MontageHandle = nullptr;
	AnimInstance = nullptr;
}
