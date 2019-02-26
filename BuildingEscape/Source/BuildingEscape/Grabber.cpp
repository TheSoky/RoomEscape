// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandlerComponent();
	
	SetupInputComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);


	FVector LineTraceEnd = PlayerViewPointLocation + (Reach * PlayerViewPointRotation.Vector());

	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}

}

///Look for attached Physics Handle
void UGrabber::FindPhysicsHandlerComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("Object %s is missing Physics Handle component"), *(GetOwner()->GetName()))
	}
}

///Look for attached Input Controller
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (!InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Object %s is missing Input Component"), *(GetOwner()->GetName()))
	}
	else
	{
		///Bind the input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);

	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	//if the physics handle is attached
	//Move the object we're holding

	/// Get the player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);


	FVector LineTraceEnd = PlayerViewPointLocation + (Reach * PlayerViewPointRotation.Vector());

	///Setup query paramaters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	///Line-trace (Ray-cast) out to reach distance
	FHitResult LineTraceHit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT LineTraceHit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	AActor* ActorHit = LineTraceHit.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Object in aim: %s"), *(ActorHit->GetName()))

	}

	///See what we hit

	return LineTraceHit;
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Pressed"))

	/// LINE TRACE and see if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	///If we hit something than attach physics handle
	if (ActorHit)
	{
		//TODO attach physics handle
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true	//Allow rotation
		);
	}
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Released"))
	PhysicsHandle->ReleaseComponent();

	//TODO release physics handle
}

