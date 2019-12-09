// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs/KRVHeroSaveData.h"
#include "KRVMapHeroMenuUserWidget.generated.h"

UCLASS()
class KRV_API UKRVMapHeroMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;

protected:
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

protected:
	UPROPERTY(BlueprintReadOnly)
		TArray<class UKRVIconUserWidget*> HeroIcons;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UKRVMapSkillSlotUserWidget*> SkillSlots;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* PickedHeroIcon;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* ConfirmIconWidget;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* CancleIconWidget;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* ResetSkillPointsIconWidget;

public:
	void BindToPlayerState(class AKRVMapPlayerState* PlayerStateToBind);

protected:
	UFUNCTION()
		void OnHeroPicked(const FName& EquippedHero);

	UFUNCTION()
		void OnHeroClicked(const FName& ClickedHero);

	UFUNCTION()
		void OnResetSkillButtonClicked();

protected:
	UPROPERTY(BlueprintReadOnly)
		FName ClickedHeroName;

	UPROPERTY()
		TArray<class UKRVHeroDataAsset*> HeroDAArray;

	UPROPERTY()
		TArray<FKRVHeroSaveData> HeroSaveDataArray;

	TMap<FName, int32> HeroIndexMap;
	int32 EquipCount = 0;

	FString SkillResetTitle = FString(TEXT("Reset Skill Points"));
	FString SkillResetText = FString(TEXT("Reset the hero skill points"));

};
