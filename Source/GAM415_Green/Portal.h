// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/BoxComponent.h"
#include "Portal.generated.h"

// Forward declaration of the player character class to avoid circular dependency
class GAM415_GreenCharacter;

UCLASS()
class GAM415_GREEN_API APortal : public AActor
{
	GENERATED_BODY()

public:
	// Constructor: Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when the actor is spawned into the world
	virtual void BeginPlay() override;

public:
	// Called every frame (if ticking is enabled)
	virtual void Tick(float DeltaTime) override;

	// The visual mesh for the portal (usually a frame or ring)
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* mesh;

	// Scene capture component to render the view through the portal
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* sceneCapture;

	// Render target used by the scene capture to output the portal view
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* renderTarget;

	// Box collision component to detect when actors overlap with the portal
	UPROPERTY(EditAnywhere)
	UBoxComponent* boxComp;

	// Reference to the other portal in the pair
	UPROPERTY(EditAnywhere)
	APortal* OtherPortal;

	// Material that will display the portal's render target
	UPROPERTY(EditAnywhere)
	UMaterialInterface* mat;

	// Function called when another actor begins overlapping with this portal
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OlverLappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Resets the player's teleportation state after a short delay
	UFUNCTION()
	void SetBool(AGAM415_GreenCharacter* playerChar);

	// Updates the scene capture location and rotation to simulate a view through the portal
	UFUNCTION()
	void UpdatePortals();
};
