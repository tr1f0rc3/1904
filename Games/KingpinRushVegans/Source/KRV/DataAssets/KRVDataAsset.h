// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "KRVDataAsset.generated.h"

UCLASS()
class KRV_API UKRVDataAsset : public UPrimaryDataAsset, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Tag")
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& NewTagContainer) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ID")
		FName ClassFName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
		FGameplayTagContainer TagContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Icon")
		TSoftObjectPtr<class UTexture2D> IconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Text")
		FText ClassFText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Text")
		FText Description;


};
