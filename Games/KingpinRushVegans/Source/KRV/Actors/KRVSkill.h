// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "KRVEnums.h"
#include "Structs/KRVCharacterSpawnData.h"
#include "Interfaces/KRVSkillInterface.h"
#include "Interfaces/KRVDataAssetInterface.h"
#include "KRVSkill.generated.h"

class UKRVDataAsset;
class UKRVShooterComponentDataAsset;
class UKRVShooterComponent;
class USpawnerComponent;
class AKRVCombatUnit;
class AKRVCharacter;
class UPooledComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCastSkillToSelfDelegate, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCastSkillToTargetDelegate, AKRVCombatUnit*, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCastSkillToLocationDelegate, const FVector&, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCooldownNotificationDelegate, float, float);

UCLASS()
class KRV_API AKRVSkill : public AActor, public IKRVSkillInterface, public IKRVDataAssetInterface
{
	GENERATED_BODY()

public:
	AKRVSkill();

public:
	virtual void PostInitializeComponents() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		class UKRVDataAsset* GetDataAsset() const;
	UKRVDataAsset* GetDataAsset_Implementation() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool LoadFromDataAsset();
	virtual bool LoadFromDataAsset_Implementation() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Load")
		TSoftObjectPtr<UKRVDataAsset> MyDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Load")
		TArray<TSoftObjectPtr<UKRVShooterComponentDataAsset>> MyShooterComponentDataAsset;

	UPROPERTY()
		UKRVDataAsset* HardPtr_MyDataAsset;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Load")
		TArray<UKRVShooterComponentDataAsset*> HardPtr_ShooterComponentDataAsset;

	bool bIsDataLoaded;
	FName RightHandSocket = "RightHand";

protected:
	UPooledComponent* PooledComponent;

public: // General Info
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool GetDescriptionWithVaraibles(FText& OutText);
	bool GetDescriptionWithVaraibles_Implementation(FText& OutText);

public: // Range && CastingTIme
	UFUNCTION(BlueprintCallable)
		float GetCastingTime() const;

	UFUNCTION(BlueprintCallable)
		ETargetingType GetTargetingType() const;

	UFUNCTION(BlueprintCallable)
		ETargetType GetTargetType() const;

	UFUNCTION(BlueprintCallable)
		float GetSkillRange() const;

	UFUNCTION(BlueprintCallable)
		float GetSkillRadius() const;

	UFUNCTION(BlueprintCallable)
		bool IsAutoCastable() const;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target")
		ETargetingType WhereToCastSkill = ETargetingType::NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target")
		ETargetType TargetType = ETargetType::HostileCharacter;

	// may designer override this value
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Casting")
		float CastingTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
		bool bIsAutoCasting = true;

	// not for self casting
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Casting")
		float SkillRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Casting")
		float SkillRadius = 450.f;

public: // Mastery Level
	UFUNCTION(BlueprintCallable)
		int32 GetMasteryLevel() const;

	UFUNCTION(BlueprintCallable)
		int32 GetMaxMasteryLevel() const;

	UFUNCTION(BlueprintCallable)
		void SetMasteryLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable)
		bool MasteryLevelUp(int32 LevelToIncrease = 1);

protected:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void SetValuesByLevel();

	UFUNCTION(BlueprintCallable)
		float GetValueByLevel_Float(int32 NewLevel, const TArray<float>& NewFloats);

	UFUNCTION(BlueprintCallable)
		int32 GetValueByLevel_Int(int32 NewLevel, const TArray<int32>& NewInts);

protected:
	// 0 means unlocked skill;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mastery")
		int32 MasteryLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mastery")
		int32 MaxMasteryLevel = 1;

public: // Cast
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill")
		bool CastToSelf(AKRVCombatUnit* Caster);
	virtual bool CastToSelf_Implementation(AKRVCombatUnit* Caster);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill")
		bool CastToTarget(AKRVCombatUnit* Target, AKRVCombatUnit* Caster);
	virtual bool CastToTarget_Implementation(AKRVCombatUnit* Target, AKRVCombatUnit* Caster);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill")
		bool CastToLocation(const FVector& Location, AKRVCombatUnit* Caster);
	virtual bool CastToLocation_Implementation(const FVector& Location, AKRVCombatUnit* Caster);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill")
		bool IsAbleToCastSkill(AKRVCombatUnit* NewCaster = nullptr) const;
	virtual bool IsAbleToCastSkill_Implementation(AKRVCombatUnit* NewCaster = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill")
		bool IsInSkillRange_Target(const AKRVCombatUnit* Target, const AKRVCombatUnit* Caster) const;
	virtual bool IsInSkillRange_Target_Implementation(const AKRVCombatUnit* Target, const AKRVCombatUnit* Caster) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill")
		bool IsInSkillRange_Location(const FVector& Location, const AKRVCombatUnit* Caster) const;
	virtual bool IsInSkillRange_Location_Implementation(const FVector& Location, const AKRVCombatUnit* Caster) const;

	// bind passive skill to CombatUnit's act. ex) on hit skills
	// 패시브 스킬을 유닛의 특정 행동에 바인드함.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void BindPassiveSkill(class AKRVCombatUnit* UnitToBind);

protected:
	virtual void OnCastSkill_General(AKRVCombatUnit* NewSkillCaster);

public:
	FOnCastSkillToSelfDelegate OnCastSkillToSelfDelegate;
	FOnCastSkillToTargetDelegate OnCastSkillToTargetDelegate;
	FOnCastSkillToLocationDelegate OnCastSkillToLocationDelegate;
	FDelegateHandle PassiveDelegateHandle;

protected:
	// Create this when use projectile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced, Category = "Fire")
		TArray<UKRVShooterComponent*> ShooterComponents;

protected:
	// Create this when use summon spell
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced)
		TArray<USpawnerComponent*> SpawnerComponents;

	// Num() of summon class and summon amount should be same.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Summon")
		TArray<FKRVCharacterSpawnData> CharacterSpawnData;

	enum { MaxBuffs = 12, };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buff")
		TArray<TSubclassOf<class AKRVBuff>> BuffClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buff")
		int32 MaxBuffsInField = 4;

public: // Cooldown
	UFUNCTION(BlueprintCallable)
		bool IsSkillReady() const;

	UFUNCTION(BlueprintCallable)
		float GetRemainingCooldown();

protected:
	void OnCooldownTimeUp();

public:
	FOnCooldownNotificationDelegate OnCooldownNotificationDelegate;

protected:
	// may designer override this value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown")
		float SkillCooldown = 1.f;

	bool bIsInCooldown;
	FTimerHandle CooldownTimer;


protected: // Internal fcns
	UFUNCTION(BlueprintCallable)
		class AKRVCombatUnit* GetKRVOwner() const;

	UFUNCTION(BlueprintCallable)
		bool SummonCharacter(int32 SpawnerCompIndex, int32 NewCharactersToSpawn, float NewLifeSpawn, AKRVCombatUnit* Caster);

	UFUNCTION(BlueprintCallable)
		bool GiveBuff(TArray<AKRVCombatUnit*> TargetUnits, int32 SpawnerCompIndex, AKRVCombatUnit* Caster);

	bool IsValidSpawnerComponentIndex(int32 Index);

};
