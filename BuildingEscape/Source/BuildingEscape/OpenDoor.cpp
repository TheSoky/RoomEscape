// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

#define OUT


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();	

	if (Owner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Component unable to find owner"))
	}

	if (PreassurePlate == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is missing a Preassure Plate reference"), *Owner->GetName())
	}
}

void UOpenDoor::OpenDoor()
{
	if (!Owner)
	{
		return;
	}
	Owner->SetActorRotation(FRotator(0.0f, OpenAngle, 0.0f));
}

void UOpenDoor::CloseDoor()
{
	if (!Owner)
	{
		return;
	}
	Owner->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll the Trigger Volume
	if (GetTotalMassOfActorsOnPlate() > 29.0f) //TODO make it into parameter
	{
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}

	//Check if it is time to close the door
	if (GetWorld()->GetTimeSeconds() >= (LastDoorOpenTime + DoorCloseDelay))
	{
		CloseDoor();
	}

}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	if (!PreassurePlate)
	{
		return 0.0f;
	}

	float TotalMass = 0.0f;

	//Fing all the overlapping actors
	TArray<AActor*> OverlappingActors;
	PreassurePlate->GetOverlappingActors(OUT OverlappingActors);

	//Iterate through them adding their masses
	for (const auto* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s is on preassure plate"), *Actor->GetName())
	}

	return TotalMass;
}

