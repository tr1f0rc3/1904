// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVLogUserWidget.h"
#include "TimerManager.h"
#include "WidgetLayoutLibrary.h"
#include "Engine/Texture2D.h"
#include "Components/CanvasPanelSlot.h"

#include "SizeBox.h"
#include "TextBlock.h"
#include "Image.h"

#include "KRV.h"
#include "WidgetBlueprintLibrary.h"

void UKRVLogUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MainSizeBox = Cast<USizeBox>(GetWidgetFromName(TEXT("SB_Main_1")));
	LogTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Log_1")));
	LogIconImage = Cast<UImage>(GetWidgetFromName(TEXT("IM_Log_1")));
	SetVisibility(ESlateVisibility::Collapsed);

}

void UKRVLogUserWidget::ShowThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);

}

void UKRVLogUserWidget::HideThisWidget_Implementation()
{
	OnThisWidgetHiddenDelegate.ExecuteIfBound(this);

}

void UKRVLogUserWidget::SetSizeBoxAuto()
{
	if (MainSizeBox)
	{
		UWidgetLayoutLibrary::SlotAsCanvasSlot(MainSizeBox)->SetAutoSize(true);
	}
}

void UKRVLogUserWidget::SetLogText(const FText& NewText, UTexture2D* NewTexture /*= nullptr*/)
{
	KRVCHECK(LogTextBlock);
	if (LogTextBlock)
	{
		LogTextBlock->SetText(NewText);
		SetSizeBoxAuto();
	}

}

bool UKRVLogUserWidget::SetLogIconImage(UTexture2D* NewTexture /*= nullptr*/)
{
	if (!LogIconImage)
	{
		KRVCHECK(LogIconImage);
	}
	else if (!NewTexture)
	{
		LogIconImage->SetVisibility(ESlateVisibility::Hidden);
		return true;
	}
	else
	{
		LogIconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		UWidgetBlueprintLibrary::SetBrushResourceToTexture(LogIconImage->Brush, NewTexture);
		return true;
	}
	return false;

}

bool UKRVLogUserWidget::PopupLog(const FText& NewText, UTexture2D* NewTexture /*= nullptr*/)
{
	if (NewText.IsEmptyOrWhitespace() || !LogTextBlock)
	{
		KRVCHECK(!NewText.IsEmptyOrWhitespace());
		KRVCHECK(LogTextBlock);
	}
	else
	{
		SetLogText(NewText);
		SetLogIconImage(NewTexture);
		ShowThisWidget();
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UKRVLogUserWidget::HideThisWidget, HideDelay, false);
			return true;
		}
	}
	return false;

}
