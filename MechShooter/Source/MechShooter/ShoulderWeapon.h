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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setup)
	TSubclassOf<class AProjectile> ProjectileClass;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* Muzzle;

	//moving the mesh ot the up position
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponState)
	bool GettingReady;

	//mesh is completely in the up position and can now fire
	//Set True or False by the ShoulderWeapon_AnimBP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponState)
	bool ReadyToFire;

public:
	AShoulderWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Fire() override;

private:
	void MoveUp();

	void MoveDown();

	void Activate();
};
