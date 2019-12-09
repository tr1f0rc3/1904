// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVEnums.generated.h"

enum
{
	MaxItemsToEquip = 3,
	MaxSkillSlotsToActive = 2,
	MaxSkillsToLearn = 4,
	MaxSkillMasteryLevel = 3,
	MaxTowersToEquip = 5,
	MaxTowerUpgradesToHave = 5,
	MaxHeroesToPick = 1,
	
};

UENUM(BlueprintType)
enum class EKRVTeam : uint8
{
	NONE,
	Player,
	Enemy,
	MAX,

};

UENUM(BlueprintType)
enum class EGameState : uint8
{
	NONE,
	Preparing,
	Spawning,
	AllSpawnFinished,
	Incoming,
	Defeated,
	Cleared,
	MAX,

};

UENUM(BlueprintType)
enum class ETargetType : uint8
{
	NONE,
	ToSelf,
	FriendlyCharacter,
	NotFriendlyCharacter,
	FriendlyTower,
	AnyHostile,
	HostileCharacter,
	NotHostileCharacter,
	HostileTower,
	AnyUnitNotSelf,
	AnyUnit,
	AnyPawn,
	MAX,

};

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	NONE,
	Armor,
	Flesh,
	MAX,

};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NONE,
	NoWeapon,
	OneHand,
	TwoHand,
	DoubleHand,
	Wand,
	Bow,
	Shield,
	MAX,

};

UENUM(BlueprintType)
enum class EDamageReductionType : uint8
{
	NONE,
	Physical,
	Magic,
	TrueDamage,
	MAX,

};

UENUM(BlueprintType)
enum class EDamageEffectType : uint8
{
	NONE,
	Arrow,
	Slash,
	Smash,
	Explosion,
	Fire,
	Thunder,
	Pierce,
	Dark,
	Poison,
	Ice,
	Heal,
	Pulse,
	MAX,

};


UENUM(BlueprintType)
enum class ERangedMediumType : uint8
{
	NONE,
	NoWeapon,
	Arrow,
	Bomb,
	Mine,
	MAX,

};

UENUM(BlueprintType)
enum class ETargetingType : uint8
{
	NONE,
	Self,
	Target,
	Location,
	Passive,
	MAX,

};

UENUM(BlueprintType)
enum class EActionStatus : uint8
{
	NONE,
	Idle,
	Stunned,
	Casting,
	Attacking,
	Moving,
	Dead,
	MAX,

};

UENUM(BlueprintType)
enum class ECombatStatus : uint8
{
	NONE,
	Melee,
	Ranged,
	OnGuard,
	Idle,
	FollowingPath,
	ForcedMoving,
	MAX,

};


UENUM(BlueprintType)
enum class ETowerUpgradeType : uint8
{
	NONE,
	NewTower,
	TowerStat,
	CharacterStat,
	TowerSkill,
	CharacterSkill,
	MAX,

};

UENUM(BlueprintType)
enum class EGearUpgradePart : uint8
{
	NONE,
	Ranged,
	LeftMelee,
	RightMelee,
	MAX

};

UENUM(BlueprintType)
enum class EIconType : uint8
{
	NONE,
	Menu,
	Skill,
	Item,
	TowerUpgrade,
	TowerSelling,
	Hero,
	Selection,
	HeroInMap,
	ItemInMap,
	ItemPurchase,
	TowerInMap,
	Gateway,
	Battlefield,
	SkillInMap,
	SkillLearning,
	MAX

};