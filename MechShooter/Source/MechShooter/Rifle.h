// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Rifle.generated.h"

UCLASS()
class MECHSHOOTER_API ARifle : public AGun
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARifle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	virtual void Fire() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GunBehavior)
	void CreateCrosshair() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = GunBehavior)
	void RemoveCrosshair() override;
};
