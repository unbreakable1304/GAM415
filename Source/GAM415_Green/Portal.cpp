// Fill out your copyright notice in the Description page of Project Settings.

#include "Portal.h"
#include "Kismet/GameplayStatics.h"
#include "GAM415_GreenCharacter.h"

// Sets default values
APortal::APortal()
{
    // Enable Tick() to be called every frame (can be disabled for performance if not needed)
    PrimaryActorTick.bCanEverTick = true;

    // Create and initialize the mesh component
    mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");

    // Create and initialize the box collision component
    boxComp = CreateDefaultSubobject<UBoxComponent>("Box Comp");

    // Create and initialize the scene capture component
    sceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture");

    // Set the root component of the actor to be the box collider
    RootComponent = boxComp;

    // Attach mesh to the box collider
    mesh->SetupAttachment(boxComp);

    // Attach the scene capture to the mesh
    sceneCapture->SetupAttachment(mesh);

    // Set the mesh to ignore all collision channels
    mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
    Super::BeginPlay();

    // Bind overlap event to OnOverlapBegin function
    boxComp->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);

    // Hide the mesh from being rendered in the scene capture
    mesh->SetHiddenInSceneCapture(true);

    // If a material is assigned, apply it to the mesh
    if (mat)
    {
        mesh->SetMaterial(0, mat);
    }
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update the portal's camera position and rotation each frame
    UpdatePortals();
}

// Called when another actor begins to overlap the portal's collision box
void APortal::OnOverlapBegin(UPrimitiveComponent* OlverLappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Attempt to cast the overlapping actor to the player character
    AGAM415_GreenCharacter* playerChar = Cast<AGAM415_GreenCharacter>(OtherActor);

    // Proceed if the overlapping actor is the player
    if (playerChar)
    {
        // Check if the destination portal exists
        if (OtherPortal)
        {
            // Prevent teleport spamming by checking teleport flag
            if (!playerChar->isTeleporting)
            {
                // Set teleport flag to true
                playerChar->isTeleporting = true;

                // Move the player to the location of the linked portal
                FVector loc = OtherPortal->GetActorLocation();
                playerChar->SetActorLocation(loc);

                // Schedule a timer to reset the teleport flag after 1 second
                FTimerHandle TimerHandle;
                FTimerDelegate TimerDelegate;
                TimerDelegate.BindUFunction(this, "SetBool", playerChar);
                GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1, false);
            }
        }
    }
}

// Resets the player's teleporting flag after a delay
void APortal::SetBool(AGAM415_GreenCharacter* playerChar)
{
    if (playerChar)
    {
        playerChar->isTeleporting = false;
    }
}

// Updates the portal's scene capture camera to simulate looking through the portal
void APortal::UpdatePortals()
{
    if (!OtherPortal) return;  // Safety check to prevent crashes

    FVector Location = this->GetActorLocation() - OtherPortal->GetActorLocation();

    APlayerCameraManager* camManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
    if (!camManager) return;  // Safety check for camera manager

    USceneComponent* camTransform = camManager->GetTransformComponent();
    if (!camTransform) return;  // Ensure the transform component exists

    FVector camLocation = camTransform->GetComponentLocation();
    FRotator camRotation = camTransform->GetComponentRotation();

    FVector CombinedLocation = camLocation + Location; // kept
	// NOTE: consider mirroring across portal plane for accurate view

    if (sceneCapture)
    {
        sceneCapture->SetWorldLocationAndRotation(CombinedLocation, camRotation);
    }
}




void APortal::UpdateRenderTargetResolution()
{
	if (!GEngine || !GEngine->GameViewport) return;
	FVector2D Size(1280,720);
	GEngine->GameViewport->GetViewportSize(Size);
	const int32 LongSide = FMath::Clamp((int32)FMath::Max(Size.X, Size.Y), 256, MaxCaptureResolution);
	const int32 ShortSide = FMath::Clamp((int32)(LongSide * 9.f / 16.f), 256, MaxCaptureResolution);
	if (renderTarget) { renderTarget->ResizeTarget(LongSide, ShortSide); }
}
