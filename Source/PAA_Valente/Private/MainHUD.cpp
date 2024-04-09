// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"

void UMainHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainHUD::AddButton()
{
    UOldMovesButtons* NewButton = NewObject<UOldMovesButtons>(this);
    if (NewButton)
    {
        NewButton->OnClicked.AddDynamic(NewButton, &UOldMovesButtons::ButtonOnClickFunction);
        NewButton->GameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
        NewButton->CPC = Cast<AChessPlayerController>(GetWorld()->GetFirstPlayerController());
        ButtonArray.Add(NewButton);

        ScrollBox->AddChild(NewButton);
    }
}

void UMainHUD::DestroyButtons()
{
    if (ScrollBox)
    {
        ScrollBox->ClearChildren();
    }
}
