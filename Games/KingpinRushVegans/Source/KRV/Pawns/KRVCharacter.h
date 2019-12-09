// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawns/KRVCombatUnit.h"
#include "KRV.h"
#include "KRVEnums.h"
#include "Structs/KRVAttackStat.h"
#include "Structs/KRVDefenceStat.h"
#include "DataAssets/KRVDataAsset.h"
#include "Navigation/PathFollowingComponent.h"
#include "KRVCharacter.generated.h"

class USoundCue;
class UAnimInstnace;

class UKRVDamageType;
class UKRVCharacterDataAsset;
class USpawnerComponent;
class UPooledComponent;
class UKRVCharacterStatComponent;
class AKRVBuff;
class UCombatUnitComponent;
class AKRVProjectile;

DECLARE_MULTICAST_DELEGATE_OneParam(FRemoveFromAggroListDelegate, AKRVCharacter*);

UCLASS()
class KRV_API AKRVCharacter : public AKRVCombatUnit
{
	GENERATED_BODY()

public:
	AKRVCharacter();

public:
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public: // Data Asset
	UFUNCTION(BlueprintCallable)
		UKRVCharacterDataAsset* GetCharacterDataAsset() const;

	virtual bool LoadFromDataAsset_Implementation() override;
	virtual bool LoadBaseStat() override;

protected:
	void OnLeftMeleeWeaponAsyncLoaded();
	void OnRightMeleeWeaponAsyncLoaded();
	virtual void OnDieMontageAsyncLoaded() override;


protected:
	FSoftObjectPath LeftMeleeWeaponMeshPath;
	FSoftObjectPath RightMeleeWeaponMeshPath;

public:
	UFUNCTION(BlueprintCallable)
		AKRVCombatUnit* GetSpawnerOfThis() const;

protected:
	void CheckFallingAndRestoreLocation();

public:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Spawn")
		bool bIsRecruitedByTower;

	void OnEscaped();

protected: // GameState
	virtual void OnGameEnd(EKRVTeam Winner);

public: // Controller && UI
	virtual void BePossessedByAIController(EKRVTeam Team, bool bDoesPlayNow = true) override;
	virtual void BindToWidgetsOnBeginPlay() override;
	virtual void OnSelected(bool bIsSelected) override;

protected: // Anim
	virtual void OnAnimFireNotified() override;

public: // Stat
	virtual UKRVStatComponent* VariateBaseStat(TSubclassOf<AKRVBuff> BuffClass) override;
	virtual UKRVStatComponent* GetBaseStat() const override;
	virtual UKRVStatComponent* GetModifiedStat() const override;

	UFUNCTION(BlueprintCallable)
		UKRVCharacterStatComponent* GetBaseCharacterStat() const;

	UFUNCTION(BlueprintCallable)
		UKRVCharacterStatComponent* GetModifiedCharacterStat() const;

protected:
	virtual void UpdateStat() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
		UKRVCharacterStatComponent* BaseCharacterStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
		UKRVCharacterStatComponent* ModifiedCharacterStat;


public:
	// Given in spawn. Ruby of most spawned unit is 0.
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Stat")
		int32 Ruby;

protected: // Buff
	virtual bool TakeBuff(AKRVBuff* NewBuff) override;

public: // Heal & regen
	UFUNCTION(BlueprintCallable, Category = "Heal")
		float GetHealed(float HealAmount);

	UFUNCTION()
		void HealthRegen();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Heal")
		UParticleSystemComponent* HealParticleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Heal")
		UAudioComponent* HealAudioComponent;


	FTimerHandle HealthRegenTimer;

public: // Combat status
	virtual bool MakeStunned(float NewStunDuration) override;
	virtual bool IsAbleToAttack() const override;

	UFUNCTION(BlueprintCallable)
		bool SetFollowingPathStatus(bool bIsFollowingPath = true);

	UFUNCTION(BlueprintCallable)
		float GetInvincibleRemainingTime() const;

	UFUNCTION(BlueprintCallable)
		bool BeInvincible(float NewDuration);

protected:
	virtual void ResetStatus() override;
	virtual bool StopAction() override;
	virtual void OnStunTimeUp() override;
	virtual void ClearAllActiveActionTimer() override;
	virtual void ClearAllPassiveActionTimer() override;

	void OnInvincibleTimeUp();

protected:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Status")
		bool bIsInvincible;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
		UAudioComponent* InvincibleAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
		UParticleSystemComponent* InvincibleParticleComponent;

	FTimerHandle InvincibleTimer;

public:
	virtual ECombatStatus FindNearestEnemyRange(class AKRVCharacter*& OutCharacter, ECombatStatus StartRange = ECombatStatus::Idle) override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Status")
		void SetRecentlyHit(bool NewHit);

	void OnRecentlyHitTimeUp();

protected:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Status")
		bool bIsRecentlyHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		float RecentlyHitRecoveryTime = 5.f;

