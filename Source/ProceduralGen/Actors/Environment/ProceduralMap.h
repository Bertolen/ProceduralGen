// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMap.generated.h"

class UInstancedStaticMeshComponent;

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

UCLASS()
class PROCEDURALGEN_API AProceduralMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralMap();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
		int MaxX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
		int MaxY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
		int TileSize;

protected:

	virtual void OnConstruction(const FTransform &Transform) override;

	// Randomly generate our map
	virtual void GenerateMap();

	// 2D array to store the map data
	TArray<EMapTileEnum> MapData;

	// The meshes for our procedural generation
	UInstancedStaticMeshComponent* FloorMeshInstances;
	UInstancedStaticMeshComponent* WallMeshInstances;
	UInstancedStaticMeshComponent* PillarMeshInstances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map|Meshes")
		UStaticMesh* FloorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map|Meshes")
		FVector FloorOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map|Meshes")
		UStaticMesh* WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map|Meshes")
		FVector WallOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map|Meshes")
		UStaticMesh* PillarMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map|Meshes")
		FVector PillarOffset;

private :

	void MakeRandomMaze();

	// Returns true if all the values are null. False if not.
	bool AllValuesNull(TArray<TArray<int>> values);

	// Returns the relative placement of the neigbhours with different values for a given point
	TArray<ETileWallEnum> GetDifferentNeighbours(TArray<TArray<int>> values, FVector2D point);

	TArray<TArray<int>> ReplaceValues(TArray<TArray<int>> values, int oldValue, int newValue);
};
