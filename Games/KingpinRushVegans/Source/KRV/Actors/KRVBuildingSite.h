// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KRVEnums.h"
#include "KRVBuildingSite.generated.h"

UCLASS()
class KRV_API AKRVBuildingSite : public AActor
{
	GENERATED_BODY()

public:
	AKRVBuildingSite();


public:
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

protected: // DataLoad
	FString UpgradePath = FString(TEXT("/Game/Resources/DataAssets/TowerUpgrades/"));
	FString UpgradePrefix = FString(TEXT("DA_U_T_B_"));
	FString UpgradePostfix = FString(TEXT(""));

protected: // Body
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UStaticMeshComponent* SiteBaseMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* FlagMeshComponent;

protected:
	UPROPERTY()
		TWeakObjectPtr<class AKRVTower> TowerInSite;


public: // Position
	void GetPointOnNearestPath(FVector& OutVec) const;

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
		FVector PointOnNearestPath;


public: // Upgrades
	void InitTowerSet(class AKRVTower* NewTower);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Upgrade")
		class AKRVTower* DummyTower;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
		TArray<class UKRVTowerUpgradeDataAsset*> UpgradeDataAssets;

	UPROPERTY()
		class AKRVTower* Ptr_TemporaryTower;

public: // Build Tower
	UFUNCTION(BlueprintCallable)
		bool BuildTower(TSubclassOf<class AKRVTower> NewTower, float BuildTime);

protected:
	UFUNCTION()
		void OnTowerBuildFinished();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* UnderConstructionMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UParticleSystemComponent* OnBuildStartParticleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UAudioComponent* OnBuildStartAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TowerBuildFinishDelay = 0.3f;

	FTimerHandle TowerBuildingTimer;

public: // Sell tower
	UFUNCTION(BlueprintCallable)
		void SellTower(class AKRVTower* TowerToSell);

	UFUNCTION(BlueprintCallable)
		int32 GetSellingCost() const;

	UFUNCTION()
		void IncreaseSellingCost(int32 CostToIncrease);

protected:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Sell")
		int32 SellingCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sell")
		float SellingCostScale = 0.75;

};
