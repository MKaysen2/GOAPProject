// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FGOAPProjectModule : public IModuleInterface
{

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	virtual bool IsGameModule() const override;
};
DECLARE_LOG_CATEGORY_EXTERN(LogGOAPProject, Log, All);
