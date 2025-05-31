// Copyright Epic Games, Inc. All Rights Reserved.

// Include the header file for this projectile class
#include "GAM415_GreenProjectile.h"

// Provides the movement functionality used by projectile actors
#include "GameFramework/ProjectileMovementComponent.h"

// Contains utility functions like random number generation
#include "Kismet/KismetMathLibrary.h"

// Provides the collision component used to detect hits
#include "Components/SphereComponent.h"

// Enables spawning and configuring of decals in the world
#include "Components/DecalComponent.h"

// General gameplay utility functions (spawning actors, sounds, etc.)
#include "Kismet/GameplayStatics.h"

// Functions to spawn Niagara particle systems at runtime
#include "NiagaraFunctionLibrary.h"

// Gives access to and control over Niagara system instances
#include "NiagaraComponent.h"

// Used to create dynamic instances of materials at runtime
#include "Materials/MaterialInstanceDynamic.h"

#include "PerlinProcTerrain.h"


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
	randColor = FLinearColor(
		UKismetMathLibrary::RandomFloatInRange(0.f, 1.f),
		UKismetMathLibrary::RandomFloatInRange(0.f, 1.f),
		UKismetMathLibrary::RandomFloatInRange(0.f, 1.f),
		1.f // Alpha (opacity) set to fully visible
	);

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

				APerlinProcTerrain* pocTerrain = Cast<APerlinProcTerrain>(OtherActor);
				if(pocTerrain)
				{
					// If the hit actor is a terrain, apply the color to the terrain's material
					pocTerrain->AlterMesh(Hit.ImpactPoint);
				}
			}
		}

		// Spawn the Niagara splatter effect at the impact point, if assigned
		if (splatP)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				splatP,
				Hit.ImpactPoint,
				Hit.ImpactNormal.Rotation(),
				FVector(1.f),                      // Scale
				true,                              // Auto destroy
				true,                              // Auto activate
				ENCPoolMethod::AutoRelease         // Efficient pooling
			)->SetNiagaraVariableLinearColor(FString("User.RandomColor"), randColor);
		}

		// If additional particle effects are assigned, spawn them attached to the collision component
		if (OtherActor != nullptr)
		{
			if (colorP)
			{
				UNiagaraComponent* particleComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
					colorP,
					HitComp,
					NAME_None,
					FVector(-20.f, 0.f, 0.f),
					FRotator(0.f),
					EAttachLocation::KeepRelativeOffset,
					true
				);

				// Apply the same color to the spawned Niagara system
				particleComp->SetNiagaraVariableLinearColor(FString("RandomColor"), randColor);

				// Remove the projectile mesh from the scene
				ballMesh->DestroyComponent();

				// Disable collisions to prevent further interactions
				CollisionComp->BodyInstance.SetCollisionProfileName("NoCollision");
			}
		}
	}

	// Destroy the projectile after it impacts an object
	Destroy();
}
