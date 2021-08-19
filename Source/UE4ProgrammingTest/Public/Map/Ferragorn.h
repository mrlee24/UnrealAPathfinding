#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "map/Map.h"
#include "Ferragorn.generated.h"

UCLASS()
class UE4PROGRAMMINGTEST_API AFerragorn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFerragorn();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* MeshComponent;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void Update();

	void OnConstruction(const FTransform& Transform) override;

private:
	class UMap* mMap;
	FTimerHandle mDrawTimer;

	// TODO: Put further additional variables/functions as needed here
	int32 SnowElfStartIndex;
	int32 ForestElfStartIndex;
	int32 SnowElfCurrentIndex;
	int32 ForestElfCurrentIndex;
	int32 TargetLocationIndex;

	TArray<FNeighbour> SnowPath;
	TArray<FNeighbour> ForestPath;
	FNeighbour StartNode;
	FNeighbour TargetNode;

	int32 SnowCounter = 0;
	int32 SnowDelay = 0;
	int32 ForestCounter = 0;
	int32 ForestDelay = 0;

	void FindPathByElfType(EElfType Elf, int32 InStartLocation, int32 TargetLocation);
	void RetracePath(FNeighbour InStartNode, FNeighbour InEndNode);
};