// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProcMeshFromStatic.generated.h"

/**
 * AProcMeshFromStatic
 *
 * Actor that converts a static mesh into a procedural mesh at runtime.
 * Useful for applying runtime modifications or physics to previously static geometry.
 */
UCLASS()
class GAM415_GREEN_API AProcMeshFromStatic : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProcMeshFromStatic();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the actor is placed in the level or created in the editor
	virtual void PostActorCreated() override;

	// Called when the actor is loaded from disk (e.g., during level load)
	virtual void PostLoad() override;

public:
	// Called every frame (disabled in constructor, but left here for flexibility)
	virtual void Tick(float DeltaTime) override;

	// Stores the vertex positions extracted from the static mesh
	UPROPERTY()
	TArray<FVector> Vertices;

	// Stores the triangle indices (order of vertices forming mesh faces)
	UPROPERTY()
	TArray<int> Triangles;

	// Stores normal vectors for each vertex
	UPROPERTY()
	TArray<FVector> Normals;

	// Stores UV mapping coordinates (used for texturing)
	TArray<FVector2D> UV0;

	// Stores linear color data for each vertex (optional for advanced effects)
	UPROPERTY()
	TArray<FLinearColor> VertexColors;

	// Stores basic vertex colors (used by procedural mesh for color data)
	TArray<FColor> UpVertexColors;

	// Stores tangents used for normal mapping and advanced material effects
	TArray<FProcMeshTangent> Tangents;

	// Static mesh component used as the source for mesh conversion
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* baseMesh;

private:
	// Procedural mesh component that will render the copied mesh at runtime
	UProceduralMeshComponent* procMesh;

	// Extracts vertex and index data from the static mesh
	void GetMeshData();

	// Builds the mesh section on the procedural mesh using extracted data
	void CreateMesh();
};
