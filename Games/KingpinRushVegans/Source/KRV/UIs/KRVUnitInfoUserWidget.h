// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVEnums.h"
#include "KRVUnitInfoUserWidget.generated.h"

UCLASS()
class KRV_API UKRVUnitInfoUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

protected:
	void OnTrueDamageTextureAsyncLoaded();
	void OnPhysicalDamageTextureAsyncLoaded();
	void OnMagicDamageTextureAsyncLoaded();

protected:
	FSoftObjectPath TrueDamageTexturePath = "/Game/Resources/Textures/T_Water.T_Water";
	FSoftObjectPath PhysicalDamageTexturePath = "/Game/Resources/Textures/T_Sword.T_Sword";
	FSoftObjectPath MagicDamageTexturePath = "/Game/Resources/Textures/T_MagicWand.T_MagicWand";

protected:
	UFUNCTION(BlueprintNativeEvent)
		void ShowThisWidget();
	void ShowThisWidget_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void HideThisWidget();
	void HideThisWidget_Implementation();

public:
	bool BindToPC(class AKRVInGamePlayerController* NewPC);

protected:
	UFUNCTION()
		void OnUnitSelected(bool bIsSelected, class AKRVCombatUnit* SelectedUnit);

protected: // UnitName
	bool SetUnitName(const FText& NewUnitName);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* UnitNameTextBlock;

protected: // Damage
	bool SetDamage(EDamageReductionType NewDamageReductionType, int32 MinDamage, int32 MaxDamage);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UHorizontalBox* DamageBox;

	UPROPERTY(BlueprintReadOnly)
		class UImage* DamageTypeImage;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* MagicDamageTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* PhysicalDamageTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* TrueDamageTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* DamageTextBlock;

protected: // Range
	bool SetRange(int32 NewRange);
	bool SetRangeToMelee();

protected:
	UPROPERTY(BlueprintReadOnly)
		class UHorizontalBox* RangeBox;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* RangeTextBlock;

	FString MeleeString = FString(TEXT("Melee"));

protected: // Armor
	bool SetArmors(int32 NewArmor, int32 NewMR);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UVerticalBox* ArmorsBox;

	UPROPERTY(BlueprintReadOnly)
		class UProgressBar* ArmorProgressBar;

	UPROPERTY(BlueprintReadOnly)
		class UProgressBar* MRProgressBar;

};
