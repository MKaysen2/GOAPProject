#include "..\Public\DummyTeamCharacter.h"


ADummyTeamCharacter::ADummyTeamCharacter(const FObjectInitializer& Initializer)
{
	SetGenericTeamId(10);
}

void ADummyTeamCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (TeamId != NewTeamID)
	{
		TeamId = NewTeamID;
	}
}
