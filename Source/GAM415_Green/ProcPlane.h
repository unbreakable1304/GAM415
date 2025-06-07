// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProcPlane.generated.h"

// Forward declaration (note: case correction applied below)
class UProceduralMeshComponent;

/**
 * AProcPlane
 *
 * A simple actor that generates a custom procedural plane mesh at runtime.
 * Mesh data (vertices, triangles, UVs) is provided via exposed properties.
 */
UCLASS()
class GAM415_GREEN_API AProcPlane : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProcPlane();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called after actor is placed or spawned in editor
	virtual void PostActorCreated() override;

	// Called when the actor is loaded from disk (e.g., on level load)
	virtual void PostLoad() override;

public:
	// Called every frame (not used unless Tick is enabled in constructor)
	virtual void Tick(float DeltaTime) override;

	// Vertices defining the shape of the procedural plane
	UPROPERTY(EditAnywhere)
	TArray<FVector> Vertices;

	// Triangle indices defining how vertices form faces
	UPROPERTY(EditAnywhere)
	TArray<int> Triangles;

	// UV mapping coordinates for applying textures
	UPROPERTY(EditAnywhere)
	TArray<FVector2D> UV0;

	// Material to apply to the generated plane mesh
	UPROPERTY(EditAnywhere)
	UMaterialInterface* PlaneMat;

	// Creates the procedural mesh using the provided data
	UFUNCTION()
	void CreateMesh();

private:
	// The procedural mesh component responsible for rendering the mesh
	UProceduralMeshComponent* procMesh;
};

