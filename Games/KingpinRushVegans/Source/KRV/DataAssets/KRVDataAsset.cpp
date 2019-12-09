// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVDataAsset.h"

void UKRVDataAsset::GetOwnedGameplayTags(FGameplayTagContainer& NewTagContainer) const
{
	NewTagContainer = TagContainer;
	return;

}