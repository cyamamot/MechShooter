// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MechShooterCharacter.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

AMechShooterCharacter::AMechShooterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 60.f;
	BaseLookUpRate = 60.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	Mesh = GetMesh();

	FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPCamera"));
	FPCamera->SetupAttachment(RootComponent);

	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSkeletalMesh"));
	FPMesh->SetupAttachment(FPCamera);

	PrimaryActorTick.bCanEverTick = true;
	JumpButtonDown = false;
	LandingNow = false;
	Sprinting = false;
	Aiming = false;
	Firing = false;
	if (Gun == NULL) IsCurrentlyArmed = false;
	else IsCurrentlyArmed = true;
	CurrentGunType = 0;
}


void AMechShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("BottomButton", IE_Pressed, this, &AMechShooterCharacter::Jump);
	PlayerInputComponent->BindAction("BottomButton", IE_Released, this, &AMechShooterCharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMechShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMechShooterCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMechShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMechShooterCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMechShooterCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMechShooterCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMechShooterCharacter::OnResetVR);

	PlayerInputComponent->BindAction("LeftStickButton", IE_Pressed, this, &AMechShooterCharacter::StartSprinting);
	PlayerInputComponent->BindAction("LeftStickButton", IE_Released, this, &AMechShooterCharacter::StopSprinting);

	PlayerInputComponent->BindAction("LeftTrigger", IE_Pressed, this, &AMechShooterCharacter::StartAiming);
	PlayerInputComponent->BindAction("LeftTrigger", IE_Released, this, &AMechShooterCharacter::StopAiming);

	PlayerInputComponent->BindAction("RightTrigger", IE_Pressed, this, &AMechShooterCharacter::StartFiring);
	PlayerInputComponent->BindAction("RightTrigger", IE_Released, this, &AMechShooterCharacter::StopFiring);

	PlayerInputComponent->BindAction("LeftButton", IE_Pressed, this, &AMechShooterCharacter::EquipWeapon);

	PlayerInputComponent->BindAction("LeftBumper", IE_Pressed, this, &AMechShooterCharacter::LeftShoulderFire);
}

void AMechShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GunBlueprint != NULL)
	{
		Gun = GetWorld()->SpawnActor<AGun>(GunBlueprint);
		Gun->AttachToComponent(Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));
		Gun->SetOwner(this);
		Gun->FP_Gun->SetOwnerNoSee(true);
		IsCurrentlyArmed = true;
	//}
	//if (FPGunBlueprint != NULL)
	//{
		//FPGun = GetWorld()->SpawnActor<AGun>(FPGunBlueprint);
		FPGun = GetWorld()->SpawnActor<AGun>(GunBlueprint);
		FPGun->AttachToComponent(FPMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));
		FPGun->SetOwner(this);
		CurrentGunType = FPGun->GunType;
	}
	if (LeftShoulderBlueprint != NULL)
	{
		LeftShoulder = GetWorld()->SpawnActor<AShoulder>(LeftShoulderBlueprint);
		LeftShoulder->AttachToComponent(Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("LeftShoulderSocket"));
		LeftShoulder->SetOwner(this);
		LeftShoulder->Mesh->SetOwnerNoSee(true);
	}
	if (RightShoulderBlueprint != NULL)
	{
		RightShoulder = GetWorld()->SpawnActor<AShoulder>(RightShoulderBlueprint);
		RightShoulder->AttachToComponent(Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightShoulderSocket"));
		RightShoulder->SetOwner(this);
		RightShoulder->Mesh->SetOwnerNoSee(true);
	}
	Mesh->SetOwnerNoSee(true);
}

void AMechShooterCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMechShooterCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMechShooterCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMechShooterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMechShooterCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMechShooterCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMechShooterCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMechShooterCharacter::Jump()
{
	JumpButtonDown = true;
	LandingNow = false;
	ACharacter::Jump();
}

void AMechShooterCharacter::StopJumping()
{
	JumpButtonDown = false;
	LandingNow = false;
	ACharacter::StopJumping();
}

void AMechShooterCharacter::Landed(const FHitResult& Hit)
{
	JumpButtonDown = false;
	LandingNow = true;
	Super::Landed(Hit);
}

void AMechShooterCharacter::StartSprinting()
{
	if ((Controller != NULL) && (GetCharacterMovement()->IsFalling() != true))
	{
		const FVector MoveDirection = GetVelocity();
		FRotator Rot = GetActorRotation();
		FVector LocalMoveDirection = Rot.UnrotateVector(MoveDirection);
		LocalMoveDirection.Normalize();
		if (LocalMoveDirection[0] >= 0.85)
		{
			Sprinting = true;
		}
	}
}

