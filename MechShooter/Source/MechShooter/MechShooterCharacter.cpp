// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MechShooterCharacter.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AMechShooterCharacter

AMechShooterCharacter::AMechShooterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	//CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	//FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	Mesh = GetMesh();

	PrimaryActorTick.bCanEverTick = true;

	JumpButtonDown = false;
	LandingNow = false;
	Sprinting = false;
	Aiming = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMechShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMechShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMechShooterCharacter::StopJumping);

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

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMechShooterCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMechShooterCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMechShooterCharacter::StartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMechShooterCharacter::StopAiming);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMechShooterCharacter::StartFiring);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMechShooterCharacter::StopFiring);
}

void AMechShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GunBlueprint == NULL) return;
	Gun = GetWorld()->SpawnActor<AGun>(GunBlueprint);
	Gun->AttachToComponent(Mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));
	IsArmed = true;
	//Gun->AnimInstance = Mesh->GetAnimInstance();
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
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMechShooterCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
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
	/*if (Firing == true)
	{
		Gun->Fire();
	}*/
}

void AMechShooterCharacter::StartAiming()
{
	Aiming = true;
}

void AMechShooterCharacter::StopAiming()
{
	Aiming = false;
}

void AMechShooterCharacter::StartFiring()
{
	if ((Gun != NULL) && (IsArmed))
	{
		//Gun->Fire();
		Firing = true;
	}
}

void AMechShooterCharacter::StopFiring()
{
	Firing = false;
}

void AMechShooterCharacter::FireGunProjectile()
{
	Gun->Fire();
}