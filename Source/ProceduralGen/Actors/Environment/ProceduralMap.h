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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (ClampMin = "3"))
		int MaxX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (ClampMin = "3"))
		int MaxY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map", meta = (ClampMin = "0"))
		int TileSize;

protected:

	virtual void OnConstruction(const FTransform &Transform) override;

	// Randomly generate our map
	virtual void GenerateMap();

	// 2D array to store the map data
	TArray<TArray<EMapTileEnum>> MapData;

	// The meshes for our procedural generation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Meshes", meta = (AllowPrivateAccess = "true"))
		UInstancedStaticMeshComponent* FloorMeshInstances;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Meshes", meta = (AllowPrivateAccess = "true"))
		UInstancedStaticMeshComponent* WallMeshInstances;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Meshes", meta = (AllowPrivateAccess = "true"))
		UInstancedStaticMeshComponent* PillarMeshInstances;

private :

	void MakeRandomMaze();

	// The root component. It's only here for that, so a basic scene component is enough
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* Root;

	// Returns true if all the values are null. False if not.
	bool AllValuesNull(TArray<TArray<int>> values);

	// Returns the relative placement of the neigbhours with different values for a given point
	TArray<ETileWallEnum> GetDifferentNeighbours(TArray<TArray<int>> values, FVector2D point);

	TArray<TArray<int>> ReplaceValues(TArray<TArray<int>> values, int oldValue, int newValue);
};
