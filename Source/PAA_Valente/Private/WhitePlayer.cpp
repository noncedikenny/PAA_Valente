// Fill out your copyright notice in the Description page of Project Settings.


#include "WhitePlayer.h"
#include "Piece.h"
#include "PiecePawn.h"
#include "ChessGameMode.h"
#include "ChessPlayerController.h"
#include "PieceKing.h"
#include "PieceRook.h"
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

	// Find enemy team attributes
	TArray<APiece*>* EnemyPieces = (AllyColor == EColor::W) ? &GameMode->CB->BlackPieces : &GameMode->CB->WhitePieces;
	EColor EnemyColor = (AllyColor == EColor::W) ? EColor::B : EColor::W;
	EOccupantColor EnemyOccupantColor = (AllyColor == EColor::W) ? EOccupantColor::B : EOccupantColor::W;

	// Detecting player's click
	FHitResult Hit = FHitResult(ForceInit);
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

	if (Hit.bBlockingHit && IsMyTurn && (GameMode->CB->GenerateStringFromPositions() == LastMoveDone))
	{
		if (APiece* PieceClicked = Cast<APiece>(Hit.GetActor()))
		{
			// Ally Piece
			if (PieceClicked->GetColor() == AllyColor)
			{
				if (SelectedPieceToMove && SelectedPieceToMove->IsA<APieceKing>() && PieceClicked->IsA<APieceRook>())
				{
					
					if (SelectedPieceToMove->Moves.Contains(GameMode->CB->TileMap[PieceClicked->GetVirtualPosition()]))
					{
						// Deleting possible old colorations
						PieceClicked->DecolorPossibleMoves();
						IsMyTurn = false;
						Cast<APieceKing>(SelectedPieceToMove)->PerformCastling(PieceClicked);
						SelectedPieceToMove = nullptr;
					}
				}
				if (IsMyTurn)
				{

					// Deleting possible old colorations
					PieceClicked->DecolorPossibleMoves();

					// Save the piece
					SelectedPieceToMove = PieceClicked;
					PieceClicked->ColorPossibleMoves();
				}
			}
			// Enemy Piece
			else if (PieceClicked->GetColor() == EnemyColor && SelectedPieceToMove != nullptr)
			{
				if (GameMode->CB->TileMap.Contains(PieceClicked->GetVirtualPosition()))
				{
					ATile* DestinationTile = GameMode->CB->TileMap[PieceClicked->GetVirtualPosition()];

					// Capture enemy's piece and call tile method
					if (SelectedPieceToMove->Moves.Contains(DestinationTile))
					{
						EnemyPieces->Remove(PieceClicked);
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

	else if (Hit.bBlockingHit && IsMyTurn && (GameMode->CB->GenerateStringFromPositions() != LastMoveDone))
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
	ATile* PreviousTile = *GameMode->CB->TileMap.Find(OldPosition);
	if (PreviousTile == nullptr)
	{
		return;
	}
	UMainHUD* MainHUD = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController())->MainHUDWidget;

	// Find enemy team attributes
	TArray<APiece*>* EnemyPieces = (AllyColor == EColor::W) ? &GameMode->CB->BlackPieces : &GameMode->CB->WhitePieces;
	EColor EnemyColor = (AllyColor == EColor::W) ? EColor::B : EColor::W;
	EOccupantColor EnemyOccupantColor = (AllyColor == EColor::W) ? EOccupantColor::B : EOccupantColor::W;

	if (CurrTile->GetOccupantColor() == EOccupantColor::E)
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

			if (!SelectedPieceToMove->GetWasMoved())
			{
				// To comunicate the first move
				SelectedPieceToMove->SetWasMoved(true);
			}

			// Pawn tie / promote check procedure
			if (SelectedPieceToMove->IsA<APiecePawn>())
			{	
				// To comunicate a general move for 50 moves rule
				GameMode->APawnHasMoved();
				Cast<APiecePawn>(SelectedPieceToMove)->Promote();
			}
		}

		// Setting the actual tile occupied by a white, setting the old one empty
		if (SelectedPieceToMove->GetVirtualPosition() == CurrTile->GetGridPosition())
		{
			CurrTile->SetOccupantColor(AllyOccupantColor);
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
			if (!Cast<APiecePawn>(SelectedPieceToMove) || Cast<APiecePawn>(SelectedPieceToMove)->GetVirtualPosition().X != 7)
			{
				SelectedPieceToMove = nullptr;
				GameMode->TurnPlayer();
			}
			// Else -> A pawn has been promoted, then the turn's passed in GameMode promotion's segment
		}
	}
}

APiece* AWhitePlayer::GetSelectedPieceToMove() const
{
	return SelectedPieceToMove;
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

void AWhitePlayer::SetTeam(EColor TeamColor)
{
	AChessGameMode* GameMode = (AChessGameMode*)(GetWorld()->GetAuthGameMode());

	AllyColor = TeamColor;
	AllyOccupantColor = (TeamColor == EColor::W) ? EOccupantColor::W : EOccupantColor::B;
	AllyPieces = (TeamColor == EColor::W) ? &GameMode->CB->WhitePieces : &GameMode->CB->BlackPieces;
}
