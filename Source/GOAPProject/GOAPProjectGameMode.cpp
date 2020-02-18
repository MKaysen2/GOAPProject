// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GOAPProjectGameMode.h"
#include "GOAPProjectPlayerController.h"
#include "GOAPProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGOAPProjectGameMode::AGOAPProjectGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AGOAPProjectPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}