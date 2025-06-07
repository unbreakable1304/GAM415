// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PerlinProcTerrain.generated.h"

// Forward declarations to reduce include dependencies
class UProceduralMeshComponent;   // Used to generate terrain mesh at runtime
class UMaterialInterface;         // Base material for rendering the generated mesh

/**
 * APerlinProcTerrain
 *
 * Actor that generates a grid-based procedural terrain mesh using Perlin noise for height data.
 * Supports runtime deformation through mesh updates based on impact points.
 */
UCLASS()
class GAM415_GREEN_API APerlinProcTerrain : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APerlinProcTerrain();

	// Number of grid units along the X-axis (horizontal resolution)
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	int XSize = 0;

	// Number of grid units along the Y-axis (vertical resolution)
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	int YSize = 0;

	// Height multiplier applied to Perlin noise results
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float ZMultiplier = 1.0f;

	// Frequency scaling factor for Perlin noise sampling
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	float NoiseScale = 1.0f;

	// Distance between adjacent grid vertices
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.000001))
	float Scale = 0;

	// UV coordinate scale factor (controls tiling of material)
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.000001))
	float UVScale = 0;

	// Radius of influence for mesh deformation (used in AlterMesh)
	UPROPERTY(EditAnywhere)
	float radius;

	// Amount to displace vertices downward when affected by deformation
	UPROPERTY(EditAnywhere)
	FVector Depth;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Material to apply to the procedural mesh
	UPROPERTY(EditAnywhere)
	UMaterialInterface* Mat;

public:
	// Called every frame (disabled in constructor, but implemented for flexibility)
	virtual void Tick(float DeltaTime) override;

	// Alters the mesh at runtime by displacing vertices near an impact point
	UFUNCTION()
	void AlterMesh(FVector impactPoint);

private:
	// Runtime-generated mesh component
	UProceduralMeshComponent* ProcMesh;

	// Stores vertex positions for the terrain mesh
	TArray<FVector> Vertices;

	// Stores triangle indices to define the mesh surface
	TArray<int> Triangles;

	// UV mapping coordinates for texturing the mesh
	TArray<FVector2D> UV0;

	// Stores normal vectors (can be calculated if needed)
	TArray<FVector> Normals;

	// Stores vertex colors (optional)
	TArray<FColor> VertexColors;

	// Used to store updated vertex colors after mesh alteration (if needed)
	TArray<FColor> UpVertexColors;

	// ID for the mesh section (allows for multiple sections if extended)
	int sectionID = 0;

	// Generates vertex positions and UVs using Perlin noise
	void CreateVertices();

	// Creates triangle indices from the vertex grid
	void CreateTriangles();
};
