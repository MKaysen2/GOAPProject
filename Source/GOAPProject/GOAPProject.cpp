// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GOAPProject.h"
#include "Modules/ModuleManager.h"


#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebugger.h"
#include "Public/GameplayDebuggerCategory_GOAP.h"
#endif //WITH_GAMEPLAY_DEBUGGER

DEFINE_LOG_CATEGORY(LogGOAPProject)

void FGOAPProjectModule::StartupModule()
{

#if WITH_GAMEPLAY_DEBUGGER

	//@author credits to Orfeas Eleftheriou
	//If the gameplay debugger is available, register the category and notify the editor about the changes
	IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();

	GameplayDebuggerModule.RegisterCategory(
		"GOAPCategory", 
		IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_GOAP::MakeInstance), 
		EGameplayDebuggerCategoryState::EnabledInGameAndSimulate
	);

	GameplayDebuggerModule.NotifyCategoriesChanged();

#endif //WITH_GAMEPLAY_DEBUGGER
}

void FGOAPProjectModule::ShutdownModule()
{

#if WITH_GAMEPLAY_DEBUGGER
	//If the gameplay debugger is available, unregister the category
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.UnregisterCategory("GOAPCategory");
	}
#endif //WITH_GAMEPLAY_DEBUGGER

}

bool FGOAPProjectModule::IsGameModule() const
{
    return true;
}

IMPLEMENT_PRIMARY_GAME_MODULE(FGOAPProjectModule, GOAPProject, "GOAPProject");
