// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pawns/KRVCombatUnit.h"
#include "GameplayTagContainer.h"
#include "KRVTower.generated.h"

class USoundCue;
class UAudioCmoponent;
class UKRVStatComponent;
class UKRVTowerStatComponent;
class UKRVTowerDataAsset;

DECLARE_DELEGATE_OneParam(FIncreaseSellingCostDelegate, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRecrutiedDelegate, class AKRVCharacter*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpgradeProgressChangedDelegate, float, float);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpgradeBeginDelegate, class UKRVTowerUpgradeDataAsset*, AKRVTower*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpgradeFinishedDelegate, class UKRVTowerUpgradeDataAsset*, AKRVTower*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSellingBeginDelegate, AKRVTower*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSellingFinishedDelegate, AKRVTower*);
DECLARE_DELEGATE_RetVal_TwoParams(bool, FBuildTowerDelegate, TSubclassOf<AKRVTower>, float);

UCLASS()
class KRV_API AKRVTower : public AKRVCombatUnit
{
	GENERATED_BODY()

public:
	AKRVTower();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

public: // DataAsset
	UFUNCTION(BlueprintCallable)
		UKRVTowerDataAsset* GetTowerDataAsset() const;

	virtual bool LoadFromDataAsset_Implementation() override;
	virtual bool LoadBaseStat() override;

protected:
	void OnPlatformMeshAsyncLoaded();
	void OnTowerMenuWidgetClassAsyncLoaded();

protected:
	FSoftObjectPath PlatformMeshPath;
	FSoftClassPath TowerMenuWidgetClassPath = "/Game/Resources/UI/UI_TowerMenu.UI_TowerMenu_C";
	FName TopSocket = "TopSocket";
	FName RootSocket = "RootSocket";

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Body")
		class UStaticMeshComponent* PlatformMeshComponent;

public: // Spawn
	virtual void OnBeginPlayAndSpawned() override;
	virtual void OnSpawned() override;
	virtual void DestroyThis() override;

	UFUNCTION(BlueprintCallable)
		class AKRVBuildingSite* GetBuildingSite() const;

	UFUNCTION(BlueprintCallable)
		void SetBuildingSite(class AKRVBuildingSite* NewBuildingSite);

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "BuildingSite")
		bool bIsDummyTower = false;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "BuildingSite")
		TWeakObjectPtr<class AKRVBuildingSite> BuildingSite;

	virtual bool PlayBrainComponent(bool bIsTrue) override;

public: // UI
	virtual void BindToWidgetsOnBeginPlay() override;
	virtual void BindToWidgetsBySpawner() override;
	virtual void OnSelected(bool bIsSelected);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class UWidgetComponent* TowerMenuWidget;

public: // GameState
	virtual void OnGameEnd(EKRVTeam Winner) override;

public: // Stat
	virtual UKRVStatComponent* VariateBaseStat(TSubclassOf<AKRVBuff> Variation) override;
	virtual UKRVStatComponent* GetBaseStat() const override;
	virtual UKRVStatComponent* GetModifiedStat() const override;

	UFUNCTION(BlueprintCallable)
		UKRVTowerStatComponent* GetBaseTowerStat() const;

	UFUNCTION(BlueprintCallable)
		UKRVTowerStatComponent* GetModifiedTowerStat() const;
protected:
	virtual void UpdateStat() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
		UKRVTowerStatComponent* BaseTowerStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
		UKRVTowerStatComponent* ModifiedTowerStat;

public: // Status
	virtual bool MakeStunned(float NewDuration) override;
	virtual bool StopAction() override;

protected:
	virtual void OnStunTimeUp() override;

public: // Action
	virtual bool Die() override;

protected:
	// Set the body mesh hide and keep showing UserWidget. When DeadBodyTimer times up, it will be destroyed.
	// 바디 메쉬를 숨기고 유저위젯을 남김. 데드바디 타이머 시간이 다되면 타워가 디스트로이됨.
	void HideBodyMesh();

protected:
	// It Should be smaller than DeadbodyLifeSpan.
	// DeadbodyLifeSpan 보다 작아야함.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float WasteBuildingLifeSpan = 2.f;

	FTimerHandle HideMeshTimer;

