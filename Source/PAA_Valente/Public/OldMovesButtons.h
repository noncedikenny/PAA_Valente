// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ChessGameMode.h"
#include "WhitePlayer.h"
#include "OldMovesButtons.generated.h"

/**
 * 
 */
UCLASS()
class PAA_VALENTE_API UOldMovesButtons : public UButton
{
	GENERATED_BODY()

	UTextBlock* MoveDone;

	FString AssociatedString;

public:

	AChessGameMode* GameMode;
	UChessGameInstance* GameInstance;

	UOldMovesButtons(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void ButtonOnClickFunction();

	void SetAssociatedString(FString& AssociatedStringToPrint);

	// Text creation functions
	void CreateText(APiece* PieceMoved, bool bItWasACapture, FVector2D NewPosition, FVector2D OldPosition);
	TCHAR PieceParsing(APiece* PieceToParse);
	FString LocationParsing(FVector2D& Location);
};
