// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM415_GreenGameMode.h"
#include "GAM415_GreenCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGAM415_GreenGameMode::AGAM415_GreenGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
