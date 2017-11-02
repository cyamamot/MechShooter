// Fill out your copyright notice in the Description page of Project Settings.

#include "ShoulderWeapon.h"
#include "MechShooterCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AShoulderWeapon::AShoulderWeapon()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShoulderMesh"));
	RootComponent = Mesh;

	WeaponRange = 5000.0f;

	GettingReady = false;
	ReadyToFire = false;
}

void AShoulderWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (Mesh != NULL)
	{
		Mesh->bCastDynamicShadow = true;
		Mesh->CastShadow = true;
		Mesh->bCastHiddenShadow = true;
	}
}

void AShoulderWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShoulderWeapon::Fire()
{
	APlayerController* PlayerController = Cast<APlayerController>(((APawn*)GetOwner())->GetController());
	FVector CamLoc;
	FRotator CamRot;
	PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector ShootDir = CamRot.Vector();

	FVector StartTrace = FVector::ZeroVector;
	if (PlayerController)
	{
		FRotator UnusedRot;
		PlayerController->GetPlayerViewPoint(StartTrace, UnusedRot);
		StartTrace = StartTrace + ShootDir * ((GetActorLocation() - StartTrace) | ShootDir);
	}
	const FVector EndTrace = StartTrace + ShootDir * WeaponRange;
	const FHitResult CameraImpact = WeaponTrace(StartTrace, EndTrace);
	if ((ProjectileClass != NULL) && (GettingReady == true) && (ReadyToFire == true))
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			FVector SocketLocation = Mesh->GetSocketLocation("MuzzleSocket");
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SocketLocation, CameraImpact.ImpactPoint);
			if ((CameraImpact.GetActor() != NULL) && (CameraImpact.GetComponent() != NULL))
			{
				FTransform Transform(LookAtRotation, SocketLocation, FVector(1.0f, 1.0f, 1.0f));
				Projectile = World->SpawnActor<AProjectile>(ProjectileClass, Transform, ActorSpawnParams);
				Projectile->SetOwner(GetOwner());
			}
			else
			{
				FRotator ControlRotation = ((APawn*)GetOwner())->GetControlRotation();
				FTransform Transform(ControlRotation, SocketLocation, FVector(1.0f, 1.0f, 1.0f));
				Projectile = World->SpawnActor<AProjectile>(ProjectileClass, Transform, ActorSpawnParams);
				Projectile->SetOwner(GetOwner());
			}
		}
	}
}

void AShoulderWeapon::MoveUp()
{
	if (GettingReady == false)
	{
		GettingReady = true;
	}
}

void AShoulderWeapon::MoveDown()
{
	if (GettingReady == true)
	{
		GettingReady = false;
	}
}

void AShoulderWeapon::Activate(UInputComponent* Input)
{
	AMechShooterCharacter* Owner = (AMechShooterCharacter*)GetOwner();
	if ((Owner == NULL) || (Owner->GetCharacterMovement()->IsFalling() == true)) return;
	if (GettingReady == false)
	{
		Owner->ToggleVisibility();
		MoveUp();

		Owner->ReplaceBinding(FName("LeftTrigger"));
		Input->BindAction("LeftTrigger", IE_Pressed, this, &AShoulderWeapon::Fire);
		Input->BindAction("LeftTrigger", IE_Released, this, &AShoulderWeapon::EmptyFunction);
	}
	else if (GettingReady == true)
	{
		Owner->ToggleVisibility();
		MoveDown();

		Owner->ReplaceBinding(FName("LeftTrigger"));
		Input->BindAction("LeftTrigger", IE_Pressed, Owner, &AMechShooterCharacter::StartAiming);
		Input->BindAction("LeftTrigger", IE_Released, Owner, &AMechShooterCharacter::StopAiming);
	}
}

FHitResult AShoulderWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace)
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams);
	return Hit;
}

void AShoulderWeapon::EmptyFunction()
{

}




