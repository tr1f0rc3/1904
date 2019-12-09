// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVEnums.h"
#include "KRVInGameMainHeroUserWidget.generated.h"

UCLASS()
class KRV_API UKRVInGameMainHeroUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	bool BindToHero(class AKRVHero* NewHero);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVIconUserWidget* HeroIconWidget;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVIconUserWidget* SkillWidget_1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVIconUserWidget* SkillWidget_2;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVPBUserWidget* HPBarWidget;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVPBUserWidget* ExpBarWidget;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class AKRVHero* BoundHero;

};