	FTimerHandle RecentlyHitTimer;

protected: // Reserve Action
	virtual bool DoReservedAction() override;

public: // Ranged
	virtual bool Fire() override;
	virtual bool IsAbleToFire() const override;
	virtual void OnAttackCooldownTimeUp() override;
	virtual void OnAttackStatusTimeUp() override;
	virtual float GetAttackRemainingTime() override;

	virtual EWeaponType GetCurrentWeaponType() const override;

public:
	virtual bool CastSkill(const FGameplayTag& SkillTagToCast, AKRVCombatUnit* NewUnit = nullptr, const FVector& NewPoint = FVector::ZeroVector) override;
	virtual void OnSkillCasted() override;

public: // Aggro
	// This fcn is to prevent to be trapped infinite loop when calling ChargeToMelee().
	// ChargeToMelee()를 부를 때 무한루프에 빠지는 걸 방지하기 위한 함수.
	UFUNCTION(BlueprintCallable)
		bool GetAggro();

	UFUNCTION(BlueprintCallable)
		bool IsInChargeRange(AKRVCharacter* CharacterToCharge = nullptr) const;

	UFUNCTION(BlueprintCallable)
		bool ChargeToMelee();

protected:
	UFUNCTION(BlueprintCallable)
		float GetMeleeAttackableDistance() const;

	UFUNCTION(BlueprintCallable)
		bool FindCharactersToMeleeWithTurn(TArray<AKRVCharacter*>& OutEnemies) const;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggro")
		float MeleeAttackRangeMargin = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggro")
		float TauntRange = 400.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aggro")
		UParticleSystemComponent* AggroParticleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aggro")
		UAudioComponent* AggroAudioComponent;

public: // Melee
	UFUNCTION(BlueprintCallable)
		bool IsAbleToMeleeAttack() const;

	UFUNCTION(BlueprintCallable)
		bool IsInMeleeAttackRange() const;

	UFUNCTION(BlueprintCallable)
		bool MeleeAttack();

	UFUNCTION(BlueprintCallable)
		bool LoadMeleeWeapon(bool bIsLeft = true, int32 UpgradeIndex = 0);

protected:
	UFUNCTION(BlueprintCallable)
		void ShowMeleeRadiusDecal(bool bDoesShow);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stat")
		class UDecalComponent* MeleeRadiusDecal;

public: // WeaponSwap
	UFUNCTION(BlueprintCallable)
		ECombatStatus GetAttackRangeType() const;

protected:
	UFUNCTION(BlueprintCallable)
		bool WeaponSwap(ECombatStatus NewStatus);

	void OnMeleeAttackFire();

protected:
	FTimerHandle MeleeAttackRateTimer;

	UPROPERTY(BlueprintReadOnly)
		ECombatStatus CurrentWeaponRangeType;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
		EWeaponType LeftMeleeWeaponType = EWeaponType::NoWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		UStaticMeshComponent* LeftMeleeWeaponMesh;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Weapon")
		int32 LeftMeleeWeaponUpgradeIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
		EWeaponType RightMeleeWeaponType = EWeaponType::NoWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
		UStaticMeshComponent* RightMeleeWeaponMesh;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Weapon")
		int32 RightMeleeWeaponUpgradeIndex;

public: // Defence;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Armor")
		EArmorType ArmorType;

public: // Move;
	UFUNCTION(BlueprintCallable)
		bool MoveToTargetUnit();

	UFUNCTION(BlueprintCallable)
		bool MoveToTargetPoint(ECombatStatus NewStatus = ECombatStatus::FollowingPath);

	UFUNCTION(BlueprintCallable)
		bool FollowPath(const FVector& NewPoint);

	UFUNCTION(BlueprintCallable)
		class AKRVPath* GetPathToFollow() const;

	bool StopMoving();

protected:
	EPathFollowingRequestResult::Type MoveToTargetUnitWithAI(class AKRVCombatUnit* TargetUnit);
	EPathFollowingRequestResult::Type MoveToTargetPointWithAI(const FVector& NewPoint);

	UFUNCTION()
		void CheckMoveToTargetUnitSucceded();

	UFUNCTION()
		void CheckMoveToTargetPointSucceded();

	void EnableWalk(bool bIsEnable);

	void CheckMoveToTargetUnitSucceded_RecursiveCheck();
	void CheckMoveToTargetPointSucceded_RecursiveCheck();


public: // Path (Enemy Only)
	UPROPERTY(Transient, BlueprintReadOnly)
		TWeakObjectPtr<class AKRVPath> PathToFollow;

	UPROPERTY(Transient, BlueprintReadOnly)
		float SplineTime;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) // Only for flyingUnits
		float FlyHeight = 400.f;

	FTimerHandle MoveSuccessCheckTimer;
	float MoveSuccessCheckDuration = 5.f;
	float MoveToUnitSuccessCheckInteval = 0.1f;
	float MoveToPointSuccessCheckInteval = 0.1f;
	int32 RemainingMoveSuccessCheck = 0;

public: // Die
	virtual bool Die() override;

};
