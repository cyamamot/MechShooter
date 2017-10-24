// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MechShooterGameMode.h"
#include "MechShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMechShooterGameMode::AMechShooterGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/MechCharacter/MechCharacter_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
