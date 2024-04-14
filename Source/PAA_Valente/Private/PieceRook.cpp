// Fill out your copyright notice in the Description page of Project Settings.


#include "PieceRook.h"
#include "EngineUtils.h"

// Sets default values
APieceRook::APieceRook()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// template function that creates a components
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);

	PieceValue = 5;
}

// Called when the game starts or when spawned
void APieceRook::BeginPlay()
{
	Super::BeginPlay();
	
	if (Color == EColor::B)
	{
		PieceValue = -PieceValue;
	}
}

// Called every frame
void APieceRook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APieceRook::PossibleMoves(FVector2D StartingPosition)
{
	// Emptying from old moves (if there are any)
	Moves.Empty();
	EatablePiecesPosition.Empty();

	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	ATile* StartTile = GameMode->CB->TileMap[StartingPosition];
	ATile* NextTile = nullptr;

	// For every direction check if the tile is occupied, if not add a possible move.
	// If a piece interrupts a path, then check the color.
	// If white -> break
	// If black -> add as eatable
	for (const FVector2D& Direction : Directions)
	{
		FVector2D NextPosition = StartingPosition + Direction;

		if (GameMode->CB->TileMap.Contains(NextPosition))
		{
			NextTile = GameMode->CB->TileMap[NextPosition];

			while (true)
			{
				if (!GameMode->CB->TileMap.Contains(NextPosition))
				{
					break;
				}

				NextTile = GameMode->CB->TileMap[NextPosition];

				if (StartTile->GetOccupantColor() != NextTile->GetOccupantColor())
				{
					if (NextTile->GetOccupantColor() == EOccupantColor::E)
					{
						Moves.Add(NextTile);
					}
					else
					{
						EatablePiecesPosition.Add(NextTile);
						break;
					}
				}
				else
				{
					break;
				}
				NextPosition += Direction;
			}
		}
	}
}
