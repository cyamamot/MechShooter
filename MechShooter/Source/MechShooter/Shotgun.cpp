// Fill out your copyright notice in the Description page of Project Settings.

#include "Shotgun.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

AShotgun::AShotgun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = true;
	RootComponent = FP_Gun;

	WeaponRange = 750.0f;
	NumberOfShots = 10;
	ShotSpread = 30.0f;
	GunType = 1;
}

// Called when the game starts or when spawned
void AShotgun::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

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
	const FVector EndTrace = (StartTrace + ShootDir * WeaponRange);
	for (int32 i = 0; i < NumberOfShots; i++)
	{
		float RandX = FMath::RandRange(0.0f, ShotSpread);
		float RandY = FMath::RandRange(0.0f, ShotSpread);
		float RandZ = FMath::RandRange(0.0f, ShotSpread);
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
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SocketLocation, CameraImpact.ImpactPoint);

				if ((CameraImpact.GetActor() != NULL) && (CameraImpact.GetComponent() != NULL))
				{
					FTransform Transform(LookAtRotation, SocketLocation, FVector(1.0f, 1.0f, 1.0f));
					Projectile = World->SpawnActor<AProjectile>(ProjectileClass, Transform, ActorSpawnParams);
				}
				else
				{
					//FRotator ControlRotation = ((APawn*)GetOwner())->GetControlRotation();



					FVector Forward = EndTrace - SocketLocation;
					FVector ToShot = EndTraceSpread - SocketLocation;
					FVector Axis = FVector::CrossProduct(Forward, ToShot);
					float Angle = FMath::DegreesToRadians(acosf(FVector::DotProduct(Forward, ToShot)));

					UE_LOG(LogTemp, Warning, TEXT("%s    %s"), *(Forward.ToString()), *(ToShot.ToString()));
					UE_LOG(LogTemp, Warning, TEXT("%f"), FVector::DotProduct(Forward, ToShot));




					FRotator ControlRotation(FQuat(Axis, Angle));




					FTransform Transform(ControlRotation, SocketLocation, FVector(1.0f, 1.0f, 1.0f));
					Projectile = World->SpawnActor<AProjectile>(ProjectileClass, Transform, ActorSpawnParams);
				}
			}
		}
	}
}


