// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimInstance.h"
#include "Projectile.h"
#include "Gun.generated.h"

class AMechShooterCharacter;

UCLASS()
class MECHSHOOTER_API AGun : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AGun();

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class UBoxComponent* CollisionComp;

	AProjectile* Projectile;

	float WeaponRange;

	float WeaponDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Setup)
	int32 GunType;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace);

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = GunBehavior)
	virtual void Fire() {}

	UFUNCTION(BlueprintCallable, Category = GunBehavior)
	virtual void CreateCrosshair() {}

	UFUNCTION(BlueprintCallable, Category = GunBehavior)
	virtual void RemoveCrosshair() {}
};
