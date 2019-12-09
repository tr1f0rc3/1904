// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVLevelInfoUserWidget.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Engine/Texture2D.h"
#include "WidgetBlueprintLibrary.h"
#include "Image.h"
#include "TextBlock.h"
#include "SizeBox.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "DataAssets/KRVLevelDataAsset.h"
#include "Controllers/KRVMapPlayerController.h"
#include "KRVIconUserWidget.h"

void UKRVLevelInfoUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OverviewImage = Cast<UImage>(GetWidgetFromName(TEXT("IM_OVERVIEW_1")));
	NameTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Name_1")));
	DescriptionTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Description_1")));
	StarSizeBox_1 = Cast<USizeBox>(GetWidgetFromName(TEXT("SB_Star_1")));
	StarSizeBox_2 = Cast<USizeBox>(GetWidgetFromName(TEXT("SB_Star_2")));
	StarSizeBox_3 = Cast<USizeBox>(GetWidgetFromName(TEXT("SB_Star_3")));
	ClearTimeTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_ClearTime_1")));

	ConfirmIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ConfirmIcon_1")));
	if (ConfirmIconWidget)
	{
		ConfirmIconWidget->SetCustomMenuIcon(EIconTextureVariation::Confirm);
		ConfirmIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVLevelInfoUserWidget::OnConfirmIconClicked);
	}

	CancleIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_CancleIcon_1")));
	if (CancleIconWidget)
	{
		CancleIconWidget->SetCustomMenuIcon(EIconTextureVariation::Cancle);
		CancleIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVLevelInfoUserWidget::OnCancleIconClicked);
	}

}

void UKRVLevelInfoUserWidget::ShowThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);

}

void UKRVLevelInfoUserWidget::HideThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);

}

void UKRVLevelInfoUserWidget::OnConfirmIconClicked()
{
	if (GetWorld())
	{
		AKRVMapPlayerController* KRVMapPlayerController = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(GetWorld());
		if (!KRVMapPlayerController)
		{
			KRVCHECK(KRVMapPlayerController);
		}
		else
		{
			KRVMapPlayerController->OpenInGameLevelByDA(BoundLevelDA, BoundLevelSaveData);
		}
	}

}

void UKRVLevelInfoUserWidget::OnCancleIconClicked()
{
	HideThisWidget();

}

bool UKRVLevelInfoUserWidget::SetLevelImage(class UTexture2D* TextureToSet)
{
	if (!OverviewImage)
	{
		KRVCHECK(OverviewImage);
	}
	else if (!TextureToSet)
	{
		OverviewImage->SetVisibility(ESlateVisibility::Hidden);
		return true;
	}
	else
	{
		OverviewImage->SetVisibility(ESlateVisibility::Visible);
		UWidgetBlueprintLibrary::SetBrushResourceToTexture(OverviewImage->Brush, TextureToSet);
		return true;
	}
	return false;

}

bool UKRVLevelInfoUserWidget::SetLevelName(const FText& NameToSet)
{
	if (!NameTextBlock)
	{
		KRVCHECK(NameTextBlock);
	}
	else if (NameToSet.IsEmptyOrWhitespace())
	{
		NameTextBlock->SetVisibility(ESlateVisibility::Hidden);
		return true;
	}
	else
	{
		NameTextBlock->SetText(NameToSet);
		NameTextBlock->SetVisibility(ESlateVisibility::Visible);
		return true;
	}
	return false;

}

bool UKRVLevelInfoUserWidget::SetLevelDescription(const FText& DescriptionToSet)
{
	if (!DescriptionTextBlock)
	{
		KRVCHECK(DescriptionTextBlock);
	}
	else if (DescriptionToSet.IsEmptyOrWhitespace())
	{
		DescriptionTextBlock->SetVisibility(ESlateVisibility::Hidden);
		return true;
	}
	else
	{
		DescriptionTextBlock->SetText(DescriptionToSet);
		DescriptionTextBlock->SetVisibility(ESlateVisibility::Visible);
		return true;
	}
	return false;

}

bool UKRVLevelInfoUserWidget::ShowStars(int32 StarsToShow)
{
	if (StarsToShow < 0)
	{
		KRVCHECK(StarsToShow >= 0);
	}
	else if (!StarSizeBox_1 || !StarSizeBox_2 || !StarSizeBox_3)
	{
		KRVCHECK(StarSizeBox_1 && StarSizeBox_2 && StarSizeBox_3);
	}
	else
	{
		StarSizeBox_1->SetVisibility(ESlateVisibility::Collapsed);
		StarSizeBox_2->SetVisibility(ESlateVisibility::Collapsed);
		StarSizeBox_3->SetVisibility(ESlateVisibility::Collapsed);

		if (StarsToShow > 0)
		{
			StarSizeBox_1->SetVisibility(ESlateVisibility::Visible);
		}

		if (StarsToShow > 1)
		{
			StarSizeBox_2->SetVisibility(ESlateVisibility::Visible);
		}

		if (StarsToShow > 2)
		{
			StarSizeBox_3->SetVisibility(ESlateVisibility::Visible);
		}

		return true;
	}
	return false;

}

bool UKRVLevelInfoUserWidget::SetBestClearTime(float ClearTimeToSet)
{
	if (!ClearTimeTextBlock)
	{
		KRVCHECK(ClearTimeTextBlock);
	}
	else
	{
		FString ClearTimeStringToSet;
		if (ClearTimeToSet > 36000.f) // more than 10h
		{
			ClearTimeStringToSet = FString(TEXT(" - "));
		}
		else
		{
			UKRVBlueprintFunctionLibrary::GetStringFromSeconds(ClearTimeStringToSet, ClearTimeToSet);
		}
		ClearTimeTextBlock->SetText(FText::FromString(ClearTimeStringToSet));
		return true;
	}
	return false;

}

bool UKRVLevelInfoUserWidget::BindToLevel(UKRVLevelDataAsset* NewLevelDA, const FKRVLevelSaveData& NewLevelSaveDA)
{
	bIsBound = false;
	if (!NewLevelDA)
	{
		KRVCHECK(NewLevelDA);
	}
	else
	{
		bIsBound = true;
		BoundLevelDA = NewLevelDA;
		BoundLevelSaveData = NewLevelSaveDA;

		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		{ // set overview texture
			UTexture2D* OverviewTextureToSet = Streamable.LoadSynchronous
			(TSoftObjectPtr<UTexture2D>(NewLevelDA->OverviewTexture), true);
			bIsBound &= SetLevelImage(OverviewTextureToSet);
		}

		bIsBound &= SetLevelName(NewLevelDA->ClassFText);
		bIsBound &= SetLevelDescription(NewLevelDA->Description);
		bIsBound &= ShowStars(NewLevelSaveDA.EarnedStars);
		bIsBound &= SetBestClearTime(NewLevelSaveDA.BestClearTime);

		return bIsBound;
	}
	return false;
}
