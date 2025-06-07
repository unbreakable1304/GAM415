// Fill out your copyright notice in the Description page of Project Settings.

#include "ProcMeshFromStatic.h"
#include "KismetProceduralMeshLibrary.h"  // Provides utilities to extract mesh data from static meshes

// Sets default values
AProcMeshFromStatic::AProcMeshFromStatic()
{
	// Disable Tick since this actor doesn't need per-frame updates
	PrimaryActorTick.bCanEverTick = false;

	// Create the procedural mesh component which will render the mesh at runtime
	procMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Proc Mesh");

	// Create the static mesh component that holds the source mesh data
	baseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");

	// Set the procedural mesh as the root component
	RootComponent = procMesh;

	// Attach the static mesh to the procedural mesh (for reference only, not rendered)
	baseMesh->SetupAttachment(procMesh);
}

// Called when the game starts or when spawned
void AProcMeshFromStatic::BeginPlay()
{
	Super::BeginPlay();
}

// Called when the actor is created in the editor or dynamically spawned
void AProcMeshFromStatic::PostActorCreated()
{
	Super::PostActorCreated();

	// Attempt to extract and apply mesh data from the static mesh
	GetMeshData();
}

// Called when the actor is loaded from disk (e.g., when loading a level)
void AProcMeshFromStatic::PostLoad()
{
	Super::PostLoad();

	// Extract and apply static mesh data to the procedural mesh
	GetMeshData();
}

// Extracts mesh section data from the static mesh and applies it to the procedural mesh
void AProcMeshFromStatic::GetMeshData()
{
	UStaticMesh* mesh = baseMesh->GetStaticMesh();

	if (mesh)
	{
		// Copy mesh section data (vertices, triangles, UVs, etc.) from the static mesh
		UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(mesh, 0, 0, Vertices, Triangles, Normals, UV0, Tangents);

		// Update the procedural mesh with the extracted data (this only updates existing sections)
		procMesh->UpdateMeshSection(0, Vertices, Normals, UV0, UpVertexColors, Tangents);

		// Fully create the mesh section if not already created
		CreateMesh();
	}
}

// Creates a new mesh section on the procedural mesh component
void AProcMeshFromStatic::CreateMesh()
{
	if (baseMesh)
	{
		// Create the procedural mesh section from the copied data
		procMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, UpVertexColors, Tangents, true);
	}
}

// Called every frame (disabled in constructor, but implemented for flexibility)
void AProcMeshFromStatic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
