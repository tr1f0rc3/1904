// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVEnums.h"
#include "Structs/KRVLevelSaveData.h"
#include "KRVLevelInfoUserWidget.generated.h"

UCLASS()
class KRV_API UKRVLevelInfoUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintNativeEvent)
		void ShowThisWidget();
	void ShowThisWidget_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void HideThisWidget();
	void HideThisWidget_Implementation();

	UFUNCTION()
		void OnConfirmIconClicked();

	UFUNCTION()
		void OnCancleIconClicked();

	bool SetLevelImage(class UTexture2D* TextureToSet);
	bool SetLevelName(const FText& NameToSet);
	bool SetLevelDescription(const FText& DescriptionToSet);
	bool ShowStars(int32 StarsToShow);
	bool SetBestClearTime(float ClearTimeToSet);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UImage* OverviewImage;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* NameTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* DescriptionTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class USizeBox* StarSizeBox_1;

	UPROPERTY(BlueprintReadOnly)
		class USizeBox* StarSizeBox_2;

	UPROPERTY(BlueprintReadOnly)
		class USizeBox* StarSizeBox_3;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* ClearTimeTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* ConfirmIconWidget;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* CancleIconWidget;

public:
	bool BindToLevel(class UKRVLevelDataAsset* NewLevelDA, const FKRVLevelSaveData& NewLevelSaveDA);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVLevelDataAsset* BoundLevelDA;

	UPROPERTY(BlueprintReadOnly)
		FKRVLevelSaveData BoundLevelSaveData;

	bool bIsBound;
	int32 EarnedStars = 0;

};
