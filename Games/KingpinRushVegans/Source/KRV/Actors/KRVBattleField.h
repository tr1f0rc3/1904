// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/KRVLevelSaveData.h"
#include "Interfaces/KRVDataAssetInterface.h"
#include "KRVBattlefield.generated.h"

DECLARE_DELEGATE_TwoParams(FOnBattlefieldIconClickedDelegate, class UKRVLevelDataAsset*, const FKRVLevelSaveData&);

UCLASS()
class KRV_API AKRVBattlefield : public AActor, public IKRVDataAssetInterface
{
	GENERATED_BODY()
	
public:	
	AKRVBattlefield();
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

public: // Asset
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		class UKRVDataAsset* GetDataAsset() const;
	virtual class UKRVDataAsset* GetDataAsset_Implementation() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool LoadFromDataAsset();
	virtual bool LoadFromDataAsset_Implementation() override;

	UFUNCTION(BlueprintCallable)
		class UKRVLevelDataAsset* GetLevelDataAsset() const;

protected:
	bool GetKRVLevelSaveData(FKRVLevelSaveData& OutSaveData, UKRVLevelDataAsset* NewLevelDA);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVLevelDataAsset* LevelDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSoftObjectPtr<class UKRVLevelDataAsset> SoftPtr_LevelDataAsset;

protected: // Body
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UBoxComponent* BoxComponent;

public: // Icon widget
	bool ShowIconWidget(bool bIsShown);

protected:
	UFUNCTION()
		void OnIconClicked();

	bool BindToWidget();

public:
	FOnBattlefieldIconClickedDelegate OnBattlefieldIconClickedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class UWidgetComponent* BattlefieldWidgetComponent;

};
