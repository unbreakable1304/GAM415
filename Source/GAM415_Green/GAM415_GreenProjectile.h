// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GAM415_GreenProjectile.generated.h"

// Forward declarations to reduce compile dependencies
class USphereComponent;
class UProjectileMovementComponent;

// Represents a projectile that can be fired in the game world,
// which spawns a matching decal (splat) on impact and has a randomized appearance.
UCLASS(config = Game)
class AGAM415_GreenProjectile : public AActor
{
	GENERATED_BODY()

protected:

	// Collision component that handles overlap and hit events.
	// This is the root component and determines the projectile's hit response.
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	// Movement component that provides trajectory logic such as speed, bouncing, and gravity.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	// Mesh component for the visual representation of the projectile.
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ballMesh;

	// Dynamic material instance applied to the projectile mesh at runtime,
	// allowing for color and texture frame customization.
	UPROPERTY()
	UMaterialInstanceDynamic* dmiMat;

	// Randomized color value used both for the projectile mesh and the splat decal.
	UPROPERTY()
	FLinearColor randColor;

	// Randomized frame index used for texture atlas/flipbook animation in materials.
	UPROPERTY()
	float frameNum;

public:

	// Constructor: Sets default values for this projectile.
	AGAM415_GreenProjectile();

	// The material used to render the decal when the projectile hits a surface.
	// Must be a deferred decal material.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
	UMaterialInterface* baseMat = nullptr;

	// The base material assigned to the projectile mesh,
	// which will be turned into a dynamic instance to support real-time parameter changes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	UMaterialInterface* projMat = nullptr;

	// Called when the projectile is spawned or begins play.
	// Used to initialize dynamic material parameters like color and frame.
	virtual void BeginPlay() override;

	// Called when the projectile collides with another object.
	// Spawns a decal at the hit location using the stored color and frame.
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	// Getter for the collision component (used externally if needed).
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	// Getter for the movement component (used externally for configuring behavior).
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};
