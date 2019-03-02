// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralGen/ActorComponents/MapGenerators/MapGenerator.h"
#include "ProceduralMap.generated.h"

class UInstancedStaticMeshComponent;

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
	TArray<TArray<EMapTileEnum>> GenerateMap();

	// The meshes for our procedural generation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Meshes", meta = (AllowPrivateAccess = "true"))
		UInstancedStaticMeshComponent* FloorMeshInstances;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Meshes", meta = (AllowPrivateAccess = "true"))
		UInstancedStaticMeshComponent* WallMeshInstances;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Meshes", meta = (AllowPrivateAccess = "true"))
		UInstancedStaticMeshComponent* PillarMeshInstances;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Generator", meta = (AllowPrivateAccess = "true"))
		UMapGenerator* mapGen;

private :

	// The root component. It's only here for that, so a basic scene component is enough
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* Root;
};
