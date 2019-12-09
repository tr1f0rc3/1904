// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVEnums.h"
#include "KRVMapSkillSlotUserWidget.generated.h"

UCLASS()
class KRV_API UKRVMapSkillSlotUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

public: // Widgets
	UFUNCTION(BlueprintCallable)
		class UKRVIconUserWidget* GetSkillIconWidget() const;

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* SkillIconWidget;

	UPROPERTY(BlueprintReadOnly)
		class UBorder* LevelBorder;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* LevelTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* LearningIconWidget;

	UPROPERTY(Transient, BlueprintReadOnly)
		bool bIsBound;

public: // Skill selection in map
	bool BindToSkillInMap(TSubclassOf<class AKRVSkill> SkillClassToBind, UKRVDataAsset* ItemDAToBind, const FName& NewHero);
	void SetIconLocked();

protected:
	UFUNCTION()
		void OnPointGivenToHeroSkill(int32 NewSkillPoint, class UKRVDataAsset* NewDA, const FName& NewHeroName);

protected:
	UPROPERTY(BlueprintReadOnly)
		FName HeroName;

	UPROPERTY()
		class UKRVDataAsset* SkillDA;

};
