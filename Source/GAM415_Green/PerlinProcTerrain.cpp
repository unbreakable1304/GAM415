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


void APerlinProcTerrain::Regenerate()
{
	BuildMesh();
}

void APerlinProcTerrain::SetTerrainParams(int32 InSeed, int32 InXVerts, int32 InYVerts, float InGridSpacing, float InHeightScale,
	int32 InOctaves, float InFrequency, float InLacunarity, float InPersistence, bool bInRidge, bool bInBillow)
{
	Seed = InSeed;
	XSize = InXVerts; // if user used XSize/YSize naming
	YSize = InYVerts;
	GridSpacing = InGridSpacing;
	HeightScale = InHeightScale;
	Octaves = FMath::Clamp(InOctaves, 1, 12);
	Frequency = FMath::Max(0.0001f, InFrequency);
	Lacunarity = FMath::Max(1.f, InLacunarity);
	Persistence = FMath::Clamp(InPersistence, 0.f, 1.f);
	bRidge = bInRidge;
	bBillow = bInBillow;
}


float APerlinProcTerrain::FractalNoise2D(float X, float Y) const
{
	float Amp = 1.f, Freq = Frequency, Sum = 0.f, Norm = 0.f;
	for (int32 Oct=0; Oct<Octaves; ++Oct)
	{
		float Sample = FMath::PerlinNoise3D(FVector(X*Freq, Y*Freq, Seed*0.1234f));
		float N = Sample;
		if (bRidge) { N = 1.f - FMath::Abs(Sample); }
		else if (bBillow) { N = FMath::Abs(Sample); }
		Sum += N * Amp; Norm += Amp; Amp *= Persistence; Freq *= Lacunarity;
	}
	return (Norm>KINDA_SMALL_NUMBER) ? (Sum/Norm) : 0.f;
}
