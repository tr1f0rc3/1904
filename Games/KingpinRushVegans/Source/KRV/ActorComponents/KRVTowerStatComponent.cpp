// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVTowerStatComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "KRV.h"
#include "Structs/KRVTowerStatData.h"
#include "KRVGameInstance.h"


UKRVStatComponent* UKRVTowerStatComponent::LoadData(const FName& UnitName)
{
	Super::LoadData(UnitName);

	UWorld* World = GetWorld();
	if (!World)
	{
		KRVCHECK(World);
		return nullptr;
	}
	UKRVGameInstance* const KRVGameInstance = Cast<UKRVGameInstance>(UGameplayStatics::GetGameInstance(World));
	if (!KRVGameInstance)
	{
		KRVCHECK(KRVGameInstance);
		return nullptr;
	}
	const FKRVTowerStatData* const LoadedStatData = KRVGameInstance->GetTowerStatData(UnitName);
	if (!LoadedStatData)
	{
		KRVCHECK(LoadedStatData);
		return nullptr;
	}

	RangedAttackStat.DamageType = LoadedStatData->Ranged_DamageType;
	RangedAttackStat.MinDamage = LoadedStatData->Ranged_MinDamage;
	RangedAttackStat.MaxDamage = LoadedStatData->Ranged_MaxDamage;
	RangedAttackStat.AttackRate = LoadedStatData->Ranged_AttackRate;
	RangedAttackStat.AttackRange = LoadedStatData->Ranged_AttackRange;

	SplashAttackStat.DamageType = LoadedStatData->Splash_DamageType;
	SplashAttackStat.MinDamage = LoadedStatData->Splash_MinDamage;
	SplashAttackStat.MaxDamage = LoadedStatData->Splash_MaxDamage;
	SplashAttackStat.AttackRate = LoadedStatData->Splash_AttackRate;
	SplashAttackStat.AttackRange = LoadedStatData->Splash_AttackRange;
	
	RangedMediumType = LoadedStatData->RangedMediumType;
	WaypointRadius = LoadedStatData->WaypointRadius;
	Sight = LoadedStatData->Sight;

	return this;
}

UKRVTowerStatComponent* UKRVTowerStatComponent::AssignmentOperator(const UKRVTowerStatComponent* const Other)
{
	UKRVStatComponent::AssignmentOperator(Cast<UKRVStatComponent>(Other));
	SplashAttackStat = Other->SplashAttackStat;
	return this;

}

