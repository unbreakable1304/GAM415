// Include the header file for this class
#include "CubeDMIMod.h"

// Include the player character class for type casting during overlap events
#include "Gam415_GreenCharacter.h"

// Include utility functions for generating random numbers and vectors
#include "Kismet/KismetMathLibrary.h"

// Constructor: Sets default values
ACubeDMIMod::ACubeDMIMod()
{
	// Enables the actor to tick every frame — useful for time-based behavior or real-time updates.
	// If performance is a concern and you don’t need per-frame logic, this can be disabled.
	PrimaryActorTick.bCanEverTick = true;

	// Create a box component that serves as the collision trigger for this actor.
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));

	// Create a static mesh component that visually represents the cube.
	cubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube Mesh"));

	// Set the root component to the box collider — this means the cube mesh will follow its transforms.
	RootComponent = boxComp;

	// Attach the cube mesh to the collision box so it inherits its position and rotation.
	cubeMesh->SetupAttachment(boxComp);
}

// Called when the game starts or when spawned
void ACubeDMIMod::BeginPlay()
{
	Super::BeginPlay();

	// Register the OnComponentBeginOverlap event handler to trigger when something enters the box collider.
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ACubeDMIMod::OnOverlapBegin);

	// If a base material is assigned, create a dynamic instance of it.
	// Dynamic materials allow real-time parameter changes, like color.
	if (baseMat)
	{
		dmiMat = UMaterialInstanceDynamic::Create(baseMat, this);
	}

	// If the cube mesh is valid, apply the dynamic material instance to it.
	if (cubeMesh)
	{
		cubeMesh->SetMaterial(0, dmiMat);
	}
}

// Called every frame
void ACubeDMIMod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Currently unused — this method exists for future updates requiring real-time logic.
}

// Event triggered when another actor overlaps with this actor’s box component.
void ACubeDMIMod::OnOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapping actor is of the player character class.
	AGAM415_GreenCharacter* overlappedActor = Cast<AGAM415_GreenCharacter>(OtherActor);
	if (overlappedActor)
	{
		// Generate random RGB values between 0 and 1.
		float ranNumX = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		float ranNumY = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		float ranNumZ = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);

		// Combine the RGB values into a linear color with full opacity (alpha = 1).
		FLinearColor randColor = FLinearColor(ranNumX, ranNumY, ranNumZ, 1.f);

		// If the dynamic material is valid, apply the new random color.
		if (dmiMat)
		{
			// Set the "Color" parameter in the material to the generated random color.
			dmiMat->SetVectorParameterValue("Color", randColor);

			// Optionally adjust a scalar material parameter (e.g., darkening effect).
			dmiMat->SetScalarParameterValue("Darkness", ranNumX);

			// If the Niagara particle system is assigned, spawn the effect at the overlap location.
			if (colorP)
			{
				// Spawn the particle system attached to the overlapping component.
				UNiagaraComponent* particleComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
					colorP,               // Niagara system to spawn
					OtherComp,            // Attach to the component that overlapped
					NAME_None,            // Attach to the default socket
					FVector(0.f),         // Location offset
					FRotator(0.f),        // Rotation
					EAttachLocation::KeepRelativeOffset, // Maintain relative transform
					true                  // Auto-destroy when complete
				);

				// Pass the same color to the particle system to match the material
				particleComp->SetNiagaraVariableLinearColor(FString("RandomColor"), randColor);
			}
		}
	}
}
