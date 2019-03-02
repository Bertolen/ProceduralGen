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

	//init the component
	mapGen = CreateDefaultSubobject<UMapGenerator>(TEXT("MapGenerator"));

	// Init some values
	MaxX = 5;
	MaxY = 5;
	TileSize = 400;
}

TArray<TArray<EMapTileEnum>> AProceduralMap::GenerateMap()
{
	if (mapGen) {
		return mapGen->GenerateMap(MaxX, MaxY);
	}
	else {
		return *(new TArray<TArray<EMapTileEnum>>);
	}
}

void AProceduralMap::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	//Do some cleanup first
	FloorMeshInstances->ClearInstances();
	WallMeshInstances->ClearInstances();
	PillarMeshInstances->ClearInstances();

	// Randomly generate our map
	TArray<TArray<EMapTileEnum>> MapData = GenerateMap();

	//If the map is empty we stop here
	if (MapData.Num() == 0) {
		return;
	}

	// Now we start the real construction
	for (int i = 0; i < 2 * MaxX + 1; i++)
	{
		int tileX = i / 2;
		for (int j = 0; j < 2 * MaxY + 1; j++)
		{
			int tileY = j / 2;
			switch (MapData[i][j])
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