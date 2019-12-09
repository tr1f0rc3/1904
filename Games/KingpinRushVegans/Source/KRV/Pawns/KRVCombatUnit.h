// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"

#include "KRVEnums.h"
#include "Interfaces/KRVDataAssetInterface.h"
#include "Structs/KRVReservedAction.h"
#include "Structs/KRVSkillMasterySaveData.h"
#include "KRVCombatUnit.generated.h"

class USoundCue;
class UAnimInstance;
class UAudioComponent;
class UParticleSystemComponent;
class UKRVShooterComponentDataAsset;
class UKRVCombatUnitDataAsset;
class UKRVStatComponent;
class UKRVShooterComponent;
class USpawnerComponent;
class AKRVSkill;
class AKRVBuff;
class AKRVProjectile;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSelectedDelegate, bool, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStatUpdatedDelegate, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnActionStatusChangedDelegate, EActionStatus, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCombatStatusChangedDelegate, ECombatStatus, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTargetUnitChangedDelegate, AKRVCombatUnit*, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTargetPointChangedDelegate, const FVector&, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNearestEnemyRangeChangedDelegate, ECombatStatus, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFiringDelegate, AKRVProjectile*, AKRVCombatUnit*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFiringDynamicDelegate, AKRVProjectile*, FiredProjectile, AKRVCombatUnit*, Shooter);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillCastBeginDelegate, AKRVSkill*, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillCastEndDelegate, AKRVSkill*, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillCastCancleDelegate, AKRVSkill*, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillAcquireDelegate, AKRVSkill*, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDieDelegate, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnDieWithRewardDelegate, int32, int32, int32, AKRVCombatUnit*);

UCLASS(Abstract)
class KRV_API AKRVCombatUnit : public ACharacter, public IKRVDataAssetInterface
{
	GENERATED_BODY()

public:
	AKRVCombatUnit();

protected:
	virtual void BeginPlay() override;

public:
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

public: // Asset
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		class UKRVDataAsset* GetDataAsset() const;
	virtual UKRVDataAsset* GetDataAsset_Implementation() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool LoadFromDataAsset();
	virtual bool LoadFromDataAsset_Implementation() override;

	UFUNCTION(BlueprintCallable)
		UKRVCombatUnitDataAsset* GetCombatUnitDataAsset() const;

	UFUNCTION(BlueprintCallable)
		virtual bool LoadBaseStat();

	UFUNCTION(BlueprintCallable)
		bool LoadRangedWeapon(int32 UpgradeIndex = 0);

protected:
	void OnLeftRangedWeaponAsyncLoaded();
	void OnRightRangedWeaponAsyncLoaded();

	void OnSpawnedParticleAsyncLoaded();
	void OnSpawnedSoundAsyncLoaded();
	void OnDestroyedParticleAsyncLoaded();
	void DeathParticleAsyncLoaded();
	void DeathSoundAsyncLoaded();

	void LoadAttackMontages();
	void LoadAttackMontages_Recursive();

	void OnGetHitMontageAsyncLoaded();
	void OnAttackMontageAsyncLoaded();
	void OnStunMontageAsyncLoaded();
	void OnCastingMontageAsyncLoaded();
	void OnVictoryMontageAsyncLoaded();
	void OnWeaponSwapMontageAsyncLoaded();
	virtual void OnDieMontageAsyncLoaded();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
		TSoftObjectPtr<UKRVCombatUnitDataAsset> SoftPtr_DataAsset;

	UPROPERTY()
		UKRVDataAsset* HardPtr_DataAsset;

	UPROPERTY()
		UKRVShooterComponentDataAsset* HardPtr_MyShooterComponentDataAsset;

	UPROPERTY()
		class UKRVAnimInstanceDataAsset* HardPtr_AnimInstanceDataAsset;

	int32 AttackMontageLoadIndex;

	FSoftObjectPath LeftRangedWeaponMeshPath;
	FSoftObjectPath RightRangedWeaponMeshPath;

	FSoftObjectPath OnSpawnedParticlePath;
	FSoftObjectPath OnSpawnedSoundPath;
	FSoftObjectPath OnDestroyedParticlePath;
	FSoftObjectPath DeathParticlePath;
	FSoftObjectPath DeathSoundPath;

