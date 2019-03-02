// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapGenerator.generated.h"

UENUM(BlueprintType)
enum class EMapTileEnum : uint8
{
	EEmpty,
	EFloor,
	EVerticalWall,
	EHorizontalWall,
	EPillar
};

UENUM(BlueprintType)
enum class ETileWallEnum : uint8
{
	ENorth,
	ESouth,
	EEast,
	EWest
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCEDURALGEN_API UMapGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMapGenerator();

protected:

public:	
	// Randomly generate our map
	virtual TArray<TArray<EMapTileEnum>> GenerateMap(int MaxX, int MaxY);

		
public:
	// 2D array to store the map data
	TArray<TArray<EMapTileEnum>> MapData;

private :
	void MakeRandomMaze(int MaxX, int MaxY);

	// Returns the relative placement of the neigbhours with different values for a given point
	TArray<ETileWallEnum> GetDifferentNeighbours(TArray<TArray<int>> values, FVector2D point);

	// Replaces all the old values by the new values in the 2d array given
	TArray<TArray<int>> ReplaceValues(TArray<TArray<int>> values, int oldValue, int newValue);

	// Returns true if all the values are null. False if not.
	bool AllValuesNull(TArray<TArray<int>> values);
};
