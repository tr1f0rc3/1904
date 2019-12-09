// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/KRVDataAsset.h"
#include "KRVAnimInstanceDataAsset.generated.h"

UCLASS()
class KRV_API UKRVAnimInstanceDataAsset : public UKRVDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<class UAnimInstance> AnimInstanceClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSoftObjectPtr<class UAnimMontage> GetHitMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<TSoftObjectPtr<class UAnimMontage>> AttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSoftObjectPtr<class UAnimMontage> StunMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSoftObjectPtr<class UAnimMontage> CastingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSoftObjectPtr<class UAnimMontage> VictoryMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSoftObjectPtr<class UAnimMontage> WeaponSwapMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSoftObjectPtr<class UAnimMontage> DieMontage;


};