public: // Recruit
	UFUNCTION(BlueprintCallable)
		int32 GetCurrentRecruits() const;

	UFUNCTION(BlueprintCallable)
		bool IsAbleToRecruit() const;

	UFUNCTION(BlueprintCallable)
		bool IsRecruitableTower() const;

	UFUNCTION(BlueprintCallable)
		void GetPointOnNearestPath(FVector& OutVec) const;

protected:
	UFUNCTION()
		void OnRecruitDie(class AKRVCombatUnit* DeadCharacter);

	UFUNCTION()
		void OnRecruitComplete();

	bool Recruit();

public:
	FOnRecrutiedDelegate OnRecrutiedDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USpawnerComponent* SpawnerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 PoolMargin = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<class AKRVCharacter*> RecruitsInPool;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<class AKRVCharacter*> RecruitsInField;

	FTimerHandle RecruitRateTimer;

public: // Waypoint
	UFUNCTION(BlueprintCallable)
		FVector GetRecruitWaypoint() const;

	UFUNCTION(BlueprintCallable)
		bool SetRecruitWaypoint(const FVector& NewLocation);

	UFUNCTION(BlueprintCallable)
		bool MakeRecruitsReturnToTheWaypoint();

protected:
	UFUNCTION(BlueprintCallable)
		bool MoveRecruits(ECombatStatus NewStatus);

	UPROPERTY(Transient, BlueprintReadOnly)
		FVector RecruitWaypoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UParticleSystemComponent* WaypointParticleComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stat")
		class UDecalComponent* WaypointRadiusDecal;

	UFUNCTION(BlueprintCallable)
		void ShowWaypointRadiusDecal(bool bDoesShow);

public: // Buy, Sell, Upgrade
	UFUNCTION(BlueprintCallable)
		class UKRVTowerUpgradeDataAsset* GetTowerUpgradeDA(int32 Index) const;

	UFUNCTION(BlueprintCallable)
		int32 GetIndexByTowerUpgradeDA(class UKRVTowerUpgradeDataAsset* DAToCheck);

	UFUNCTION(BlueprintCallable)
		int32 GetTowerUpgradeLevel(int32 Index) const;

	UFUNCTION(BlueprintCallable)
		int32 GetActualTowerUpgradeCost(int32 NewDAIndex) const;

	UFUNCTION(BlueprintCallable)
		int32 GetSellingCost() const;

	UFUNCTION(BlueprintCallable)
		bool UpgradeTower(int32 Index);

	void OnNewTowerBeginBuilt();

	UFUNCTION(BlueprintCallable)
		bool IsAbleToUpgradeTower(int32 Index);

	UFUNCTION(BlueprintCallable)
		void AcquireTowerUpgrade(class UKRVTowerUpgradeDataAsset* NewTowerUpgradeDA);

	UFUNCTION(BlueprintCallable)
		bool IsSellable() const;

	UFUNCTION(BlueprintCallable)
		float GetSellingTime() const;

	UFUNCTION(BlueprintCallable)
		void SellThis();

	UFUNCTION(BlueprintCallable)
		float GetRemainingUpgradeBuildTime();

protected:
	UFUNCTION()
		void OnUpgradeFinished();

	UFUNCTION()
		void OnSellingFinished();

public:
	FOnSellingBeginDelegate OnSellingBeginDelegate;
	FOnSellingFinishedDelegate OnSellingFinishedDelegate;
	FOnUpgradeBeginDelegate OnUpgradeBeginDelegate;
	FOnUpgradeFinishedDelegate OnUpgradeFinishedDelegate;
	FOnUpgradeProgressChangedDelegate OnUpgradeProgressChangedDelegate;
	FBuildTowerDelegate BuildTowerDelegate;
	FIncreaseSellingCostDelegate IncreaseSellingCostDelegate;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TowerSellingTime = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FGameplayTag, float> TowerUpgradeDiscounts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<class UKRVTowerUpgradeDataAsset*> TowerUpgradeDAs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<int32> TowerUpgradeLevels;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		int32 CurrentUpgradeIndex;

	FTimerHandle TowerUpgradeTimer;

};
