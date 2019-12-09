// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs/KRVHeroSaveData.h"
#include "Structs/KRVItemStockData.h"
#include "Structs/KRVTowerSaveData.h"
#include "KRVIconTooltipUserWidget.generated.h"

UCLASS()
class KRV_API UKRVIconTooltipUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool ShowThisWidget(bool bIsTrue);
	virtual bool ShowThisWidget_Implementation(bool bIsTrue);

	UFUNCTION(BlueprintCallable)
		bool IsBoundTo() const;

	bool BindToDataAsset(class UKRVDataAsset* NewDA);

	bool BindToSkill(class AKRVSkill* NewSkill);
	bool BindToHero(class UKRVDataAsset* NewDA);
	bool BindToTowerSelling(class AKRVTower* NewTower);
	bool BindToTowerUpgrade(int32 UpgradeIndex, class AKRVTower* NewTower);
	bool BindtoGateway(class AKRVGateway* NewGateway);
	bool BindToHeroInMap(class UKRVHeroDataAsset* HeroDAToBind, bool bIsUnlocked);
	bool BindToItemInMap(class UKRVItemDataAsset* ItemDAToBind, bool bIsUnlocked);
	bool BindToTowerInMap(class UKRVTowerDataAsset* TowerDAToBind, bool bIsUnlocked);

	bool SetTexts(const FText& NewTitle, const FText& NewText = FText::GetEmpty(), const FText& NewRequirement = FText::GetEmpty());
	bool SetTitleText(const FText& NewText);
	bool SetTextText(const FText& NewText);
	bool SetWarningText(const FText& NewText);

protected:
	UFUNCTION(BlueprintCallable)
		void HideThisWidget();

	void SetSizeBoxAuto();
	void ResetTexts();
	bool IsAllTextsEmpty();

protected:
	UPROPERTY(BlueprintReadOnly)
		class USizeBox* MainSizeBox;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* TitleTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* TextTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* WarningTextBlock;

	UPROPERTY(Transient, BlueprintReadOnly)
		bool bIsBound;

protected: // default texts
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName NotLearnedSkillText = "This is not learned yet.";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName MaxTowerUpgradeText = "Max level reached.";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName TowerSellingText = "Sells this tower.";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName UnableToSellingText = "This cannot be sold.";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName HeroText = "Toggles selecting the hero.";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName NextWaveIncomingText = "Next wave:";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName FlyingUnitWarningText = "Prepare to flying units!";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName UnlockedText = "This is locked.";

};