	FSoftObjectPath GetHitMontagePath;
	FSoftObjectPath AttackMontagePath;
	FSoftObjectPath StunMontagePath;
	FSoftObjectPath CastingMontagePath;
	FSoftObjectPath VictoryMontagePath;
	FSoftObjectPath WeaponSwapMontagePath;
	FSoftObjectPath DieMontagePath;

	FName LeftShieldSocket = "LeftShield";
	FName RightShieldSocket = "RightShield";
	FName BackLeftWeaponSocket = "BackLeft";
	FName BackRightWeaponSocket = "BackRight";
	FName BackShieldSocket = "BackShield";

	FName LeftHandSocket = "LeftHand";
	FName RightHandSocket = "RightHand";
	FName BackSocket = "Back";
	FName OverHeadSocket = "OverHead";

public: // Spawn and destroy
	UFUNCTION(BlueprintCallable)
		virtual void DestroyThis();

protected:
	virtual void OnBeginPlayAndSpawned();
	virtual void OnSpawned();
	virtual void OnDestroyed();
	void SetLocationToSpawnedLocation();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UPooledComponent* PooledComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
		UAudioComponent* OnSpawnedAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
		UParticleSystemComponent* OnSpawnedParticleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spawn")
		UParticleSystem* OnDestroyedParticle;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Spawn")
		FVector SpawnedLocation;


public: // GameState 
	UFUNCTION(BlueprintCallable)
		int32 GetWaveAtSpawned() const;

protected:
	UFUNCTION()
		virtual void OnGameEnd(EKRVTeam Winner);

protected:
	// This var of all units begins with game is set to 0
	UPROPERTY(Transient, BlueprintReadOnly, Category = "ID")
		int32 WaveAtSpawned;

public: // Control && UI
	UFUNCTION(BlueprintCallable)
		virtual bool PlayBrainComponent(bool bIsTrue);

	UFUNCTION(BlueprintCallable, Category = "Controller")
		virtual void BePossessedByAIController(EKRVTeam Team, bool bDoesPlayNow = true);

	UFUNCTION(BlueprintCallable)
		class AKRVAIController* GetPossessingKRVAIController();

	UFUNCTION(BlueprintCallable)
		bool IsCurrentlySelected() const;

	UFUNCTION()
		virtual void OnSelected(bool bIsSelected);

	virtual void BindToWidgetsBySpawner();

protected:
	void BeUnPossessedAndDestroyController();
	virtual void BindToWidgetsOnBeginPlay();


public:
	FOnSelectedDelegate OnSelectedDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class UWidgetComponent* CombatUnitWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class UWidgetComponent* KillRewardWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Control")
		class UDecalComponent* SelectionDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Control")
		FVector DecalLocation = FVector(0.f, 0.f, -90.f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Control")
		class UAudioComponent* SelectionAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Control")
		class UMaterialInterface* PlayerSelectionDecalMI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Control")
		class UMaterialInterface* EnemySelectionDecalMI;

public:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Control")
		TWeakObjectPtr<class AKRVAIController> MainAIController;

protected:
	bool bIsCurrentlySelected;

protected: // Anim
	UFUNCTION(BlueprintCallable)
		class UKRVAnimInstance* GetKRVAnimInstance() const;

	UFUNCTION(BlueprintCallable)
		bool PlayAttackMontage(EWeaponType NewWeaponType, float AttackRate = 0);

	UFUNCTION(BlueprintCallable)
		bool PlayCastingMontage(EWeaponType NewWeaponType);

	UFUNCTION(BlueprintCallable)
		bool PlayStunMontage();

	UFUNCTION(BlueprintCallable)
		bool PlayVictoryMontage(EWeaponType NewWeaponType);

	UFUNCTION(BlueprintCallable)
		bool PlayDieMontage(EWeaponType NewWeaponType);

	UFUNCTION(BlueprintCallable)
		bool PlayWeaponSwapMontage(EWeaponType NewWeaponType);

	UFUNCTION()
		virtual void OnAnimFireNotified();

	void FreezeAsDead();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UAnimMontage* GetHitMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UAnimMontage* StunMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UAnimMontage* CastingMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UAnimMontage* VictoryMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UAnimMontage* WeaponSwapMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UAnimMontage* DieMontage;

	FTimerHandle MontageDelayTimer;

public: // ID
	UFUNCTION(BlueprintCallable)
		FName GetClassFName() const;

	UFUNCTION(BlueprintCallable)
		EKRVTeam GetTeam() const;

	UFUNCTION(BlueprintCallable)
		int32 GetUnitInFieldID() const;

	UFUNCTION(BlueprintCallable)
		bool IsFriendly(AKRVCombatUnit* _Target) const;

	UFUNCTION(BlueprintCallable)
		bool IsHostile(AKRVCombatUnit* _Target) const;

protected:
	void SetTeam(EKRVTeam NewTeam);

protected:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "ID")
		EKRVTeam TeamID;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "ID")
		int32 UnitInFieldID;

