#pragma once


#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ActuatorInterface.generated.h"

class UAITask_AnimMontage;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActuatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Should return various components actually,
 * then the components can be used to control the animations
 */
class GOAPPROJECT_API IActuatorInterface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//This should spawn an AnimMontage AI Task
	UFUNCTION()
		virtual UAITask_AnimMontage* Act(FName Action)
	{
		return nullptr;
	}

};