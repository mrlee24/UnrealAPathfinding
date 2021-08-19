#include "Map/Ferragorn.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Map/Map.h" 
#include "Algo/Reverse.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AFerragorn::AFerragorn()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MapMesh"));
	MeshComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AFerragorn::BeginPlay()
{
	Super::BeginPlay();

	mMap = NewObject<UMap>(this, UMap::StaticClass());
	mMap->LoadMap(MeshComponent);


	// TODO: Figure out the path here!!
	SnowElfStartIndex = mMap->GetStart(EElfType::SnowElf);
	ForestElfStartIndex = mMap->GetStart(EElfType::ForestElf);
	FindPathByElfType(EElfType::SnowElf, SnowElfStartIndex, mMap->GetEnd());
	FindPathByElfType(EElfType::ForestElf, ForestElfStartIndex, mMap->GetEnd());

	GetWorldTimerManager().SetTimer(mDrawTimer, this, &AFerragorn::Update, 0.1f, true);
}

// Main loop of application
void AFerragorn::Update()
{
	if (SnowCounter == SnowPath.Num() - 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Snow Elf reached target location first at: %d"), SnowPath[SnowCounter].TileIndex);
		GetWorldTimerManager().ClearTimer(mDrawTimer);
		return;
	}
	
	if (ForestCounter == ForestPath.Num() - 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Snow Elf reached target location first at: %d"), ForestPath[ForestCounter].TileIndex);
		GetWorldTimerManager().ClearTimer(mDrawTimer);
		return;
	}

	if (SnowCounter < SnowPath.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Snow Elf Current Location: %d, Current Terrain: %d"), SnowPath[SnowCounter].TileIndex, (int8)SnowPath[SnowCounter].Terrain);
		SnowCounter++;
	}

	if (ForestCounter < ForestPath.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Forest Elf Current Location: %d, Current Terrain: %d"), ForestPath[ForestCounter].TileIndex, (int8)ForestPath[ForestCounter].Terrain);
		ForestCounter++;
	}
}

void AFerragorn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UKismetSystemLibrary::FlushPersistentDebugLines(GetWorld());
	
}

void AFerragorn::FindPathByElfType(EElfType Elf, int32 InStartLocation, int32 TargetLocation)
{
	TArray<FNeighbour> OpenSet;
	TArray<FNeighbour> ClosedSet;

	StartNode = { InStartLocation, mMap->GetTileTerrain(InStartLocation), 0 };
	StartNode.Parent = nullptr;
	TargetNode = { TargetLocation, mMap->GetTileTerrain(mMap->GetEnd()), mMap->GetManhattanHeuristicEstimate(Elf, InStartLocation, TargetLocation) };
	OpenSet.AddUnique(StartNode);

	while (OpenSet.Num() > 0)
	{
		FNeighbour CurrentNode = OpenSet[0];
		for (int32 Index = 1; Index < OpenSet.Num(); Index++)
		{
			if (OpenSet[Index].Distance < CurrentNode.Distance || OpenSet[Index].Distance == CurrentNode.Distance && OpenSet[Index].EstimateCost < CurrentNode.EstimateCost)
			{
				CurrentNode = OpenSet[Index];
			}
		}

		OpenSet.Remove(CurrentNode);
		ClosedSet.AddUnique(CurrentNode);

		(Elf == SnowElf) ? SnowPath.Add(CurrentNode) : ForestPath.Add(CurrentNode);

		// Add one more to indicate delay
		if (CurrentNode.Terrain == Forest && Elf == SnowElf)
		{
			SnowPath.Add(CurrentNode);
		}

		if (CurrentNode.Terrain == Snow && Elf == ForestElf)
		{
			ForestPath.Add(CurrentNode);
		}
		// ~

		if (CurrentNode == TargetNode)
		{
			UE_LOG(LogTemp, Warning, TEXT("Reached Location: %d"), (*CurrentNode.Parent).TileIndex);
			return;
		}

		for (FNeighbour& Neighbour : mMap->GetNeighbours(CurrentNode.TileIndex))
		{
			// Not Walkable
			if (Neighbour.Terrain == Mountain || Neighbour.Terrain == Water)
			{
				continue;
				/*if (ClosedSet.Contains(Neighbour))
				{
					continue;
				}*/
			}

			int32 NewMovementCostToNeighbour = CurrentNode.Distance + mMap->GetManhattanHeuristicEstimate(Elf, CurrentNode.TileIndex, Neighbour.TileIndex);
			if (NewMovementCostToNeighbour < Neighbour.Distance || !OpenSet.Contains(Neighbour))
			{
				Neighbour.Distance = NewMovementCostToNeighbour;
				Neighbour.EstimateCost = mMap->GetManhattanHeuristicEstimate(Elf, Neighbour.TileIndex, TargetNode.TileIndex);
				Neighbour.Parent = &CurrentNode;

				if (!OpenSet.Contains(Neighbour))
				{
					OpenSet.AddUnique(Neighbour);
				}
			}
		}
	}
}

void AFerragorn::RetracePath(FNeighbour InStartNode, FNeighbour InEndNode)
{
	/*FNeighbour* CurrentNode = &InEndNode;
	UE_LOG(LogTemp, Warning, TEXT("Trace Back"));
	while (CurrentNode)
	{
		if (CurrentNode == &InStartNode) return;
		Path.AddUnique(*CurrentNode);
		CurrentNode = CurrentNode->Parent;
	}

	Algo::Reverse(Path);*/
}
