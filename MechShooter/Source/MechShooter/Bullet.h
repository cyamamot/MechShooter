// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Bullet.generated.h"


UCLASS()
class MECHSHOOTER_API ABullet : public AProjectile
{
	GENERATED_BODY()
	
public:
	ABullet();

protected:
	void BeginPlay() override;
	
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
