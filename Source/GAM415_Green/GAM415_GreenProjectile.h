// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once  // Ensures the header file is only included once per compilation

// Includes core types, math, logging, and Unreal-specific data types
#include "CoreMinimal.h"

// Provides base functionality for all actors that can be placed in the level
#include "GameFramework/Actor.h"

// Required for Unreal Header Tool to process reflection information
#include "GAM415_GreenProjectile.generated.h"

// Forward declarations to reduce dependencies and compile time
class USphereComponent;               // For collision handling
class UProjectileMovementComponent;   // For projectile physics behavior
class UNiagaraSystem;                 // For spawning Niagara particle systems

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

	// Niagara system used to spawn an additional effect on impact (e.g., confetti)
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* colorP;

	// Niagara system used to spawn a splatter effect at the impact point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* splatP;

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
		UPrimitiveComponent* HitComp,     // Component that registered the hit
		AActor* OtherActor,               // Actor that was hit
		UPrimitiveComponent* OtherComp,   // The other component involved in the collision
		FVector NormalImpulse,            // Force of impact
		const FHitResult& Hit             // Full result information from the collision
	);

	// Getter for the collision component (used externally if needed).
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	// Getter for the movement component (used externally for configuring behavior).
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	// === Added: randomized FX parameters (preserve original collision/projectile behavior) ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FX")
	FLinearColor randColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FX", meta=(ClampMin="0", ClampMax="3"))
	int32 frameNum = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FX")
	bool bRandomDecalRotation = true;
};
