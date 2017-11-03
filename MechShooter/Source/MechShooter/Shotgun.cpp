// Fill out your copyright notice in the Description page of Project Settings.

#include "Shotgun.h"
#include "GameFramework/PlayerController.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AShotgun::AShotgun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	RootComponent = FP_Gun;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComp"));
	CollisionComp->SetupAttachment(RootComponent);

	WeaponRange = 2000.0f;
	NumberOfShots = 10;
	ShotSpread = 200.0f;
	GunType = 1;
}

// Called when the game starts or when spawned
void AShotgun::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
/*void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}*/

void AShotgun::Fire()
{
	// Now send a trace from the end of our gun to see if we should hit anything
	APlayerController* PlayerController = Cast<APlayerController>(((APawn*)GetOwner())->GetController());

	// Calculate the direction of fire and the start location for trace
	FVector CamLoc;
	FRotator CamRot;
	PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector ShootDir = CamRot.Vector();

	FVector StartTrace = FVector::ZeroVector;
	if (PlayerController)
	{
		FRotator UnusedRot;
		PlayerController->GetPlayerViewPoint(StartTrace, UnusedRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		StartTrace = StartTrace + ShootDir * ((GetActorLocation() - StartTrace) | ShootDir);
	}
	else return;
	const FVector EndTrace = (StartTrace + ShootDir * WeaponRange);
	for (int32 i = 0; i < NumberOfShots; i++)
	{
		WeaponDamage = FMath::RandRange(30.0f, 50.0f);
		float RandX = FMath::RandRange(-ShotSpread, ShotSpread);
		float RandY = FMath::RandRange(-ShotSpread, ShotSpread);
		float RandZ = FMath::RandRange(-ShotSpread, ShotSpread);
		// Calculate endpoint of trace
		const FVector EndTraceSpread = EndTrace + FVector(RandX, RandY, RandZ);
		// Check for impact
		const FHitResult CameraImpact = WeaponTrace(StartTrace, EndTraceSpread);
		// try and fire a projectile
		if (ProjectileClass != NULL)
		{
			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				
				FVector SocketLocation = FP_Gun->GetSocketLocation("Muzzle");
				FRotator LookAtRotation;
				if (CameraImpact.Actor != NULL)
				{
					LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SocketLocation, CameraImpact.ImpactPoint);
				}
				else
				{
					LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SocketLocation, EndTraceSpread);
				}
				FTransform Transform(LookAtRotation, SocketLocation, FVector(1.0f, 1.0f, 1.0f));
				Projectile = World->SpawnActor<AProjectile>(ProjectileClass, Transform, ActorSpawnParams);
				Projectile->SetOwner(GetOwner());
				Projectile->Damage = WeaponDamage;
				Projectile->SetLifeSpan(1.5f);
			}
		}
	}
}


