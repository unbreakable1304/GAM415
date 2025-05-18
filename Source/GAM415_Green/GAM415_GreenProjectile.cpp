// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM415_GreenProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

// Constructor: Sets default values and initializes components
AGAM415_GreenProjectile::AGAM415_GreenProjectile()
{
	// Create and configure the collision component (used to detect hits and overlaps)
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);  // Radius of the collision sphere
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile"); // Sets up collision behavior
	CollisionComp->OnComponentHit.AddDynamic(this, &AGAM415_GreenProjectile::OnHit); // Bind hit event

	// Prevent characters from stepping on the projectile
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set this as the root component of the actor
	RootComponent = CollisionComp;

	// Create and attach the visual mesh to the collision sphere
	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball Mesh"));
	ballMesh->SetupAttachment(CollisionComp);

	// Initialize the projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;  // Makes the projectile rotate in the direction it's moving
	ProjectileMovement->bShouldBounce = true;             // Enables bouncing behavior on impact

	// Automatically destroy the projectile after 3 seconds
	InitialLifeSpan = 3.0f;
}

// Called when the projectile is spawned or begins play
void AGAM415_GreenProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Generate a random color using Unreal's math library
	randColor = FLinearColor::MakeRandomColor();

	// Random frame index for flipbook-style texture atlasing (e.g., splats)
	frameNum = FMath::FRandRange(0.f, 3.f);

	// If the material and mesh are valid, create and apply a dynamic material instance
	if (projMat && ballMesh)
	{
		dmiMat = UMaterialInstanceDynamic::Create(projMat, this);
		if (dmiMat)
		{
			// Apply random values to the material parameters
			dmiMat->SetVectorParameterValue("Color", randColor);
			dmiMat->SetScalarParameterValue("Frame", frameNum);

			// Assign the dynamic material to the projectile's mesh
			ballMesh->SetMaterial(0, dmiMat);
		}
	}
	else
	{
		// Log a warning if material assignment failed
		UE_LOG(LogTemp, Warning, TEXT("Projectile material or mesh not assigned!"));
	}
}

// Called when the projectile collides with another actor
void AGAM415_GreenProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Proceed only if we hit a valid actor and have a decal material assigned
	if (OtherActor && baseMat && GetWorld())
	{
		// Calculate where to spawn the decal (slightly offset from the surface)
		FVector DecalLocation = Hit.ImpactPoint + Hit.ImpactNormal * 5.0f;

		// Rotate the decal to align with the surface normal
		FRotator DecalRotation = Hit.ImpactNormal.Rotation();
		DecalRotation.Yaw += FMath::FRandRange(0.f, 360.f); // Randomize decal orientation

		// Randomize decal size for visual variety
		float decalSize = FMath::FRandRange(25.f, 45.f);
		FVector DecalScale(decalSize);

		// Spawn the decal into the world with specified properties
		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			baseMat,
			DecalScale,
			DecalLocation,
			DecalRotation,
			10.0f // Lifetime in seconds
		);

		// If the decal was successfully created, assign the same color and frame as the projectile
		if (Decal)
		{
			UMaterialInstanceDynamic* MatInstance = Decal->CreateDynamicMaterialInstance();
			if (MatInstance)
			{
				MatInstance->SetVectorParameterValue("Color", randColor);
				MatInstance->SetScalarParameterValue("Frame", frameNum);
			}
		}
	}

	// Destroy the projectile after impact
	Destroy();
}
