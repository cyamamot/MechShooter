// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "ShoulderWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MECHSHOOTER_API AShoulderWeapon : public AGun
{
	GENERATED_BODY()
	
public:
	AShoulderWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Fire();
	
};
