// Fill out your copyright notice in the Description page of Project Settings.

#include "ProcPlane.h"
#include "ProceduralMeshComponent.h"  // Provides runtime mesh generation and rendering

// Sets default values
AProcPlane::AProcPlane()
{
	// Disable Tick as this actor doesn't require per-frame logic
	PrimaryActorTick.bCanEverTick = false;

	// Create the procedural mesh component and assign a name for editor visibility
	procMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Proc Mesh");
}

// Called when the game starts or when spawned
void AProcPlane::BeginPlay()
{
	Super::BeginPlay();
}

// Called when the actor is placed or spawned in the editor
void AProcPlane::PostActorCreated()
{
	Super::PostActorCreated();

	// Construct the plane mesh
	CreateMesh();

	// Apply material if one is set
	if (PlaneMat)
	{
		procMesh->SetMaterial(0, PlaneMat);
	}
}

// Called when the actor is loaded from disk (e.g., on level load)
void AProcPlane::PostLoad()
{
	Super::PostLoad();

	// Reconstruct the mesh and reapply material (if needed)
	CreateMesh();

	if (PlaneMat)
	{
		procMesh->SetMaterial(0, PlaneMat);
	}
}

// Called every frame (Tick is disabled by default, but method is here for extensibility)
void AProcPlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Creates the mesh section for the procedural plane using existing vertex and triangle data
void AProcPlane::CreateMesh()
{
	procMesh->CreateMeshSection(
		0,                    // Section index
		Vertices,             // Vertex positions
		Triangles,            // Triangle indices
		TArray<FVector>(),    // Normals (can be calculated or left empty)
		UV0,                  // UV coordinates for texturing
		TArray<FColor>(),     // Vertex colors (optional)
		TArray<FProcMeshTangent>(), // Tangents (optional)
		true                  // Enable collision
	);
}
