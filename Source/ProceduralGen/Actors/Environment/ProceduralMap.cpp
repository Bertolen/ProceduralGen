// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralMap.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AProceduralMap::AProceduralMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	// Init the meshes 
	FloorMeshInstances = CreateAbstractDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloorMeshInstances"));
	FloorMeshInstances->SetupAttachment(GetRootComponent());

	WallMeshInstances = CreateAbstractDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallMeshInstances"));
	WallMeshInstances->SetupAttachment(GetRootComponent());

	PillarMeshInstances = CreateAbstractDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PillarMeshInstances"));
	PillarMeshInstances->SetupAttachment(GetRootComponent());

	// Init some values
	MaxX = 5;
	MaxY = 5;
	TileSize = 400;
}

void AProceduralMap::GenerateMap()
{
	//cleanup the data first
	MapData.Empty();

	//map generation one tile at a time
	for (int i = 0; i < (2 * MaxX + 1) * (2 * MaxY + 1); i++)
	{
		int mapI = i / (2 * MaxY + 1);
		int mapJ = i % (2 * MaxY + 1);

		if (mapI % 2) {
			if (mapJ % 2) {
				// floor
				MapData.Push(EMapTileEnum::EFloor);
			}
			else {
				//vertical wall
				MapData.Push(EMapTileEnum::EVerticalWall);
			}
		}
		else {
			if (mapJ % 2) {
				//horizontal wall
				MapData.Push(EMapTileEnum::EHorizontalWall);
			}
			else {
				// pillar
				MapData.Push(EMapTileEnum::EPillar);
			}
		}
	}

	// Make random maze
	MakeRandomMaze();
}

void AProceduralMap::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	//Do some cleanup first
	FloorMeshInstances->ClearInstances();
	WallMeshInstances->ClearInstances();
	PillarMeshInstances->ClearInstances();

	// Randomly generate our map
	GenerateMap();

	// Now we start the real construction
	for (int i = 0; i < 2 * MaxX + 1; i++)
	{
		int tileX = i / 2;
		for (int j = 0; j < 2 * MaxY + 1; j++)
		{
			int tileY = j / 2;
			switch (MapData[i * (2 * MaxY + 1) + j])
			{
			case EMapTileEnum::EFloor :
				FloorMeshInstances->AddInstance(FTransform(FVector(tileX*TileSize, tileY*TileSize, 0)));
				break;

			case EMapTileEnum::EHorizontalWall:
				WallMeshInstances->AddInstance(FTransform(FQuat(FVector::UpVector, UKismetMathLibrary::GetPI() / 2), FVector(tileX*TileSize, tileY*TileSize, 0)));
				break;

			case EMapTileEnum::EVerticalWall:
				WallMeshInstances->AddInstance(FTransform(FVector(tileX*TileSize, tileY*TileSize, 0)));
				break;

			case EMapTileEnum::EPillar:
				PillarMeshInstances->AddInstance(FTransform(FVector(tileX*TileSize, tileY*TileSize, 0)));
				break;

			default:
				break;
			}
		}
	}
}

