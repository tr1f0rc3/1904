// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVConfirmUserWidget.h"

#include "KRV.h"
#include "TextBlock.h"
#include "KRVIconUserWidget.h"

void UKRVConfirmUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ConfirmIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ConfirmIcon_1")));
	if (ConfirmIconWidget)
	{
		ConfirmIconWidget->SetCustomMenuIcon(EIconTextureVariation::Confirm);
		ConfirmIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVConfirmUserWidget::OnConfirmIconClicked);
	}

	CancleIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_CancleIcon_1")));
	if (CancleIconWidget)
	{
		CancleIconWidget->SetCustomMenuIcon(EIconTextureVariation::Cancle);
		CancleIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVConfirmUserWidget::OnCancleIconClicked);
	}

	ConfirmTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Confirm_1")));

}

bool UKRVConfirmUserWidget::SetConfirmText(const FText& NewText)
{
	if (!ConfirmTextBlock)
	{
		KRVCHECK(ConfirmTextBlock);
	}
	else
	{
		ConfirmTextBlock->SetText(NewText);
		return true;
	}
	return false;

}

void UKRVConfirmUserWidget::OnConfirmIconClicked()
{
	OnConfirmClickedDelegate.ExecuteIfBound();

}

void UKRVConfirmUserWidget::OnCancleIconClicked()
{
	SetVisibility(ESlateVisibility::Hidden);
	RemoveFromParent();

}
