// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceKing.h"
#include "EngineUtils.h"

// Sets default values
APieceKing::APieceKing()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// template function that creates a components
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);
}

// Called when the game starts or when spawned
void APieceKing::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APieceKing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APieceKing::PossibleMoves()
{
	Moves.Empty();
	EatablePieces.Empty();

	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector ActorLocation = RelativePosition();
	FVector2D TileLocation(ActorLocation.X, ActorLocation.Y);
	ATile** NextTile = GameMode->CB->TileMap.Find(TileLocation);
	
	// For every direction check if the tile is occupied, if not add a possible move
	for (const FVector2D& Direction : Directions)
	{
		FVector2D NextPosition = TileLocation + Direction;
		NextTile = GameMode->CB->TileMap.Find(NextPosition);
		if (NextTile == nullptr || ((*NextTile)->GetTileStatus() == ETileStatus::OCCUPIED && (*NextTile)->GetOccupantColor() == EOccupantColor::W))
		{
			continue;
		}
		else if (NextTile != nullptr && !IsSameColorAsTileOccupant((*NextTile)) && (*NextTile)->GetTileStatus() != ETileStatus::EMPTY)
		{
			EatablePieces.Add((*NextTile));
		}
		else
		{
			Moves.Add((*NextTile));
		}
	}
}
