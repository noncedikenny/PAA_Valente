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

	PieceValue = 10;
}

// Called when the game starts or when spawned
void APieceKing::BeginPlay()
{
	Super::BeginPlay();
	
	if (Color == EColor::B)
	{
		PieceValue = -PieceValue;
	}
}

// Called every frame
void APieceKing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APieceKing::PossibleMoves()
{
	// Emptying from old moves (if there are any)
	Moves.Empty();
	EatablePiecesPosition.Empty();

	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector2D ActorLocation = Relative2DPosition();
	ATile* StartTile = GameMode->CB->TileMap[ActorLocation];
	ATile* NextTile = nullptr;

	// For every direction check if the tile is occupied, if not add a possible move
	for (const FVector2D& Direction : Directions)
	{
		FVector2D NextPosition = ActorLocation + Direction;
		if (GameMode->CB->TileMap.Contains(NextPosition))
		{
			NextTile = GameMode->CB->TileMap[NextPosition];

			if (StartTile->GetOccupantColor() != NextTile->GetOccupantColor())
			{
				if (NextTile->GetOccupantColor() == EOccupantColor::E)
				{
					Moves.Add(NextTile);
					continue;
				}
				EatablePiecesPosition.Add(NextTile);
				continue;
			}
		}
	}
}
