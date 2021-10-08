// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThreadTestGameMode.h"
#include "ThreadTestHUD.h"
#include "ThreadTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AThreadTestGameMode::AThreadTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AThreadTestHUD::StaticClass();
}
