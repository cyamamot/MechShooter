// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gun.h"
#include "Shoulder.h"
#include "ShoulderWeapon.h"
#include "MechShooterCharacter.generated.h"

UCLASS(config=Game)
class AMechShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment)
	AGun* Gun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment)
	AGun* FPGun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment)
	AShoulder* LeftShoulder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment)
	AShoulder* RightShoulder;

	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FPMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPCamera;


public:
	AMechShooterCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ButtonStates)
	bool JumpButtonDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerState)
	bool LandingNow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ButtonStates)
	bool Sprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ButtonStates)
	bool Aiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setup)
	TSubclassOf<class AGun> GunBlueprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setup)
	TSubclassOf<class AGun> FPGunBlueprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setup)
	TSubclassOf<class AShoulder> LeftShoulderBlueprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Setup)
	TSubclassOf<class AShoulder> RightShoulderBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerState)
	bool Firing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerState)
	bool IsCurrentlyArmed;

	//UFUNCTION(BlueprintCallable, Category = GunBehavior)
	//void FireGunProjectile();

	UFUNCTION(BlueprintCallable, Category = GunBehavior)
	void HolsterUnholsterWeapon();

	void ToggleVisibility();

	void ReplaceBinding(FName ActionName);

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void Jump();

	void StopJumping();

	void Landed(const FHitResult& Hit) override;

	void StartSprinting();

	void StopSprinting();

	void StartAiming();

	void StopAiming();

	void StartFiring();

	void StopFiring();

	void EquipWeapon();

	void LeftShoulderFire();

	void Tick(float DeltaTime);


	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);


protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay();

};

