// Fill out your copyright notice in the Description page of Project Settings.


#include "Chessboard.h"
#include "Kismet/GameplayStatics.h"
#include "Piece.h"
#include "PieceBishop.h"
#include "PieceKing.h"
#include "PieceKnight.h"
#include "PiecePawn.h"
#include "PieceQueen.h"
#include "PieceRook.h"
#include "WhitePlayer.h"
#include "ChessPlayerController.h"
#include "ChessGameInstance.h"
#include "MainHUD.h"
#include "EngineUtils.h"

AChessboard::AChessboard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Size = 8;
	TileSize = 120;
	Kings.SetNum(2);
}

void AChessboard::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AChessboard::BeginPlay()
{
	Super::BeginPlay();
	GenerateField();

	// Using FEN notation to generate every piece
	FString GeneratingString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

	GeneratePositionsFromString(GeneratingString);
	// Using FEN notation for the replay mechanic too
	HistoryOfMoves.Add(GeneratingString);

	SetTilesOwners();
}

void AChessboard::ResetField()
{
	// Declaration
	AChessPlayerController* CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
	
	HistoryOfMoves.Empty();
	if (CPC->MainHUDWidget)
	{
		CPC->MainHUDWidget->DestroyButtons();
	}

	FString GeneratingString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	GeneratePositionsFromString(GeneratingString);
	HistoryOfMoves.Add(GeneratingString);

	SetTilesOwners();
}

void AChessboard::GenerateField()
{
	UMaterialInterface* LoadWhiteMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_White"));
	UMaterialInterface* LoadBlackMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_Black"));

	for (int32 x = 0; x < Size; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			FVector Location = AChessboard::GetRelativeLocationByXYPosition(x, y);
			ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass, Location, FRotator::ZeroRotator);

			// Setting the color of the tile (b/w)
			if ((x + y) % 2 == 0)
			{
				Obj->ChangeMaterial(LoadBlackMaterial);
			}
			else
			{
				Obj->ChangeMaterial(LoadWhiteMaterial);
			}

			const float TileScale = TileSize / 100;
			Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
			Obj->SetGridPosition(x, y);
			TileArray.Add(Obj);
			TileMap.Add(FVector2D(x, y), Obj);
		}
	}
}

FString AChessboard::GenerateStringFromPositions()
{
	// This function generates a FEN string depending on pieces' position 
	FString ResultantString = TEXT("");

	// FEN's logic counts from the last row to the first
	for (int32 Row = 7; Row >= 0; --Row)
	{
		// Counter to increment if a tile's empty
		int32 EmptyCount = 0;
		for (int32 Col = 0; Col < 8; ++Col)
		{
			ATile** CurrentTile = TileMap.Find(FVector2D(Row, Col));

			// White piece case
			if ((*CurrentTile)->GetOccupantColor() == EOccupantColor::W)
			{
				for (int32 i = 0; i < WhitePieces.Num(); ++i)
				{
					FVector Location(Row, Col, 10.f);
					APiece* PieceFound = WhitePieces[i];

					if (PieceFound->IsA<APiecePawn>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('P');
						break;
					}
					else if (PieceFound->IsA<APieceKing>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);

							EmptyCount = 0;
						}
						ResultantString.AppendChar('K');
						break;
					}
					else if (PieceFound->IsA<APieceKnight>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('N');
						break;
					}
					else if (PieceFound->IsA<APieceQueen>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('Q');
						break;
					}
					else if (PieceFound->IsA<APieceRook>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('R');
						break;
					}
					else if (PieceFound->IsA<APieceBishop>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('B');
						break;
					}
				}
			}

			// Black piece case
			else if ((*CurrentTile)->GetOccupantColor() == EOccupantColor::B)
			{
				for (int32 i = 0; i < BlackPieces.Num(); ++i)
				{
					FVector Location(Row, Col, 10.f);
					APiece* PieceFound = BlackPieces[i];

					if (PieceFound->IsA<APiecePawn>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('p');
						break;
					}
					else if (PieceFound->IsA<APieceKing>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('k');
						break;
					}
					else if (PieceFound->IsA<APieceKnight>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('n');
						break;
					}
					else if (PieceFound->IsA<APieceQueen>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('q');
						break;
					}
					else if (PieceFound->IsA<APieceRook>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('r');
						break;
					}
					else if (PieceFound->IsA<APieceBishop>() && PieceFound->GetVirtualPosition() == FVector2D(Location.X, Location.Y))
					{
						if (EmptyCount > 0)
						{
							ResultantString.AppendInt(EmptyCount);
							EmptyCount = 0;
						}
						ResultantString.AppendChar('b');
						break;
					}
				}
			}

			// Empty tile case
			else
			{
				++EmptyCount;
			}
		}
		// Append the number of empty tile before passing to the next row
		if (EmptyCount > 0)
		{
			ResultantString.AppendInt(EmptyCount);
		}
		// Add a slash at the end of the row
		if (Row > 0)
		{
			ResultantString.AppendChar('/');
		}
	}

	return ResultantString;
}

