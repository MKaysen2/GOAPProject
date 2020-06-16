// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\AITask_AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

void UAITask_AnimMontage::SetUp()
{
	
}

void UAITask_AnimMontage::Activate()
{
	Super::Activate();
	ACharacter* Character = Cast<ACharacter>(GetAIController()->GetPawn());
	AnimInstance = Character->GetMesh()->GetAnimInstance();
	float MontageDuration = AnimInstance->Montage_Play(MontageHandle);
	/*
	if (bLooping)
	{
		
		float SectionDuration = MontageHandle->GetSectionLength(0);
		OwnerController->GetWorldTimerManager().SetTimer(MontageSectionTimerHandle, this, &UAITask_AnimMontage::MontageLoop, SectionDuration, false);
	}
	else
	{
		OwnerController->GetWorldTimerManager().SetTimer(MontageSectionTimerHandle, this, &UAITask_AnimMontage::OnMontageEnded, MontageDuration, false);
	}
	*/
	MontageEndedDelegate.BindUObject(this, &UAITask_AnimMontage::MontageEndedCallback);
	AnimInstance->Montage_SetBlendingOutDelegate(MontageEndedDelegate, MontageHandle);
}

void UAITask_AnimMontage::ExternalCancel()
{

	AnimInstance = nullptr;
	MontageHandle = nullptr;
	EndTask();

}

void UAITask_AnimMontage::OnDestroy(bool bOwnerEnded)
{
	if (!bOwnerEnded)
	{
		StopMontage();
	}
	Super::OnDestroy(bOwnerEnded);
}

void UAITask_AnimMontage::StopMontage()
{
	if (AnimInstance == nullptr || MontageHandle == nullptr)
	{
		return;
	}
	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageHandle);
	if (MontageInstance)
	{
		MontageInstance->OnMontageBlendingOutStarted.Unbind();
		MontageInstance->OnMontageEnded.Unbind();
		AnimInstance->Montage_Stop(.2f, MontageHandle);
	}
}

void UAITask_AnimMontage::MontageEndedCallback(UAnimMontage* Montage, bool bInterrupted)
{
	EndTask();
}

void UAITask_AnimMontage::MontageLoop()
{
}

void UAITask_AnimMontage::FinishMontageTask()
{
	EndTask();
	//Fire delegate
}

void UAITask_AnimMontage::OnMontageEnded()
{
	FinishMontageTask();
	MontageHandle = nullptr;
	AnimInstance = nullptr;
}
