#include "Map/Map.h"
#include "Components/StaticMeshComponent.h"

UMap::UMap()
{
	
}

// Load up the map from the json file
void UMap::LoadMap(UStaticMeshComponent* MeshComponent)
{
	MeshComp = MeshComponent;

	// Ignores Unreal's version of the file
	FString JsonFilePath = FPaths::ProjectContentDir() + "Data/map.json";
	FString JsonString;

	int BuildingTileCount = 0;

	FFileHelper::LoadFileToString(JsonString, *JsonFilePath);
	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		// Width and height
		mWidth = JsonObject->GetIntegerField("width");
		mHeight = JsonObject->GetIntegerField("height");

		// Layer objects
		TArray<TSharedPtr<FJsonValue>> LayerArray = JsonObject->GetArrayField("layers");
		for (int32 i = 0; i < LayerArray.Num(); ++i)
		{
			TSharedPtr<FJsonObject> LayerObject = LayerArray[i]->AsObject();
			TArray<TSharedPtr<FJsonValue>> LayerData = LayerObject->GetArrayField("data");
			ETerrain LayerType = (ETerrain)i;
			// Layer data array
			for (int32 j = 0; j < LayerData.Num(); ++j)
			{
				// Fill with water
				if (LayerType == ETerrain::Water)
				{
					mTileData.Add(ETerrain::Water);
				}
				// Set tile if it is non-zero
				else if (LayerData[j]->AsNumber() > 0)
				{
					mTileData[j] = LayerType;
					// Start and end points
					if (LayerType == Building)
					{
						BuildingTileCount += 1;
						switch(BuildingTileCount)
						{
							case 1:
							case 2:
								mSettlementIndexLocations.Add(j);
								break;
							case 5:
								mTowerIndexLocation = j;
								break;
						}
					}
				}
			}
		}
	}
}

// Return the tile index of an elf's starting point
int32 UMap::GetStart(EElfType elf) const
{
	return mSettlementIndexLocations[elf];
}

// Return a vector of indicies up to 8 elements which are the N, E, S, W,
// NE, SE, SW and NW neighbours of the given tile. Tiles on on the edge
//  of the map will only return 5 neighbours, tiles on the corner only return 3.
TArray<FNeighbour> UMap::GetNeighbours(int32 tileIndex) const
{
    TArray<FNeighbour> Neighbours;

	int32	x			= tileIndex % mWidth;
	int32	y			= (tileIndex - x) / mWidth;
	bool	bHasNorth	= (y > 0);			// there's a north
	bool	bHasEast	= (x < mWidth - 1);	// there's an east
	bool	bHasSouth	= (y < mHeight - 1);// there's a south
	bool	bHasWest	= (x > 0);			// there's a west
	int32	TileDiag	= (int32)sqrt((double)(TILE_SIZE * TILE_SIZE) + (TILE_SIZE * TILE_SIZE));
    
	if (bHasNorth)
	{
		int32 index = GetTileIndex(x, y - 1);
		Neighbours.Emplace(FNeighbour(index, mTileData[index], TILE_SIZE));
	}

	if (bHasEast)
	{
		int32 index = GetTileIndex(x + 1, y);
		Neighbours.Emplace(FNeighbour(index, mTileData[index], TILE_SIZE));
	}

	if (bHasSouth)
	{
		int32 index = GetTileIndex(x, y + 1);
		Neighbours.Emplace(FNeighbour(index, mTileData[index], TILE_SIZE));
	}

	if (bHasWest)
	{
		int32 index = GetTileIndex(x - 1, y);
		Neighbours.Emplace(FNeighbour(index, mTileData[index], TILE_SIZE));
	}

	if (bHasNorth && bHasEast)
	{
		int32 index = GetTileIndex(x + 1, y - 1);
		Neighbours.Emplace(FNeighbour(index, mTileData[index], TileDiag));
	}

	if (bHasSouth && bHasEast)
	{
		int32 index = GetTileIndex(x + 1, y + 1);
		Neighbours.Emplace(FNeighbour(index, mTileData[index], TileDiag));
	}

	if (bHasSouth && bHasWest)
	{
		int32 index = GetTileIndex(x - 1, y + 1);
		Neighbours.Emplace(FNeighbour(index, mTileData[index], TileDiag));
	}

	if (bHasNorth && bHasWest)
	{
		int32 index = GetTileIndex(x - 1, y - 1);
		Neighbours.Emplace(FNeighbour(index, mTileData[index], TileDiag));
	}

	return Neighbours;
}

// Return a 1-dimensional tile index based on 2d coordinate system
int32 UMap::GetTileIndex(int32 x, int32 y) const
{
	check(x >= 0);
	check(x < mWidth);
	check(y >= 0);
	check(y < mHeight);

	return x + (y * mWidth);
}

// Return the terrain  based on 2d coordinates
ETerrain UMap::GetTileTerrain(int32 x, int32 y) const
{
	check(x >= 0);
	check(x < mWidth);
	check(y >= 0);
	check(y < mHeight);

	return mTileData[x + (y * mWidth)];
}

// Return the terrain of a given index
ETerrain UMap::GetTileTerrain(int32 index) const
{
	check(index > 0);
	check(index < mWidth * mHeight);
	return mTileData[index];
}

// Returns the x and y world coordinates of the center of the tile at the index
void UMap::GetTileCenterCoordinates(int32 tileIndex, float& x, float& y) const
{
	// Get socket name of tile
	FString NameString = "Center.";
	if (tileIndex < 100)
	{
		NameString += "0";
	}
	if (tileIndex < 10)
	{
		NameString += "0";
	}
	NameString += FString::FromInt(tileIndex);
	FName SocketName = FName(*NameString);
	FVector Coords = MeshComp->GetSocketLocation(SocketName);
	
	x = Coords.X;
	y = Coords.Y;
}

// Return the tile data
const TArray<ETerrain>& UMap::GetTileData() const
{
	return mTileData;
}

// return the goal tile index
int32 UMap::GetEnd() const
{
	return mTowerIndexLocation;
}

// http://en.wikipedia.org/wiki/Taxicab_geometry - to estimate between two points
int32 UMap::GetManhattanHeuristicEstimate(EElfType elf, int32 start, int32 end) const
{
	if(start == end) return 0;
	
	int startX	= start % mWidth;
	int startY	= (start - startX) / mWidth;
	int endX	= end % mWidth;
	int endY	= (end - endX) / mWidth;

	return ((abs(endX - startX) + abs(endY - startY))) * TILE_SIZE;	
}