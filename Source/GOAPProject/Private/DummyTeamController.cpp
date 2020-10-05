#include "..\Public\DummyTeamController.h"

ADummyTeamController::ADummyTeamController(const FObjectInitializer& Initializer) : Super(Initializer)
{
	//Just setting it to some random ID so it registers as a hostile
	SetGenericTeamId(FGenericTeamId(10));
}