void AMechShooterCharacter::StopSprinting()
{
	Sprinting = false;
}

void AMechShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetCharacterMovement()->IsFalling()) Sprinting = false;
	else if ((Controller != NULL) && (Sprinting == true))
	{
		const FVector MoveDirection = GetVelocity();
		FRotator Rot = GetActorRotation();
		FVector LocalMoveDirection = Rot.UnrotateVector(MoveDirection);
		LocalMoveDirection.Normalize();
		if (LocalMoveDirection[0] >= 0.85)
		{
			Sprinting = true;
		}
		else Sprinting = false;
	}
}

void AMechShooterCharacter::ReplaceBinding(FName ActionName)
{
	for (int32 i = InputComponent->GetNumActionBindings() - 1; i >= 0; i--)
	{
		if (InputComponent->GetActionBinding(i).ActionName.IsEqual(ActionName))
		{
			InputComponent->RemoveActionBinding(i);
		}
	}

}

void AMechShooterCharacter::StartAiming()
{
	if (Gun != NULL)
	{
		Aiming = true;
	}
}

void AMechShooterCharacter::StopAiming()
{
	if (Gun != NULL)
	{
		Aiming = false;
	}
}

void AMechShooterCharacter::StartFiring()
{
	if (Gun != NULL)
	{
		Firing = true;
	}
}

void AMechShooterCharacter::StopFiring()
{
	Firing = false;
}

void AMechShooterCharacter::EquipWeapon()
{
	if ((IsCurrentlyArmed == true) && (Gun != NULL))
	{
		IsCurrentlyArmed = false;
	}
	else if ((IsCurrentlyArmed == false) && (Gun != NULL))
	{
		IsCurrentlyArmed = true;
	}
}

//Called in AnimBP by the firing animation sequence so bullet only fires during certain part of animation
/*void AMechShooterCharacter::FireGunProjectile()
{
	Gun->Fire();
	FPGun->Fire();
}*/

//Called in AnimBP by equipping animation sequence so gun is placed in correct socket at correct frame of animation
void AMechShooterCharacter::HolsterUnholsterWeapon()
{
	if ((IsCurrentlyArmed == false) && (Gun != NULL))
	{
		Gun->AttachToComponent(Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponHolsterSocket"));
		FPGun->AttachToComponent(FPMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponHolsterSocket"));
	}
	else if ((IsCurrentlyArmed == true) && (Gun != NULL))
	{
		Gun->AttachToComponent(Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));
		FPGun->AttachToComponent(FPMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));
	}
}

void AMechShooterCharacter::LeftShoulderFire()
{
	AShoulderWeapon* Left = dynamic_cast<AShoulderWeapon*>(LeftShoulder);
	if (Left != NULL)
	{
		LeftShoulder->Activate(InputComponent);
	}
}

void AMechShooterCharacter::ToggleVisibility()
{
	if (Gun != NULL)
	{
		if (Gun->FP_Gun->bOwnerNoSee == false) Gun->FP_Gun->SetOwnerNoSee(true);
		else if (Gun->FP_Gun->bOwnerNoSee == true) Gun->FP_Gun->SetOwnerNoSee(false);
	}
	if (FPGun != NULL)
	{
		if (FPGun->FP_Gun->bOwnerNoSee == false) FPGun->FP_Gun->SetOwnerNoSee(true);
		else if (FPGun->FP_Gun->bOwnerNoSee == true) FPGun->FP_Gun->SetOwnerNoSee(false);
	}
	if (LeftShoulder != NULL)
	{
		if (LeftShoulder->Mesh->bOwnerNoSee == false) LeftShoulder->Mesh->SetOwnerNoSee(true);
		else if (LeftShoulder->Mesh->bOwnerNoSee == true) LeftShoulder->Mesh->SetOwnerNoSee(false);
	}
	if (RightShoulder != NULL)
	{
		if (RightShoulder->Mesh->bOwnerNoSee == false) RightShoulder->Mesh->SetOwnerNoSee(true);
		else if (RightShoulder->Mesh->bOwnerNoSee == true) RightShoulder->Mesh->SetOwnerNoSee(false);
	}
	if (Mesh != NULL)
	{
		if (Mesh->bOwnerNoSee == false) Mesh->SetOwnerNoSee(true);
		else if (Mesh->bOwnerNoSee == true) Mesh->SetOwnerNoSee(false);
	}
	if (FPMesh != NULL)
	{
		if (FPMesh->bOwnerNoSee == false) FPMesh->SetOwnerNoSee(true);
		else if (FPMesh->bOwnerNoSee == true) FPMesh->SetOwnerNoSee(false);
	}
}

