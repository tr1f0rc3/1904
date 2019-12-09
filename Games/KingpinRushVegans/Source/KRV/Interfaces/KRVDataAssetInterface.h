// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVDataAssetInterface.generated.h"

UINTERFACE(MinimalAPI)
class UKRVDataAssetInterface : public UInterface
{
	GENERATED_BODY()

};


class KRV_API IKRVDataAssetInterface
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		class UKRVDataAsset* GetDataAsset() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool LoadFromDataAsset();

};
