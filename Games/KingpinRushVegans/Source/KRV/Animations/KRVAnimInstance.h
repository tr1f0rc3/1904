// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "KRVEnums.h"
#include "Interfaces/KRVDataAssetInterface.h"
#include "KRVAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackFireNotifyDelegate);

UENUM(BlueprintType)
enum class EBlendPart : uint8
{
	NONE,
	Cervical,
	UpperBody,
	FullBody,
	MAX,
};

UCLASS()
class KRV_API UKRVAnimInstance : public UAnimInstance/*, public IKRVDataAssetInterface*/
{
	GENERATED_BODY()

public:
	UKRVAnimInstance();
	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected: // Pawn 
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, Category = "Pawn")
		class AKRVCombatUnit* KRVCombatUnit;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, Category = "Pawn")
		float PawnWalkSpeed;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, Category = "Pawn")
		EWeaponType WeaponType = EWeaponType::NoWeapon;

public:
	class AKRVCombatUnit* TryGetKRVCombatUnit();

protected:
	UFUNCTION()
		void AnimNotify_AttackFire();

public:
	FOnAttackFireNotifyDelegate OnAttackFireNotifyDelegate;

public: // Slot
	UFUNCTION(BlueprintCallable)
		FName GetSlotNameByEnum(EBlendPart BodyPart, EWeaponType NewWeaponType) const;

protected:
	static const FName FullBodyDefaultSlotName;
	static const FName FullBodyNoWeaponSlotName;
	static const FName FullBodyOneHandSlotName;
	static const FName FullBodyTwoHandSlotName;
	static const FName FullBodyDoubleHandSlotName;
	static const FName FullBodyWandSlotName;
	static const FName FullBodyBowSlotName;
	static const FName UpperBodyDefaultSlotName;
	static const FName UpperBodyNoWeaponSlotName;
	static const FName UpperBodyOneHandSlotName;
	static const FName UpperBodyTwoHandSlotName;
	static const FName UpperBodyDoubleHandSlotName;
	static const FName UpperBodyWandSlotName;
	static const FName UpperBodyBowSlotName;
	static const FName CervicalNoWeaponSlotName;

};
