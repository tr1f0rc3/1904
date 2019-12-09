// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVAnimInstance.h"
#include "Engine/AssetManager.h"

#include "KRV.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "DataAssets/KRVAnimInstanceDataAsset.h"

const FName UKRVAnimInstance::FullBodyDefaultSlotName(TEXT("F_Default"));
const FName UKRVAnimInstance::FullBodyNoWeaponSlotName(TEXT("F_NoWeapon"));
const FName UKRVAnimInstance::FullBodyOneHandSlotName(TEXT("F_OneHand"));
const FName UKRVAnimInstance::FullBodyTwoHandSlotName(TEXT("F_TwoHand"));
const FName UKRVAnimInstance::FullBodyDoubleHandSlotName(TEXT("F_DoubleHand"));
const FName UKRVAnimInstance::FullBodyWandSlotName(TEXT("F_Wand"));
const FName UKRVAnimInstance::FullBodyBowSlotName(TEXT("F_Bow"));
const FName UKRVAnimInstance::UpperBodyDefaultSlotName(TEXT("U_Default"));
const FName UKRVAnimInstance::UpperBodyNoWeaponSlotName(TEXT("U_NoWeapon"));
const FName UKRVAnimInstance::UpperBodyOneHandSlotName(TEXT("U_OneHand"));
const FName UKRVAnimInstance::UpperBodyTwoHandSlotName(TEXT("U_TwoHand"));
const FName UKRVAnimInstance::UpperBodyDoubleHandSlotName(TEXT("U_DoubleHand"));
const FName UKRVAnimInstance::UpperBodyWandSlotName(TEXT("U_Wand"));
const FName UKRVAnimInstance::UpperBodyBowSlotName(TEXT("U_Bow"));
const FName UKRVAnimInstance::CervicalNoWeaponSlotName(TEXT("C_Default"));

UKRVAnimInstance::UKRVAnimInstance()
{
	//PawnWalkSpeed = 0.f;

}

void UKRVAnimInstance::NativeInitializeAnimation()
{
	Super::NativeBeginPlay();
	KRVCombatUnit = TryGetKRVCombatUnit();

}

void UKRVAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (KRVCombatUnit)
	{
		PawnWalkSpeed = KRVCombatUnit->GetVelocity().Size();
		WeaponType = KRVCombatUnit->GetCurrentWeaponType();
	}
	else
	{
		KRVCombatUnit = TryGetKRVCombatUnit();
	}

}

AKRVCombatUnit* UKRVAnimInstance::TryGetKRVCombatUnit()
{
	if (KRVCombatUnit)
	{
		return KRVCombatUnit;
	}
	else
	{
		APawn* ReturnedPawn = TryGetPawnOwner();
		if (!IsValid(ReturnedPawn))
		{
			return nullptr;
		}
		else
		{
			KRVCombatUnit = Cast<AKRVCombatUnit>(ReturnedPawn);
			return KRVCombatUnit;
		}
	}
}

void UKRVAnimInstance::AnimNotify_AttackFire()
{
	OnAttackFireNotifyDelegate.Broadcast();

}

FName UKRVAnimInstance::GetSlotNameByEnum(EBlendPart NewBodyPart, EWeaponType NewWeaponType) const
{
	FName OutName;

	switch (NewBodyPart)
	{
	case EBlendPart::Cervical:
	{
		switch (NewWeaponType)
		{
		case EWeaponType::NoWeapon:
			OutName = CervicalNoWeaponSlotName;
			break;
		default:
			break;
		}
		break;
	}
	case EBlendPart::UpperBody:
	{
		switch (NewWeaponType)
		{
		case EWeaponType::NoWeapon:
			OutName = UpperBodyNoWeaponSlotName;
			break;
		case EWeaponType::OneHand:
			OutName = UpperBodyOneHandSlotName;
			break;
		case EWeaponType::TwoHand:
			OutName = UpperBodyTwoHandSlotName;
			break;
		case EWeaponType::DoubleHand:
			OutName = UpperBodyDoubleHandSlotName;
			break;
		case EWeaponType::Wand:
			OutName = UpperBodyWandSlotName;
			break;
		case EWeaponType::Bow:
			OutName = UpperBodyBowSlotName;
			break;
		case EWeaponType::Shield:
			OutName = UpperBodyOneHandSlotName;
			break;
		default:
			break;
		}
		break;
	}
	case EBlendPart::FullBody:
	{
		switch (NewWeaponType)
		{
		case EWeaponType::NoWeapon:
			OutName = FullBodyBowSlotName;
			break;
		case EWeaponType::OneHand:
			OutName = FullBodyOneHandSlotName;
			break;
		case EWeaponType::TwoHand:
			OutName = FullBodyTwoHandSlotName;
			break;
		case EWeaponType::DoubleHand:
			OutName = FullBodyDoubleHandSlotName;
			break;
		case EWeaponType::Wand:
			OutName = FullBodyWandSlotName;
			break;
		case EWeaponType::Bow:
			OutName = FullBodyBowSlotName;
			break;
		case EWeaponType::Shield:
			OutName = FullBodyOneHandSlotName;
			break;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
	return OutName;

}

