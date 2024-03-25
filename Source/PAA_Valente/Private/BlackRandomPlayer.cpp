// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackRandomPlayer.h"
#include "Piece.h"
#include "PiecePawn.h"
#include "EngineUtils.h"

// Sets default values
ABlackRandomPlayer::ABlackRandomPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	PlayerNumber = 1;
}

// Called when the game starts or when spawned
void ABlackRandomPlayer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABlackRandomPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABlackRandomPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABlackRandomPlayer::OnTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
	GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode->bIsBlackOnCheck)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Black is on Check!"));
	}

	FTimerHandle TimerHandle;

	GameMode->bIsBlackThinking = true;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			// Declarations

			AChessGameMode* GameModeCallback = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
			APiece* ChosenPiece = nullptr;
			TArray<ATile*> MovesAndEatablePieces;

			do
			{
				MovesAndEatablePieces.Empty();

				// Picking a random piece
				int32 RandIdx0 = FMath::Rand() % GameModeCallback->CB->BlackPieces.Num();
				ChosenPiece = GameModeCallback->CB->BlackPieces[RandIdx0];

				// Calculate piece's possible moves.
				ChosenPiece->PossibleMoves();
				ChosenPiece->FilterOnlyLegalMoves();

				MovesAndEatablePieces = ChosenPiece->Moves;
				MovesAndEatablePieces.Append(ChosenPiece->EatablePieces);

			} while (MovesAndEatablePieces.Num() == 0);

			// Getting previous tile
			ATile** PreviousTilePtr = GameModeCallback->CB->TileMap.Find(ChosenPiece->Relative2DPosition());

			// Getting the new tile and the new position
			int32 RandIdx1 = FMath::Rand() % MovesAndEatablePieces.Num();
			ATile* DestinationTile = MovesAndEatablePieces[RandIdx1];
			FVector TilePositioning = GameModeCallback->CB->GetRelativeLocationByXYPosition(DestinationTile->GetGridPosition().X, DestinationTile->GetGridPosition().Y);
			TilePositioning.Z = 10.0f;
			APiece* PieceToCapture = nullptr;

			// If it's an eating move, then delete the white piece
			if (DestinationTile->GetOccupantColor() == EOccupantColor::W)
			{
				// Search the white piece who occupies the tile and capture it
				for (APiece* WhitePiece : GameModeCallback->CB->WhitePieces)
				{
					if (WhitePiece->GetActorLocation() == TilePositioning)
					{
						GameModeCallback->CB->WhitePieces.Remove(WhitePiece);
						WhitePiece->PieceCaptured();
						break;
					}
				}
			}

			// Moving the piece
			ChosenPiece->SetActorLocation(TilePositioning);
			if (Cast<APiecePawn>(ChosenPiece))
			{
				Cast<APiecePawn>(ChosenPiece)->TurnsWithoutMoving = 0;
				Cast<APiecePawn>(ChosenPiece)->Promote();
				if (Cast<APiecePawn>(ChosenPiece)->bFirstMove == true)
				{
					Cast<APiecePawn>(ChosenPiece)->bFirstMove = false;
				}
			}
			else
			{
				for (APiece* BlackPawn : GameModeCallback->CB->BlackPieces)
				{
					if (Cast<APiecePawn>(BlackPawn))
					{
						Cast<APiecePawn>(BlackPawn)->TurnsWithoutMoving++;
					}
				}
			}

			// Setting the actual tile occupied by a black, setting the old one empty
			(*PreviousTilePtr)->SetOccupantColor(EOccupantColor::E);
			DestinationTile->SetOccupantColor(EOccupantColor::B);

			// Generate the FEN string and add it to the history of moves for replays
			FString LastMove = GameModeCallback->CB->GenerateStringFromPositions();
			GameModeCallback->CB->HistoryOfMoves.Add(LastMove);

			// Turn ending
			GameModeCallback->bIsBlackThinking = false;
			if (!Cast<APiecePawn>(ChosenPiece) || Cast<APiecePawn>(ChosenPiece)->Relative2DPosition().X != 0)
			{
				GameModeCallback->TurnPlayer();
			}


		}, 3, false);
}

void ABlackRandomPlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Wins!"));
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
	GameInstance->IncrementScoreAiPlayer();
}

void ABlackRandomPlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Loses!"));
	// GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}

bool ABlackRandomPlayer::IsCheckStatus()
{
	return false;
}
