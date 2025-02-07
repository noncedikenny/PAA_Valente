// Fill out your copyright notice in the Description page of Project Settings.


#include "WhitePlayer.h"
#include "Piece.h"
#include "PiecePawn.h"
#include "ChessGameMode.h"
#include "ChessPlayerController.h"
#include "MainHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Sets default values
AWhitePlayer::AWhitePlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// create a camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//set the camera as RootComponent
	SetRootComponent(Camera);

	GameInstance = Cast<UChessGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	
	bIsACapture = false;
	IsMyTurn = false;
}

// Called when the game starts or when spawned
void AWhitePlayer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWhitePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWhitePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AWhitePlayer::PieceSelection()
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FString LastMoveDone = GameMode->CB->HistoryOfMoves.Last();

	// Detecting player's click
	FHitResult Hit = FHitResult(ForceInit);
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

	if (Hit.bBlockingHit && IsMyTurn && (GameMode->CB->GenerateStringFromPositions() == LastMoveDone) && !GameMode->GetOnMenu())
	{
		if (APiece* PieceClicked = Cast<APiece>(Hit.GetActor()))
		{
			// Ally Piece
			if (PieceClicked->GetColor() == EColor::W)
			{
				// Save the piece
				SelectedPieceToMove = PieceClicked;

				// Deleting possible old colorations
				PieceClicked->DecolorPossibleMoves();
				PieceClicked->ColorPossibleMoves();
			}
			// Enemy Piece
			else if (PieceClicked->GetColor() == EColor::B && SelectedPieceToMove != nullptr)
			{
				if (GameMode->CB->TileMap.Contains(PieceClicked->GetVirtualPosition()))
				{
					ATile* DestinationTile = GameMode->CB->TileMap[PieceClicked->GetVirtualPosition()];

					// Capture enemy's piece and call tile method
					if (SelectedPieceToMove->Moves.Contains(DestinationTile))
					{
						GameMode->CB->BlackPieces.Remove(PieceClicked);
						PieceClicked->Destroy();
						bIsACapture = true;
						DestinationTile->SetOccupantColor(EOccupantColor::E);
						TileSelection(DestinationTile);
					}
				}
			}
		}
		else if (ATile* TileClicked = Cast<ATile>(Hit.GetActor()))
		{
			if (SelectedPieceToMove != nullptr)
			{
				TileSelection(TileClicked);
			}
		}
	}

	else if (Hit.bBlockingHit && IsMyTurn && (GameMode->CB->GenerateStringFromPositions() != LastMoveDone) && !GameMode->GetOnMenu())
	{
		UMainHUD* MainHUD = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController())->MainHUDWidget;
		MainHUD->ButtonArray.Last()->ButtonOnClickFunction();
	}
}

void AWhitePlayer::TileSelection(ATile* CurrTile)
{
	// Declarations
	AChessGameMode* GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
	FVector2D OldPosition = SelectedPieceToMove->GetVirtualPosition();
	ATile* PreviousTile = GameMode->CB->TileMap[OldPosition];
	UMainHUD* MainHUD = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController())->MainHUDWidget;

	if (CurrTile->GetOccupantColor() == EOccupantColor::E && !GameMode->GetOnMenu())
	{
		// If a tile is clicked, decolor possible moves
		SelectedPieceToMove->DecolorPossibleMoves();

		// If the move is legal, move the piece
		if (SelectedPieceToMove->Moves.Contains(CurrTile))
		{
			FVector ActorPositioning = GameMode->CB->GetRelativeLocationByXYPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);
			ActorPositioning.Z = 10.0f;
			SelectedPieceToMove->SetActorLocation(ActorPositioning);
			SelectedPieceToMove->SetVirtualPosition(CurrTile->GetGridPosition());

			// Pawn tie / promote check procedure
			if (SelectedPieceToMove->IsA<APiecePawn>())
			{
				if (Cast<APiecePawn>(SelectedPieceToMove)->GetIsFirstMove())
				{
					Cast<APiecePawn>(SelectedPieceToMove)->PawnMovedForTheFirstTime();
				}
				GameMode->SetPawnMoved(true);
				Cast<APiecePawn>(SelectedPieceToMove)->Promote();
			}
		}

		// Setting the actual tile occupied by a white, setting the old one empty
		if (SelectedPieceToMove->GetVirtualPosition() == CurrTile->GetGridPosition())
		{
			CurrTile->SetOccupantColor(EOccupantColor::W);
			PreviousTile->SetOccupantColor(EOccupantColor::E);

			// Generate the FEN string and add it to the history of moves for replays
			FString LastMove = GameMode->CB->GenerateStringFromPositions();
			GameMode->CB->HistoryOfMoves.Add(LastMove);
			
			// Create dinamically the move button
			if (MainHUD)
			{
				MainHUD->AddButton(LastMove, SelectedPieceToMove, bIsACapture, SelectedPieceToMove->GetVirtualPosition(), OldPosition);
			}

			bIsACapture = false;

			// Turn ending
			IsMyTurn = false;
			if (!SelectedPieceToMove->IsA<APiecePawn>() || Cast<APiecePawn>(SelectedPieceToMove)->GetVirtualPosition().X != 7)
			{
				SelectedPieceToMove = nullptr;
				GameMode->TurnPlayer();
			}
			// Else -> A pawn has been promoted, then the turn's passed in GameMode promotion's segment
		}
	}
}

void AWhitePlayer::Deselect()
{
	SelectedPieceToMove = nullptr;
}

void AWhitePlayer::OnTurn()
{
	IsMyTurn = true;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Your Turn"));
	GameInstance->SetTurnMessage(TEXT("Human Turn"));
}

void AWhitePlayer::OnWin()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Win!"));
	GameInstance->SetTurnMessage(TEXT("Human Wins!"));
	GameInstance->IncrementScoreHumanPlayer();
}

bool AWhitePlayer::GetThinkingStatus() const
{
	return false;
}

void AWhitePlayer::DestroyPlayer()
{
	this->Destroy();
}
