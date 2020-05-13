#pragma once


#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ActuatorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActuatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actuation of animation system
 * Implemented by Controllers, probably
 */
class GOAPPROJECT_API IActuatorInterface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
		virtual void Act(FName Action)
	{

	}

};