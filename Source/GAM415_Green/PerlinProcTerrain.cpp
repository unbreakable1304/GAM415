// Fill out your copyright notice in the Description page of Project Settings.

#include "PerlinProcTerrain.h"
#include "ProceduralMeshComponent.h"           // For procedural mesh generation
#include "ProcMeshFromStatic.h"                // Custom utility for static mesh conversion (assumed)
#include "ProcPlane.h"                         // Custom plane generation helper (assumed)
#include "KismetProceduralMeshLibrary.h"       // Provides mesh manipulation utilities like slicing and copying

// Sets default values
APerlinProcTerrain::APerlinProcTerrain()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create the procedural mesh component and attach it to the root (or pending root) component
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Procedural Mesh");
	ProcMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void APerlinProcTerrain::BeginPlay()
{
	Super::BeginPlay();

	// Generate the vertex positions and UVs using Perlin noise
	CreateVertices();

	// Define triangle indices to form mesh faces from the generated vertices
	CreateTriangles();

	// Create a mesh section from the generated data and apply the default material
	ProcMesh->CreateMeshSection(sectionID, Vertices, Triangles, Normals, UV0, UpVertexColors, TArray<FProcMeshTangent>(), true);
	ProcMesh->SetMaterial(0, Mat);
}

// Called every frame (disabled in constructor, but available if needed)
void APerlinProcTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Alters the mesh dynamically based on an impact point (e.g., for terrain deformation)
void APerlinProcTerrain::AlterMesh(FVector impactPoint)
{
	for (int i = 0; i < Vertices.Num(); i++)
	{
		// Get local-space vector from actor origin to impact point
		FVector tempVector = impactPoint - this->GetActorLocation();

		// If the vertex is within a specified radius of the impact, lower it by Depth
		if (FVector(Vertices[i] - tempVector).Size() < radius)
		{
			Vertices[i] = Vertices[i] - Depth;

			// Update the mesh with the new vertex positions
			ProcMesh->UpdateMeshSection(sectionID, Vertices, Normals, UV0, UpVertexColors, TArray<FProcMeshTangent>());
		}
	}
}

// Creates vertices and UVs for a grid-based terrain using Perlin noise for height variation
void APerlinProcTerrain::CreateVertices()
{
	for (int X = 0; X <= XSize; X++)
	{
		for (int Y = 0; Y <= YSize; Y++)
		{
			// Generate a Perlin noise value based on scaled X and Y positions
			float Z = FMath::PerlinNoise2D(FVector2D(X * NoiseScale + 0.1, Y * NoiseScale + 0.1)) * ZMultiplier;

			// Debug: display the height value in the viewport for inspection
			GEngine->AddOnScreenDebugMessage(-1, 999.0f, FColor::Yellow, FString::Printf(TEXT("Z: %f"), Z));

			// Add the vertex and corresponding UV coordinate
			Vertices.Add(FVector(X * Scale, Y * Scale, Z));
			UV0.Add(FVector2D(X * UVScale, Y * UVScale));
		}
	}
}

// Creates triangle indices for connecting the grid vertices into a mesh surface
void APerlinProcTerrain::CreateTriangles()
{
	int Vertex = 0;

	for (int X = 0; X < XSize; X++)
	{
		for (int Y = 0; Y < YSize; Y++)
		{
			// Define two triangles for each quad in the grid
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 1);

			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 2);
			Triangles.Add(Vertex + YSize + 1);

			Vertex++;
		}

		// Skip to the next row of vertices
		Vertex++;
	}
}
