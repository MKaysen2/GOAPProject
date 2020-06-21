#include "../Public/GameplayDebuggerCategory_GOAP.h"

#if WITH_GAMEPLAY_DEBUGGER


#include "GameFramework/Pawn.h"

FGameplayDebuggerCategory_GOAP::FGameplayDebuggerCategory_GOAP()
{

}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_GOAP::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_GOAP());
}

void FGameplayDebuggerCategory_GOAP::CollectData(APlayerController* OwnerPC, AActor* DebugActor) 
{
	APawn* MyPawn = Cast<APawn>(DebugActor);
	if (MyPawn)
	{

	}
}

#endif //WITH_GAMEPLAY_DEBUGGER