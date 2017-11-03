// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class MECHSHOOTER_API AShotgun : public AGun
{
	GENERATED_BODY()
	
public:
	AShotgun();

protected:
	virtual void BeginPlay() override;

	int32 NumberOfShots;

	float ShotSpread;

public:
	//virtual void Tick(float DeltaTime) override;

	virtual void Fire() override;	
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GunBehavior)
	void CreateCrosshair() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GunBehavior)
	void RemoveCrosshair() override;
};
