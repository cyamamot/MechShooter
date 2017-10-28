// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Missile.generated.h"

/**
 * 
 */
UCLASS()
class MECHSHOOTER_API AMissile : public AProjectile
{
	GENERATED_BODY()

public:
	AMissile();

protected:
	AActor* FX;

	void BeginPlay() override;

	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
};
