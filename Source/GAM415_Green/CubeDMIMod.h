// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CubeDMIMod.generated.h"

// This class represents a cube actor with dynamic material properties
// that change color when the player overlaps with it.
UCLASS()
class GAM415_GREEN_API ACubeDMIMod : public AActor
{
	GENERATED_BODY()

public:
	// Constructor: Sets default values for this actor's properties.
	ACubeDMIMod();

protected:
	// Called once when the game starts or when the actor is spawned into the level.
	virtual void BeginPlay() override;

public:
	// Called every frame. Used if runtime logic is needed.
	virtual void Tick(float DeltaTime) override;

	// The collision box component that defines the interaction area for overlap events.
	UPROPERTY(EditAnywhere)
	UBoxComponent* boxComp;

	// The static mesh that visually represents the cube in the world.
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* cubeMesh;

	// The base material used to create a dynamic material instance at runtime.
	UPROPERTY(EditAnywhere)
	UMaterialInterface* baseMat;

	// The dynamic material instance used to modify visual properties like color in real-time.
	UPROPERTY()
	UMaterialInstanceDynamic* dmiMat;

	// Function triggered when another actor overlaps with the cube's collision box.
	UFUNCTION()
	void OnOverlapBegin(
		class UPrimitiveComponent* overlappedComp,
		class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