public: // Stats
	// Call this fcn to get non-combat stats, not changed by buffs.
	UFUNCTION(BlueprintCallable)
		virtual UKRVStatComponent* GetBaseStat() const;

	// Call this fcn to get health and combat stats, changed by buffs
	UFUNCTION(BlueprintCallable)
		virtual UKRVStatComponent* GetModifiedStat() const;

protected:
	UFUNCTION(BlueprintCallable)
		virtual void UpdateStat();

public: // Buff
	UFUNCTION(BlueprintCallable)
		virtual bool TakeBuffWithStatUpdate(AKRVBuff* NewBuff);

	UFUNCTION(BlueprintCallable)
		virtual bool TakeBuffClassWithStatUpdate(TSubclassOf<AKRVBuff> NewBuffClass, AKRVCombatUnit* Caster);

	// buff maybe removed by timer or forcibly clenased by a combat unit
	UFUNCTION(BlueprintCallable)
		virtual bool RemoveBuffWithStatUpdate(AKRVBuff* NewBuff);

	UFUNCTION(BlueprintCallable)
		virtual void RemoveAllBuffsWithStatUpdate(ETargetType NewTargetType);

	UFUNCTION(BlueprintCallable)
		virtual UKRVStatComponent* VariateBaseStat(TSubclassOf<AKRVBuff> BuffClass);

protected:
	AKRVBuff* UpdateBuffStatSum();
	virtual bool TakeBuff(AKRVBuff* NewBuff);
	bool RemoveBuff(AKRVBuff* NewBuff);
	void RemoveAllBuffs(ETargetType NewTargetType);


public:
	// delegate to call after stat updated
	FOnStatUpdatedDelegate OnStatUpdatedDelegate;

protected:
	// it does not contain DOT info. just for calculating modified stat.
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
		AKRVBuff* BuffSum;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
		TArray<AKRVBuff*> Buffs;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Stat", meta = (AllowPrivateAccess = "true"))
		TArray<AKRVBuff*> Debuffs;

	UPROPERTY()
		AKRVBuff* Ptr_TemporaryBuff;


public: // Status
	UFUNCTION(BlueprintCallable)
		ECombatStatus GetCombatStatus() const;

	UFUNCTION(BlueprintCallable)
		EActionStatus GetActionStatus() const;

	UFUNCTION(BlueprintCallable)
		virtual bool IsAbleToAttack() const;

	UFUNCTION(BlueprintCallable)
		bool IsAbleToMove() const;

	UFUNCTION(BlueprintCallable)
		float GetStunRemainingTime() const;

	UFUNCTION(BlueprintCallable)
		virtual bool MakeStunned(float NewDuration);

	void SetGlobalCaster(bool bIsTrue);

protected:
	UFUNCTION(BlueprintCallable)
		bool SetActionStatus(EActionStatus NewStatus, bool bDoesReset = false);

	UFUNCTION(BlueprintCallable)
		virtual bool SetCombatStatus(ECombatStatus NewStatus);

	UFUNCTION(BlueprintCallable)
		virtual bool StopAction();

	UFUNCTION()
		virtual void ResetStatus();

	UFUNCTION()
		virtual void ClearAllActiveActionTimer();

	UFUNCTION()
		virtual void ClearAllPassiveActionTimer();

	UFUNCTION()
		virtual void OnStunTimeUp();

public:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Status")
		ECombatStatus CurrentCombatStatus;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Status")
		EActionStatus CurrentActionStatus;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Status")
		EActionStatus LastActionStatus;

	FOnCombatStatusChangedDelegate OnCombatStatusChangedDelegate;
	FOnActionStatusChangedDelegate OnActionStatusChangedDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
		UAudioComponent* StunAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
		UParticleSystemComponent* StunParticleComponent;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Status")
		float CombatableRange;

	FTimerHandle StunTimer;