void AProceduralMap::MakeRandomMaze()
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("Begin maze generation"));
	TArray<TArray<int>> tileValues;

	// init the tile values
	for (int i = 0; i < MaxX; i++)
	{
		tileValues.Push(TArray<int>());

		for (int j = 0; j < MaxY; j++)
		{
			tileValues[i].Push(i * MaxY + j);
		}
	}

	do
	{
		// declare a point in our maze
		FVector2D pointA(UKismetMathLibrary::RandomIntegerInRange(0, MaxX - 1),	UKismetMathLibrary::RandomIntegerInRange(0, MaxY - 1));
		int aValue = tileValues[pointA.X][pointA.Y];

		// find all the adjacing points with different values
		TArray<ETileWallEnum> neighbours = GetDifferentNeighbours(tileValues, pointA);

		// if there is at least one
		if (neighbours.Num())
		{
			// pick one at random
			ETileWallEnum wall = neighbours[UKismetMathLibrary::RandomIntegerInRange(0, neighbours.Num() - 1)];

			// TODO : remove the wall in between
			int bValue;
			int mapDataX = 2 * pointA.X + 1;
			int mapDataY = 2 * pointA.Y + 1;
			int mapDataColsAmt = 2 * MaxY + 1;
			switch (wall)
			{
			case ETileWallEnum::ENorth:
				bValue = tileValues[pointA.X - 1][pointA.Y];
				MapData[(mapDataX - 1) * mapDataColsAmt + mapDataY] = EMapTileEnum::EEmpty;
				break;

			case ETileWallEnum::ESouth:
				bValue = tileValues[pointA.X + 1][pointA.Y];
				MapData[(mapDataX + 1) * mapDataColsAmt + mapDataY] = EMapTileEnum::EEmpty;
				break;

			case ETileWallEnum::EEast:
				bValue = tileValues[pointA.X][pointA.Y + 1];
				MapData[mapDataX * mapDataColsAmt + mapDataY + 1] = EMapTileEnum::EEmpty;
				break;

			case ETileWallEnum::EWest:
				bValue = tileValues[pointA.X][pointA.Y - 1];
				MapData[mapDataX * mapDataColsAmt + mapDataY - 1] = EMapTileEnum::EEmpty;
				break;

			default:
				bValue = MaxX * MaxY + 1;
				break;

			}

			// set all the tiles with the same values to the min value between the 2
			tileValues = ReplaceValues(tileValues, FMath::Max(bValue, aValue), FMath::Min(bValue, aValue));

		}
	} while (!AllValuesNull(tileValues));

	UE_LOG(LogTemp, VeryVerbose, TEXT("End of maze generation"));
}


// Returns true if all the values are null. False if not.
bool AProceduralMap::AllValuesNull(TArray<TArray<int>> values)
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("AllValuesNull"));
	for (int i = 0; i < values.Num(); i++)
	{
		for (int j = 0; j < values[i].Num(); j++)
		{
			if (values[i][j] != 0) 
			{
				UE_LOG(LogTemp, VeryVerbose, TEXT("found a non null value : %d at %d - %d."), values[i][j], i, j);
				return false;
			}
		}
	}

	UE_LOG(LogTemp, VeryVerbose, TEXT("All values are null"));
	return true;
}


// Returns the relative placement of the neigbhours with different values for a given point
TArray<ETileWallEnum> AProceduralMap::GetDifferentNeighbours(TArray<TArray<int>> values, FVector2D point)
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("Begin GetDifferentNeighbours"));
	TArray<ETileWallEnum> points;

	// check the tile to the north
	if (point.X - 1 >= 0 && values[point.X][point.Y] != values[point.X - 1][point.Y])
	{
		points.Push(ETileWallEnum::ENorth);
	}

	// check the tile to the south
	if (point.X + 1 <= values.Num() - 1 && values[point.X][point.Y] != values[point.X + 1][point.Y])
	{
		points.Push(ETileWallEnum::ESouth);
	}

	// check the tile to the east
	if (point.Y + 1 <= values[0].Num() - 1 && values[point.X][point.Y] != values[point.X][point.Y + 1])
	{
		points.Push(ETileWallEnum::EEast);
	}

	// check the tile to the west
	if (point.Y - 1 >= 0 && values[point.X][point.Y] != values[point.X][point.Y - 1])
	{
		points.Push(ETileWallEnum::EWest);
	}

	UE_LOG(LogTemp, VeryVerbose, TEXT("End GetDifferentNeighbours : found %d neighours"), points.Num());
	return points;
}

// TODO
TArray<TArray<int>> AProceduralMap::ReplaceValues(TArray<TArray<int>> values, int oldValue, int newValue)
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("ReplaceValues : %d to %d"), oldValue, newValue);
	for (int i = 0; i < values.Num(); i++)
	{
		for (int j = 0; j < values[i].Num(); j++)
		{
			if (values[i][j] == oldValue)
			{
				values[i][j] = newValue;
			}
		}
	}
	return values;
}