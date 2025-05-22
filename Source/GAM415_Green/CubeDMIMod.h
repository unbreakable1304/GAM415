#pragma once  // Ensures the header is only included once during compilation

// Includes core engine types and utilities
#include "CoreMinimal.h"

// Provides base functionality for all actors in the game world
#include "GameFramework/Actor.h"

// Provides access to UBoxComponent, a component used for collision detection
#include "Components/BoxComponent.h"

// Provides Niagara particle system spawning functions
#include "NiagaraFunctionLibrary.h"

// Gives access to Niagara particle system components for runtime manipulation
#include "NiagaraComponent.h"

// This generated header must come last for Unreal’s reflection system
#include "CubeDMIMod.generated.h"

// Forward declaration of the Niagara System to reduce compile-time dependencies
class UNiagaraSystem;

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

	// Niagara particle system to spawn upon player overlap — used to visually enhance feedback.
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* colorP;

	// Function triggered when another actor overlaps with the cube's collision box.
	UFUNCTION()
	void OnOverlapBegin(
		class UPrimitiveComponent* overlappedComp,  // The component that was overlapped
		class AActor* OtherActor,                   // The actor that initiated the overlap
		class UPrimitiveComponent* OtherComp,       // The specific component that overlapped
		int32 OtherBodyIndex,                       // Index of the body that overlapped (for multi-body components)
		bool bFromSweep,                            // True if this was a sweep result
		const FHitResult& SweepResult               // Detailed result of the overlap, including hit location
	);
};