public: // Targeting;
	UFUNCTION(BlueprintCallable)
		float GetMaxEngageRange() const;

	UFUNCTION(BlueprintCallable)
		AKRVCombatUnit* GetTargetUnit() const;

	UFUNCTION(BlueprintCallable)
		FVector GetTargetPoint() const;

	UFUNCTION(BlueprintCallable)
		virtual bool SetTargetUnit(AKRVCombatUnit* NewTarget);

	UFUNCTION(BlueprintCallable)
		virtual bool SetTargetPoint(const FVector& NewTargetLocation);

	UFUNCTION(BlueprintCallable)
		void ClearTargetUnit();

	UFUNCTION(BlueprintCallable)
		bool IsUnitInSight(const AKRVCombatUnit* UnitToCheck = nullptr) const;

	UFUNCTION(BlueprintCallable)
		bool IsTargetPointInCone() const;

	UFUNCTION(BlueprintCallable)
		bool CanSeeTheUnitWithTurn(const AKRVCombatUnit* UnitToSee, bool bDoesDrawDebugLine = false) const;

	UFUNCTION(BlueprintCallable)
		virtual ECombatStatus FindNearestEnemyRange(class AKRVCharacter*& OutCharacter, ECombatStatus StartRange = ECombatStatus::Idle);

	UFUNCTION(BlueprintCallable)
		AKRVCharacter* FindTheNearestFromFowardVector(TArray<AKRVCharacter*>& TargetsToCheck);

	UFUNCTION(BlueprintCallable)
		AKRVCharacter* FindTheNearestCharacter(TArray<AKRVCharacter*>& TargetsToCheck);

	UFUNCTION(BlueprintCallable)
		bool FindVisibleCharactersWithTurn(TArray<AKRVCharacter*>& OutEnemies, float SightRadius, bool bIsFriendly = false) const;

public:
	FOnTargetUnitChangedDelegate OnTargetUnitChangedDelegate;
	FOnTargetPointChangedDelegate OnTargetPointChangedDelegate;
	FOnNearestEnemyRangeChangedDelegate OnNearestEnemyRangeChangedDelegate;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite)
		TWeakObjectPtr<AKRVCombatUnit> CurrentTargetUnit;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite)
		FVector CurrentTargetPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HalfSightAngle = 60.f;


public: // Reserved Action
	void ReserveAction(const FKRVReservedAction& NewAction);

protected:
	virtual bool DoReservedAction();

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		TArray<FKRVReservedAction> ReservedActionsToPop;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		TArray<FKRVReservedAction> ReservedActionsToStack;

	FKRVReservedAction NextActionToDo;

public: // Ranged
	UFUNCTION(BlueprintCallable)
		virtual EWeaponType GetCurrentWeaponType() const;

	//UFUNCTION(BlueprintCallable)
	//	void AutoFire();

	UFUNCTION(BlueprintCallable)
		virtual bool IsAbleToFire() const;

	UFUNCTION(BlueprintCallable)
		bool IsInFireRange(AKRVCombatUnit* UnitToShoot = nullptr) const;

	// not usually called directly
	UFUNCTION(BlueprintCallable)
		virtual bool Fire();

	UFUNCTION(BlueprintCallable)
		virtual float GetAttackRemainingTime();

protected:
	UFUNCTION(BlueprintCallable)
		void ShowFireRadiusDecal(bool bDoesShow);

	UFUNCTION()
		void OnFired();

	UFUNCTION()
		virtual void OnAttackCooldownTimeUp();

	UFUNCTION()
		virtual void OnAttackStatusTimeUp();

public:
	FOnFiringDelegate OnFiringDelegate;

	UPROPERTY(BlueprintAssignable)
		FOnFiringDynamicDelegate OnFiringDynamicDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ranged")
		EWeaponType RangedWeaponType = EWeaponType::NoWeapon;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stat")
		class UDecalComponent* FireRadiusDecal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		UStaticMeshComponent* LeftRangedWeaponMesh;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Ranged")
		int32 LeftRangedWeaponUpgradeIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		UStaticMeshComponent* RightRangedWeaponMesh;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Ranged")
		int32 RightRangedWeaponUpgradeIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		UKRVShooterComponent* ShooterComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ranged")
		ERangedMediumType RangedMediumType;

	float AttackStatusDuration = 1.5f;
	//int32 AutoAttacks = 0;
	//float AutoAttackingTime = 0.f;

	FTimerHandle RangedAttackRateTimer;
	FTimerHandle AttackStatusTimer;


