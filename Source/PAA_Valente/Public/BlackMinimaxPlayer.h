// Fill out your copyright notice in the Description page of Project Settings. a

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ChessGameInstance.h"
#include "ChessGameMode.h"
#include "Piece.h"
#include "PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "BlackMinimaxPlayer.generated.h"

UCLASS()
class PAA_VALENTE_API ABlackMinimaxPlayer : public APawn, public IPlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABlackMinimaxPlayer();
	UChessGameInstance* GameInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool bIsACapture;
	bool bThinking;
	APiece* BestPiece;
	int32 MinimaxDepth;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Interface's methods
	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual bool GetThinkingStatus() const override;
	virtual void DestroyPlayer() override;

	// Used in blueprint
	UFUNCTION(BlueprintCallable)
	void SetDepth(int32 Depth);

	// Minimax basic functions
	int32 Maxi(int32 Depth, int32 Alpha, int32 Beta);
	int32 Mini(int32 Depth, int32 Alpha, int32 Beta);
	int32 Evaluate();
	ATile* FindBestMove();
};
