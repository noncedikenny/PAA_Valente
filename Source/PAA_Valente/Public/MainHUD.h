// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "OldMovesButtons.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class PAA_VALENTE_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddButton();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ResetButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ResetText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* ScrollBox;

	TArray<UOldMovesButtons*> ButtonArray;

	int32 IntToKeepTrack = 0;

	virtual void NativeConstruct() override;
};