public:	// Skill
	UFUNCTION(BlueprintCallable)
		bool HasActiveSkill() const;

	UFUNCTION(BlueprintCallable)
		FGameplayTag GetReservedSkillTag() const;

	UFUNCTION(BlueprintCallable)
		bool GetSkillTagByIndex(FGameplayTag& OutSkillTag, int32 SkillIndexToGet);

	UFUNCTION(BlueprintCallable)
		AKRVSkill* GetSkillByTag(const FGameplayTag& SkillTagToGet) const;

	UFUNCTION(BlueprintCallable)
		bool SetReservedSkill(const FGameplayTag& SkillTagToSet);

	UFUNCTION(BlueprintCallable)
		float GetMaxSkillRange() const;

	UFUNCTION(BlueprintCallable)
		bool IsSkillReady(const FGameplayTag& SkillTagToCheck);

	UFUNCTION(BlueprintCallable)
		bool IsAbleToCastSomeSkill();

	UFUNCTION(BlueprintCallable)
		bool IsInSkillCastRange(const FGameplayTag& SkillTagToCast, class AKRVCombatUnit* NewTargetUnit = nullptr, const FVector& NewTargetPoint = FVector::ZeroVector) const;

	UFUNCTION(BlueprintCallable)
		AKRVCharacter* FindTheNearestFromFowardVectorInSkillCastRange(const FGameplayTag& SkillTagToCast, TArray<AKRVCharacter*>& TargetsToCheck);

	UFUNCTION(BlueprintCallable)
		virtual bool CastSkill(const FGameplayTag& SkillTagToCast, AKRVCombatUnit* NewUnit = nullptr, const FVector& NewPoint = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable)
		bool AcquireSkill(TSubclassOf<AKRVSkill> NewSkill, int32 LevelToIncrease = 1, bool bDoesIncreaseFromZero = false);

	UFUNCTION(BlueprintCallable)
		bool SetSkillLevels(const TArray<FKRVSkillMasterySaveData>& NewSkillMasterySaveData);

protected:
	UFUNCTION()
		void OnCastingTimeUp();

	UFUNCTION()
		virtual void OnSkillCasted();

	void CancleSkillCasting();
	int32 GetSkillIndexByTag(const FGameplayTag& SkillTag) const;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Caster")
		bool bIsGlobalCaster = false;

	FOnSkillCastBeginDelegate OnSkillCastBeginDelegate;
	FOnSkillCastEndDelegate OnSkillCastEndDelegate;
	FOnSkillCastCancleDelegate OnSkillCastCancleDelegate;
	FOnSkillAcquireDelegate OnSkillAcquireDelegate;

protected:
	UPROPERTY(BlueprintReadOnly)
		TArray<AKRVSkill*> Skills;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		TWeakObjectPtr<AKRVCombatUnit> ReservedUnitToCast;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		FVector ReservedPointToCast;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		FGameplayTag ReservedSkillTagToCast;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		float MaxSkillCastRange = 0.f;

	TMap<FGameplayTag, int32> SkillMap;
	int32 ReservedSkillIndexToCast;
	FTimerHandle SkillCastingTimer;

public: // Death
	UFUNCTION(BlueprintCallable)
		virtual bool Die();

	UFUNCTION(BlueprintCallable)
		void PlayGoldPopUpEffect();

	UFUNCTION(BlueprintCallable)
		void PlayRubyPopUpEffect();

public:
	FOnDieDelegate OnDieDelegate;
	FOnDieWithRewardDelegate OnDieWithRewardDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death")
		UAudioComponent* DeathAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death")
		UParticleSystemComponent* DeathParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death")
		float DeadbodyLifeSpan = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
		class UParticleSystem* CoinParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
		class UParticleSystem* RubyParticle;


	FTimerHandle DeadbodyLifeSpanTimer;

public: // Upgrade
	UFUNCTION(BlueprintCallable)
		void OnUpgrade();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Upgrade")
		class UParticleSystem* UpgradeParticle;


};
