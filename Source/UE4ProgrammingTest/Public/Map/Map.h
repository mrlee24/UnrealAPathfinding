#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Map.generated.h"

#define TILE_SIZE	16

// These terrain enumerations line up with the indicies
// of the array of layers used in the json map file.
UENUM()
enum ETerrain
{
	Water,
	Grass,
	Snow,
	Forest,
	Mountain,
	Building
};

// Snow elf = 0, forest elf = 1
UENUM()
enum EElfType
{
	SnowElf,
	ForestElf
};

USTRUCT()
struct FNeighbour
{
	GENERATED_BODY()

	int			TileIndex;
	ETerrain	Terrain;	
	int			Distance;	// either 16 (left, right, up or down) or diagonal (22ish)
	int         EstimateCost;
	bool bWalkable = (Terrain != ETerrain::Mountain && Terrain != ETerrain::Water);

	FNeighbour* Parent;

	FNeighbour() : TileIndex(-1), Terrain(Water), Distance(0) {}
	FNeighbour(int index, ETerrain terrain, int distance) : TileIndex(index), Terrain(terrain), Distance(distance) {}

	FORCEINLINE bool operator == (const FNeighbour& Other) const
	{
		return TileIndex == Other.TileIndex;
	}

	FORCEINLINE bool operator != (const FNeighbour& Other) const
	{
		return TileIndex != Other.TileIndex;
	}
};

UCLASS()
class UE4PROGRAMMINGTEST_API UMap : public UObject
{
	GENERATED_BODY()

public:
	UMap();
	void LoadMap(UStaticMeshComponent* MeshComponent);

	int32 GetStart(EElfType elf) const;
	int32 GetEnd() const;
	int32 GetTileIndex(int32 x, int32 y) const;
	const TArray<ETerrain>& GetTileData() const;
	TArray<FNeighbour> GetNeighbours(int32 index) const;
	int32 GetManhattanHeuristicEstimate(EElfType elf, int32 start, int32 end) const;
	ETerrain GetTileTerrain(int32 index) const;
	ETerrain GetTileTerrain(int32 x, int32 y) const;
	void GetTileCenterCoordinates(int32 tileIndex, float& x, float& y) const;

private:
	class UStaticMeshComponent* MeshComp;					// static mesh representation of map
	int32						mWidth;						// width of the map in tiles
	int32						mHeight;					// height of the map in files
	TArray<ETerrain>			mTileData;					// a 1-dimentional array holding the tile data, by row
	TArray<int>					mSettlementIndexLocations;	// indices of where the settlements are relative to the tile data, 1st is the snow elf, 2nd is the forest elf
	int32						mTowerIndexLocation;		// the destination

	friend class AFerragorn;
};
