// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVEnums.h"
#include "DataAssets/KRVDataAsset.h"
#include "KRVWeaponDataAsset.generated.h"

UCLASS()
class KRV_API UKRVWeaponDataAsset : public UKRVDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSoftObjectPtr<class UStaticMesh> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		ECombatStatus WeaponRangeType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		EWeaponType WeaponType;

};
