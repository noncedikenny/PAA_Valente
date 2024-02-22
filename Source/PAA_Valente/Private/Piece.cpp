// Fill out your copyright notice in the Description page of Project Settings.


#include "Piece.h"
#include "PieceKing.h"
#include "PiecePawn.h"
#include "EngineUtils.h"

// Sets default values
APiece::APiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Color = EColor::E;

}

// Called when the game starts or when spawned
void APiece::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APiece::ChangeMaterial(UMaterialInterface* NewMaterial)
{
	UStaticMeshComponent* MeshComponent = FindComponentByClass<UStaticMeshComponent>();
	if (MeshComponent)
	{
		MeshComponent->SetMaterial(0, NewMaterial);
	}
}

bool APiece::IsPathObstructed(FVector2D CurrentTileLocation, FVector2D TargetTileLocation, FVector MoveDirection)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector2D CheckTileLocation = CurrentTileLocation;
	ATile** CheckTilePtr = GameMode->CB->TileMap.Find(CheckTileLocation);

	int32 DeltaX = FMath::Sign(MoveDirection.X);
	int32 DeltaY = FMath::Sign(MoveDirection.Y);

	while (CheckTileLocation != TargetTileLocation)
	{
		CheckTileLocation.X += DeltaX;
		CheckTileLocation.Y += DeltaY;

		CheckTilePtr = GameMode->CB->TileMap.Find(CheckTileLocation);

		// Controlla se ci sono pezzi sul percorso
		if (CheckTilePtr != nullptr)
		{
			// Se la casella � occupata da un pezzo del proprio colore, il movimento non � valido
			if (((*CheckTilePtr)->GetOccupantColor() == EOccupantColor::W && Color == EColor::W) ||
				((*CheckTilePtr)->GetOccupantColor() == EOccupantColor::B && Color == EColor::B))
			{
				return true;
			}
		}
	}
	return false;
}

void APiece::PieceCaptured()
{
	Destroy();
}

FVector APiece::RelativePosition() const
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	// Getting the absolute location and getting the relative one passing through the position of the tile.
	FVector2D CurrentRelativeLocation2D = GameMode->CB->GetXYPositionByRelativeLocation(GetActorLocation());
	FVector CurrentRelativeLocation3D(CurrentRelativeLocation2D.X, CurrentRelativeLocation2D.Y, 10.f);
	return CurrentRelativeLocation3D;
}

void APiece::PossibleMoves(FVector& ActorLocation, TArray<ATile*>& ResultantArray)
{
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector2D TileLocation(ActorLocation.X, ActorLocation.Y);
	ATile** TilePtr = GameMode->CB->TileMap.Find(TileLocation);

	for (const FVector2D& Direction : Directions)
	{
		FVector2D NextPosition = TileLocation + Direction;
		TilePtr = GameMode->CB->TileMap.Find(NextPosition);
		bool bIsObstructed = false;

		while (bIsObstructed == false)
		{
			if (NextPosition.X >= 0 && NextPosition.X < 8 && NextPosition.Y >= 0 && NextPosition.Y < 8 &&
				(*TilePtr)->GetOccupantColor() == EOccupantColor::E)
			{
				ResultantArray.Add((*TilePtr));
				if (!Cast<APieceKing>(this))
				{
					NextPosition += Direction;
				}
				if (Cast<APiecePawn>(this) && Cast<APiecePawn>(this)->bFirstMove == true)
				{
					NextPosition += Direction;
					TilePtr = GameMode->CB->TileMap.Find(TileLocation);
					ResultantArray.Add((*TilePtr));
					break;
				}
			}
			else
			{
				bIsObstructed = true;
			}
		}
	}
}
