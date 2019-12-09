// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneComponents/SpawnerComponent.h"
#include "KRVEnums.h"
#include "Interfaces/KRVDataAssetInterface.h"
#include "Structs/KRVAttackStat.h"
#include "KRVShooterComponent.generated.h"

class UAudioComponent;
class USoundCue;
class AKRVProjectile;
class UKRVShooterComponentDataAsset;

UCLASS()
class KRV_API UKRVShooterComponent : public USpawnerComponent, public IKRVDataAssetInterface
{
	GENERATED_BODY()
public:
	UKRVShooterComponent();

	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public: // DataAsset
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		class UKRVDataAsset* GetDataAsset() const;
	UKRVDataAsset* GetDataAsset_Implementation() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool LoadFromDataAsset();
	virtual bool LoadFromDataAsset_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Asset")
		UKRVShooterComponentDataAsset* GetShooterComponentDataAsset() const;

	UFUNCTION(BlueprintCallable, Category = "Asset")
		bool SetDataAssetAndLoad(UKRVShooterComponentDataAsset* DataAsset);

protected:
	UPROPERTY()
		UKRVDataAsset* HardPtr_MyDataAsset;

	bool bIsDataAssetLoaded;

public: // Shooter
	UFUNCTION(BlueprintCallable, Category = "Target")
	bool SetShooter(AKRVCombatUnit* NewShooter);

public:
	UPROPERTY(BlueprintReadWrite, Category = "Shooter")
		TWeakObjectPtr<AKRVCombatUnit> Shooter;

public: // Target
	UFUNCTION(BlueprintCallable, Category = "Target")
		AKRVCombatUnit* GetTargetUnit() const;

	UFUNCTION(BlueprintCallable, Category = "Target")
		void SetTargetUnit(AKRVCombatUnit* NewTarget);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Target")
		TWeakObjectPtr<AKRVCombatUnit> FlyTarget;


public: // fire
	UFUNCTION(BlueprintCallable, Category = "Fire")
		AKRVProjectile* Fire(const FKRVAttackStat& RangedAttackStat, const FKRVAttackStat& SplashAttackStat);	
	
	UFUNCTION(BlueprintCallable, Category = "Fire")
		AKRVProjectile* SpawnMine(const FVector& SpawnLoc, ETargetType TargetTypeToResponse, const FKRVAttackStat& NewSplashAttackStat);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fire")
		UParticleSystemComponent* FireParticleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fire")
		UAudioComponent* FireAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fire")
		FTransform RelativeFireTransform;

public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void SetProjectileFlyParams(float _InitSpeed, float _MaxSpeed, float _Accel, float _Gravity = 1.f);

};
