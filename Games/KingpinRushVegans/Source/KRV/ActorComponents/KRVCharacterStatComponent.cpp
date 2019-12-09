
#include "KRVCharacterStatComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "KRV.h"
#include "Structs/KRVCharacterStatData.h"
#include "KRVGameInstance.h"

UKRVCharacterStatComponent::UKRVCharacterStatComponent()
{
}


void UKRVCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();
	InitHealth();

}


UKRVStatComponent* UKRVCharacterStatComponent::LoadData(const FName& UnitName)
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
	const FKRVCharacterStatData* const LoadedStatData = KRVGameInstance->GetCharacterStatData(UnitName);
	if (!LoadedStatData)
	{
		KRVCHECK(LoadedStatData);
		return nullptr;
	}

	MeleeAttackStat.DamageType = LoadedStatData->Melee_DamageType;
	MeleeAttackStat.MinDamage = LoadedStatData->Melee_MinDamage;
	MeleeAttackStat.MaxDamage = LoadedStatData->Melee_MaxDamage;
	MeleeAttackStat.AttackRate = LoadedStatData->Melee_AttackRate;
	MeleeAttackStat.AttackRange = LoadedStatData->Melee_AttackRange;

	RangedAttackStat.MinDamage = LoadedStatData->Ranged_MinDamage;
	RangedAttackStat.MaxDamage = LoadedStatData->Ranged_MaxDamage;
	RangedAttackStat.DamageType = LoadedStatData->Ranged_DamageType;
	RangedAttackStat.AttackRate = LoadedStatData->Ranged_AttackRate;
	RangedAttackStat.AttackRange = LoadedStatData->Ranged_AttackRange;

	RangedMediumType = LoadedStatData->RangedMediumType;
	Sight = LoadedStatData->Sight;

	DefenceStat.MaxHealth = LoadedStatData->MaxHealth;
	DefenceStat.HealthRegenInRest = LoadedStatData->HealthRegenInRest;
	DefenceStat.HealthRegenInCombat = LoadedStatData->HealthRegenInCombat;
	DefenceStat.Armor = LoadedStatData->Armor;
	DefenceStat.MagicResistance = LoadedStatData->MagicResistance;

	MovementSpeed = LoadedStatData->MovementSpeed;
	LivesTaken = LoadedStatData->LivesTaken;
	Bounty = LoadedStatData->Bounty;
	ExpGiven = LoadedStatData->ExpGiven;

	return this;
}

UKRVCharacterStatComponent* UKRVCharacterStatComponent::AssignmentOperator(const UKRVCharacterStatComponent* const Other)
{
	UKRVStatComponent::AssignmentOperator(Cast<UKRVStatComponent>(Other));
	MeleeAttackStat = Other->MeleeAttackStat;
	DefenceStat = Other->DefenceStat;
	MovementSpeed = Other->MovementSpeed;
	return this;

}

void UKRVCharacterStatComponent::InitHealth()
{
	SetHealth(float(DefenceStat.MaxHealth));
}

float UKRVCharacterStatComponent::GetHealth() const
{
	return CurrentHealth;
}

float UKRVCharacterStatComponent::GetHealthRatio() const
{
	return CurrentHealth / DefenceStat.MaxHealth;
}

float UKRVCharacterStatComponent::SetHealth(float _Health)
{
	CurrentHealth = FMath::Clamp<float>(_Health, 0.f, float(DefenceStat.MaxHealth));
	OnHPChangedDelegate.Broadcast(CurrentHealth, DefenceStat.MaxHealth);
	OnHpChangedDynamicDelegate.Broadcast(CurrentHealth, DefenceStat.MaxHealth);
	return CurrentHealth;
}

float UKRVCharacterStatComponent::LoseHealth(float _Health)
{
	return SetHealth(CurrentHealth - _Health);

}

float UKRVCharacterStatComponent::GainHealth(float _Health)
{
	return SetHealth(CurrentHealth + _Health);

}




