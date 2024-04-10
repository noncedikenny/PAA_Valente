// Fill out your copyright notice in the Description page of Project Settings.


#include "PiecePawn.h"
#include "PieceQueen.h"
#include "PieceRook.h"
#include "PieceBishop.h"
#include "PieceKnight.h"
#include "ChessPlayerController.h"
#include "EngineUtils.h"

// Sets default values
APiecePawn::APiecePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// template function that creates a components
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);

	PieceValue = 1;
	TurnsWithoutMoving = 0;
	bIsFirstMove = true;
}

// Called when the game starts or when spawned
void APiecePawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (Color == EColor::B)
	{
		PieceValue = -PieceValue;
	}
}

void APiecePawn::Promote()
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->PawnPromotionWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), GameMode->PawnPromotionWidgetClass);

	// If it's white, show thw widget
	if (Color == EColor::W && RelativePosition().X == 7)
	{
		if (GameMode->PawnPromotionWidgetInstance)
		{
			GameMode->PawnPromotionWidgetInstance->AddToViewport();
			GameMode->PawnToPromote = this;
		}
	}

	// If it's black, chose randomly
	else if (Color == EColor::B && RelativePosition().X == 0)
	{
		GameMode->PawnToPromote = this;
		int32 RandIdx0 = FMath::Rand() % 4;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Randomly here"));
		switch (RandIdx0)
		{
			case 0:
				GameMode->PromoteToQueen();
				break;
			case 1:
				GameMode->PromoteToRook();
				break;
			case 2:
				GameMode->PromoteToBishop();
				break;
			case 3:
				GameMode->PromoteToKnight();
				break;
			default: 
				return;
		}
	}
}

// Called every frame
void APiecePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APiecePawn::PossibleMoves()
{
	Moves.Empty();
	EatablePiecesPosition.Empty();

	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector ActorLocation = RelativePosition();
	FVector2D TileLocation(ActorLocation.X, ActorLocation.Y);
	ATile** NextTile = GameMode->CB->TileMap.Find(TileLocation);

	// If the pawn is black, then invert his movements
	if (Color == EColor::B)
	{
		Directions = { FVector2D(-1, 0), FVector2D(-1, -1), FVector2D(-1, 1) };
	}

	for (const FVector2D& Direction : Directions)
	{
		FVector2D NextPosition = TileLocation + Direction;
		NextTile = GameMode->CB->TileMap.Find(NextPosition);

		if (NextTile != nullptr)
		{
			if (Direction == Directions[0] && (*NextTile)->GetOccupantColor() == EOccupantColor::E)
			{
				Moves.Add((*NextTile));

				NextPosition += Direction;
				NextTile = GameMode->CB->TileMap.Find(NextPosition);

				if (NextTile != nullptr && Direction == Directions[0] && bIsFirstMove && (*NextTile)->GetOccupantColor() == EOccupantColor::E)
				{
					Moves.Add(*NextTile);
				}

				continue;
			}
			else if (!IsSameColorAsTileOccupant(*NextTile) && (*NextTile)->GetOccupantColor() != EOccupantColor::E && Direction != Directions[0])
			{
				EatablePiecesPosition.Add(*NextTile);
				continue;
			}
		}
		else
		{
			continue;
		}
	}
}

void APiecePawn::IncrementTurnsWithoutMoving()
{
	TurnsWithoutMoving++;
}

void APiecePawn::ResetTurnsWithoutMoving()
{
	TurnsWithoutMoving = 0;
}

void APiecePawn::PawnMovedForTheFirstTime()
{
	bIsFirstMove = false;
}

int32 APiecePawn::GetTurnsWithoutMoving() const
{
	return TurnsWithoutMoving;
}

bool APiecePawn::GetIsFirstMove() const
{
	return bIsFirstMove;
}
