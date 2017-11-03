// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shoulder.generated.h"

class AMechShooterCharacter;

UCLASS()
class MECHSHOOTER_API AShoulder : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AShoulder();

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	virtual void Fire() {}

	virtual void Activate(UInputComponent* Input) {}
};
