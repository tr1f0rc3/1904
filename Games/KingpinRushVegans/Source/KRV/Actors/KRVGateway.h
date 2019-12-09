// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/KRVDataAssetInterface.h"
#include "Structs/KRVCharacterSpawnData.h"
#include "KRVEnums.h"
#include "KRVGateway.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterSpawned, AKRVCharacter*);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnNextWaveIncomingWidgetDelegate, float, CallTimeLimit);
DECLARE_DELEGATE_TwoParams(FOnSpawnInTheWaveFinishedDelegate, float, AKRVGateway*);
DECLARE_DELEGATE_OneParam(FOnAllSpawnFinishedDelegate, AKRVGateway*);
DECLARE_DYNAMIC_DELEGATE(FOnNextWaveCalledWidgetDelegate);
DECLARE_MULTICAST_DELEGATE(FRequestNextWaveCallDelegate);
DECLARE_DELEGATE_OneParam(FRequestToShowNextWavePathsDelegate, bool);

UCLASS()
class KRV_API AKRVGateway : public AActor, public IKRVDataAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKRVGateway();

public:
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

public: // Data Asset
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		class UKRVDataAsset* GetDataAsset() const;
	UKRVDataAsset* GetDataAsset_Implementation() const override;

	UFUNCTION(BlueprintCallable)
		class UKRVGatewayDataAsset* GetGatewayDataAsset() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool LoadFromDataAsset();
	virtual bool LoadFromDataAsset_Implementation() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSoftObjectPtr<UKRVGatewayDataAsset> SoftPtr_GateWayDataAsset;

	UPROPERTY()
		class UKRVGatewayDataAsset* HardPtr_GateWayDataAsset;

protected:
	UFUNCTION()
		void OnGameEnd(EKRVTeam Winner);

protected: // Body
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UBoxComponent* BoxComponent;

public: // UI && Tooltip
	void RequestToShowNextWavePaths(bool bIsTrue);
	bool GetNextWaveTooltipText(FText& OutText);
	bool IsFlyingUnitIncomingInNextWave() const;

protected:
	void ShowNextWavePaths(bool bIstrue);
	bool BindToWidget();
	bool GetNextWaveTooltip_Internal(FString& OutString, const FKRVCharacterSpawnData& CharacterSpawnDataToGet);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class UWidgetComponent* IncomingWidget;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "UI")
		bool bIsFlyingUnitIncomingInNextWave;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "UI")
		bool bIsPathsShown;

public: // Incoming
	UFUNCTION(BlueprintNativeEvent)
		void OnNextWaveIncoming(float CallTimeLimit = -1.f);
	void OnNextWaveIncoming_Implementation(float CallTimeLimit = -1.f);

	UFUNCTION(BlueprintNativeEvent)
		void OnNextWaveCalled();
	void OnNextWaveCalled_Implementation();


	UFUNCTION(BlueprintCallable)
		int32 GetTotalWaves() const;

	void RequestNextWaveCall();

public:
	FOnNextWaveCalledWidgetDelegate OnNextWaveCalledWidgetDelegate;
	FOnNextWaveIncomingWidgetDelegate OnNextWaveIncomingWidgetDelegate;
	FRequestNextWaveCallDelegate RequestNextWaveCallDelegate;
	FRequestToShowNextWavePathsDelegate RequestToShowNextWavePathsDelegate;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Wave")
		int32 TotalWaves;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
		class UParticleSystemComponent* IncomingParticleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
		class UAudioComponent* NextWaveCallAudioComponent;

public: // Spawn
	UFUNCTION(BlueprintCallable)
		void SpawnRecursive();

	UFUNCTION(BlueprintCallable)
		void SpawnCharacter(const FKRVCharacterSpawnData& CharacterSpawnData);

public:
	FOnCharacterSpawned OnCharacterSpawned;
	FOnSpawnInTheWaveFinishedDelegate OnSpawnInTheWaveFinishedDelegate;
	FOnAllSpawnFinishedDelegate OnAllSpawnFinishedDelegate;

protected:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Spawn")
		int32 CurrentSpawnIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced, Category = "Spawn")
		TArray<class USpawnerComponent*> SpawnerComponents;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
		TMap<TSubclassOf<class AKRVCharacter>, int32> SpawnerComponentMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
		class UParticleSystemComponent* SpawnParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float DelayToBeginSpawn = 2.f;

	FTimerHandle CharacterSpawnTimer;

public:
	UFUNCTION(BlueprintCallable)
		class AKRVPath* GetPath(int32 Index);

protected:
	enum { MaxPaths = 6, };
	UPROPERTY(VisibleAnywhere, Category = "Path")
		TArray<TWeakObjectPtr<class AKRVPath>> Pathways;
};
