// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Particles/ParticleSystemComponent.h"
#include "Missile.generated.h"


UCLASS()
class MECHSHOOTER_API AMissile : public AProjectile
{
	GENERATED_BODY()

public:
	AMissile();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setup)
	class UParticleSystem* TrailingEffect;

	UPROPERTY(VisibleDefaultsOnly, Category = Setup)
	class USceneComponent* TrailingLocation;

	void BeginPlay() override;

	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
};