void AChessboard::GeneratePositionsFromString(FString& String)
{
	// Emptying old pieces (if there are any) to recreate the chosen move
	for (APiece* WhitePiece : WhitePieces)
	{
		WhitePiece->Destroy();
	}
	for (APiece* BlackPiece : BlackPieces)
	{
		BlackPiece->Destroy();
	}
	for (ATile* Tile : TileArray)
	{
		Tile->SetOccupantColor(EOccupantColor::E);
	}
	WhitePieces.Empty();
	BlackPieces.Empty();

	// Skin declarations
	UMaterialInterface* LoadBlackBishop = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BBishop"));
	UMaterialInterface* LoadBlackPawn = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BPawn"));
	UMaterialInterface* LoadBlackKing = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BKing"));
	UMaterialInterface* LoadBlackQueen = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BQueen"));
	UMaterialInterface* LoadBlackKnight = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BKnight"));
	UMaterialInterface* LoadBlackRook = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BRook"));

	// Blueprint declarations
	UBlueprint* PawnsBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Pawn"));
	UBlueprint* BishopBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Bishop"));
	UBlueprint* KingBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_King"));
	UBlueprint* QueenBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Queen"));
	UBlueprint* KnightBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Knight"));
	UBlueprint* RookBlueprint = LoadObject<UBlueprint>(nullptr, TEXT("/Game/Blueprints/BP_Rook"));

	// Rows and Columns indexes
	int32 Row = 7;
	int32 Col = 0;

	// Loop through each character in the FEN string
	for (int32 i = 0; i < String.Len(); ++i)
	{
		TCHAR Char = String[i];

		// Slash case
		if (Char == '/')
		{
			--Row;
			Col = 0;
		}
		// Digit case
		else if (FChar::IsDigit(Char))
		{
			if (Char - '0' > 8)
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid string."));
				break;
			}
			Col += Char - '0';
		}
		// Char case
		else
		{
			APiece* Obj = nullptr;
			FVector Location = AChessboard::GetRelativeLocationByXYPosition(Row, Col);
			Location.Z = 10.f;

			// Determine the piece type based on the character
			switch (Char)
			{
			// WHITE PIECES
			case 'P':
				Obj = GetWorld()->SpawnActor<APiecePawn>(PawnsBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::W);
				Obj->SetVirtualPosition(FVector2D(Row, Col));
				if (Row != 1)
				{
					Cast<APiecePawn>(Obj)->PawnMovedForTheFirstTime();
				}

				TileMap[FVector2D(Row, Col)]->SetOccupantColor(EOccupantColor::W);

				WhitePieces.Add(Obj);
				break;

			case 'N':
				Obj = GetWorld()->SpawnActor<APieceKnight>(KnightBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::W);
				Obj->SetVirtualPosition(FVector2D(Row, Col));

				TileMap[FVector2D(Row, Col)]->SetOccupantColor(EOccupantColor::W);

				WhitePieces.Add(Obj); 
				break;

			case 'B':
				Obj = GetWorld()->SpawnActor<APieceBishop>(BishopBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::W);
				Obj->SetVirtualPosition(FVector2D(Row, Col));
				WhitePieces.Add(Obj); 
				break;

			case 'R':
				Obj = GetWorld()->SpawnActor<APieceRook>(RookBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::W);
				Obj->SetVirtualPosition(FVector2D(Row, Col));

				TileMap[FVector2D(Row, Col)]->SetOccupantColor(EOccupantColor::W);

				WhitePieces.Add(Obj);
				break;

			case 'Q':
				Obj = GetWorld()->SpawnActor<APieceQueen>(QueenBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::W);
				Obj->SetVirtualPosition(FVector2D(Row, Col));

				TileMap[FVector2D(Row, Col)]->SetOccupantColor(EOccupantColor::W);

				WhitePieces.Add(Obj);
				break;

			case 'K':
				Obj = GetWorld()->SpawnActor<APieceKing>(KingBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::W);
				Obj->SetVirtualPosition(FVector2D(Row, Col));
				Kings[0] = (Obj);

				TileMap[FVector2D(Row, Col)]->SetOccupantColor(EOccupantColor::W);

				WhitePieces.Add(Obj);
				break;

			
			// BLACK PIECES
			case 'p':
				Obj = GetWorld()->SpawnActor<APiecePawn>(PawnsBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::B);
				Obj->SetVirtualPosition(FVector2D(Row, Col));
				if (Row != 6)
				{
					Cast<APiecePawn>(Obj)->PawnMovedForTheFirstTime();
				}
				Obj->ChangeMaterial(LoadBlackPawn);

				TileMap[FVector2D(Row, Col)]->SetOccupantColor(EOccupantColor::B);

				BlackPieces.Add(Obj);
				break;

			case 'n':
				Obj = GetWorld()->SpawnActor<APieceKnight>(KnightBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::B);
				Obj->SetVirtualPosition(FVector2D(Row, Col));
				Obj->ChangeMaterial(LoadBlackKnight);

				TileMap[FVector2D(Row, Col)]->SetOccupantColor(EOccupantColor::B);

				BlackPieces.Add(Obj);
				break;

			case 'b':
				Obj = GetWorld()->SpawnActor<APieceBishop>(BishopBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::B);
				Obj->SetVirtualPosition(FVector2D(Row, Col));
				Obj->ChangeMaterial(LoadBlackBishop);

				TileMap[FVector2D(Row, Col)]->SetOccupantColor(EOccupantColor::B);

				BlackPieces.Add(Obj);
				break;

			case 'r':
				Obj = GetWorld()->SpawnActor<APieceRook>(RookBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::B);
				Obj->SetVirtualPosition(FVector2D(Row, Col));
				Obj->ChangeMaterial(LoadBlackRook); 

				TileMap[FVector2D(Row, Col)]->SetOccupantColor(EOccupantColor::B);

				BlackPieces.Add(Obj);
				break;

			case 'q':
				Obj = GetWorld()->SpawnActor<APieceQueen>(QueenBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::B);
				Obj->SetVirtualPosition(FVector2D(Row, Col));
				Obj->ChangeMaterial(LoadBlackQueen);

				TileMap[FVector2D(Row, Col)]->SetOccupantColor(EOccupantColor::B);

				BlackPieces.Add(Obj); 
				break;

			case 'k':
				Obj = GetWorld()->SpawnActor<APieceKing>(KingBlueprint->GeneratedClass, Location, FRotator::ZeroRotator);
				Obj->SetColor(EColor::B);
				Obj->SetVirtualPosition(FVector2D(Row, Col));
				Obj->ChangeMaterial(LoadBlackKing);
				Kings[1] = (Obj);

				TileMap[FVector2D(Row, Col)]->SetOccupantColor(EOccupantColor::B);

				BlackPieces.Add(Obj); 
				break;

			default:
				UE_LOG(LogTemp, Error, TEXT("Invalid string."));
				break;
			}

			// Move to the next column
			++Col; 
		}
	}
}

void AChessboard::SetTilesOwners()
{
	TArray<APiece*> AllPieces = WhitePieces;
	AllPieces.Append(BlackPieces);

	for (ATile* Tile : TileArray)
	{
		for (APiece* Piece : AllPieces)
		{
			if (Piece->GetActorLocation() == FVector(Tile->GetActorLocation().X, Tile->GetActorLocation().Y, 10.f))
			{
				if (Piece->GetColor() == EColor::W)
				{
					Tile->SetOccupantColor(EOccupantColor::W);
				}
				else if (Piece->GetColor() == EColor::B)
				{
					Tile->SetOccupantColor(EOccupantColor::B);
				}
				break;
			}
		}
	}
}

FVector AChessboard::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
	return TileSize * FVector(InX, InY, 0);
}

FVector2D AChessboard::GetXYPositionByRelativeLocation(const FVector& Location) const
{
	const double x = Location[0] / TileSize;
	const double y = Location[1] / TileSize;
	return FVector2D(x, y);
}
