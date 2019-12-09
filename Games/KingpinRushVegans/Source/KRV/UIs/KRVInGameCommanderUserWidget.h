// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVInGameCommanderUserWidget.generated.h"

UCLASS()
class KRV_API UKRVInGameCommanderUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

public:
	bool BindToGlobalSkills(class AKRVCombatUnit* NewCommander);
	bool BindToItems(class AKRVCombatUnit* NewCommander);

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVIconUserWidget* SkillWidget_1;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVIconUserWidget* SkillWidget_2;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVIconUserWidget* SkillWidget_3;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class AKRVCombatUnit* BoundCommander;

};
