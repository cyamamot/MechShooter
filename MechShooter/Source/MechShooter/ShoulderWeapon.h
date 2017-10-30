// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shoulder.h"
#include "Projectile.h"
#include "Animation/AnimInstance.h"
#include "ShoulderWeapon.generated.h"


UCLASS()
class MECHSHOOTER_API AShoulderWeapon : public AShoulder
{
	GENERATED_BODY()
	
public:
	AProjectile* Projectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setup)
	TSubclassOf<class AProjectile> ProjectileClass;

	//moving the mesh ot the up position
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponState)
	bool GettingReady;

	//mesh is completely in the up position and can now fire
	//Set True or False by the ShoulderWeapon_AnimBP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponState)
	bool ReadyToFire;

	float WeaponRange;

public:
	AShoulderWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void EmptyFunction();

	void Fire() override;

private:
	void MoveUp();

	void MoveDown();

	void Activate(UInputComponent* Input);
};
