// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"

#include "KRVEnums.h"
#include "Interfaces/KRVDataAssetInterface.h"
#include "Structs/KRVItemStockData.h"
#include "Structs/KRVHeroSaveData.h"
#include "Structs/KRVTowerSaveData.h"
#include "Structs/KRVLevelSaveData.h"
#include "Structs/KRVSkillMasterySaveData.h"
#include "KRVIconUserWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnIconHoveredDelegate);
DECLARE_MULTICAST_DELEGATE(FOnIconUnhoveredDelegate);
DECLARE_MULTICAST_DELEGATE(FOnIconPressedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnIconReleasedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnIconClickedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnIconDoubleClickedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnProgressFinishedDelegate);

DECLARE_MULTICAST_DELEGATE(FOnTowerUpgradeConfirmedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnTowerSellingConfirmedDelegate);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHeroInMapClickedDelegate, const FName&);

UENUM(BlueprintType)
enum class EIconTextureVariation : uint8
{
	NONE,
	Default,
	Locked,
	DataAsset,
	Custom,
	Confirm,
	Cancle,
	Restore,
	Achievement,
	QuestionMark,
	Document,
	Leaderboard,
	Menu,
	Star,
	Setting,
	RightArrow,
	ShutDown,
	LogOut,
	Sound,
	MAX,

};

UENUM(BlueprintType)
enum class EIconInteraction : uint8
{
	NONE,
	Normal,
	Pressed,
	Hovered,
	MAX,

};

UENUM(BlueprintType)
enum class EIconStatus : uint8
{
	NONE,
	NoInteraction,
	Normal,
	Disabled,
	OnlyHovered,
	DisabledAndNotHovered,
	MAX,

};


UCLASS()
class KRV_API UKRVIconUserWidget : public UUserWidget, public IKRVDataAssetInterface
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void InitializeThisWidget();

protected:
	bool bIsThisWidgetInitialized = false;

public: // DataAsset
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		class UKRVDataAsset* GetDataAsset() const;
	virtual class UKRVDataAsset* GetDataAsset_Implementation() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool LoadFromDataAsset();
	virtual bool LoadFromDataAsset_Implementation() override;

	bool BindToDataAsset(class UKRVDataAsset* NewDA, bool bDoesBindToTooltip = true);

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		class UKRVDataAsset* BoundDataAsset;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* DAIconTexture;

	FSoftObjectPath DAIconTexturePath;

protected:
	void IconTooltipWidgetClassAsyncLoaded();
	void OnIconTextureAsyncLoaded();

	void LockedTextureAsyncLoaded();
	void ConfirmTextureAsyncLoaded();
	void CancleTextureAsyncLoaded();
	void RestoreTextureAsyncLoaded();
	void AchievementTextureAsyncLoaded();
	void QuestionMarkTextureAsyncLoaded();
	void DocumentTextureAsyncLoaded();
	void LeaderboardTextureAsyncLoaded();
	void MenuTextureAsyncLoaded();
	void StarTextureAsyncLoaded();
	void SettingTextureAsyncLoaded();
	void RightArrowTextureAsyncLoaded();
	void ShutDownTextureAsyncLoaded();
	void LogOutTextureAsyncLoaded();
	void SoundTextureAsyncLoaded();

protected:
	FSoftClassPath IconTooltipWidgetClassPath = "/Game/Resources/UI/UI_IconTooltip.UI_IconTooltip_C";
	FSoftObjectPath LockedTexturePath = "/Game/UM3/Textures/GUI/Test/Symbol_Locked.Symbol_Locked";

	//FSoftObjectPath OnPressedSoundPath = "/Game/Retro_8Bit_Sounds/Menu_UI_Beeps/Cues/retro_ui_menu_simple_click_01_Cue.retro_ui_menu_simple_click_01_Cue";
	//FSoftObjectPath RejectedSoundPath = "/Game/Retro_8Bit_Sounds/Alarms_Sirens/Cues/retro_alarm_siren_loop_20_Cue.retro_alarm_siren_loop_20_Cue";
	//FSoftObjectPath UpgradeSoundPath = "/Game/Retro_8Bit_Sounds/Powerup/Cues/retro_powerup_collect_29_Cue.retro_powerup_collect_29_Cue";
	//FSoftObjectPath WaveCallSoundPath = "/Game/Retro_8Bit_Sounds/Miscellaneous/Cues/retro_misc_various_sounds_87_Cue.retro_misc_various_sounds_87_Cue";

	FSoftObjectPath ConfirmTexturePath = "/Game/UM3/Textures/GUI/Test/Symbole_Check.Symbole_Check";
	FSoftObjectPath CancleTexturePath = "/Game/UM3/Textures/GUI/Test/Button_Cancel.Button_Cancel";
	FSoftObjectPath RestoreTexturePath = "/Game/UM3/Textures/GUI/Test/SymbolReplay.SymbolReplay";
	FSoftObjectPath AchievementTexturePath = "/Game/UM3/Textures/GUI/Test/SymbolAchievements.SymbolAchievements";
	FSoftObjectPath QuestionMarkTexturePath = "/Game/UM3/Textures/GUI/Test/SymbolCredits.SymbolCredits";
	FSoftObjectPath DocumentTexturePath = "/Game/UM3/Textures/GUI/Test/SymbolDocumentation.SymbolDocumentation";
	FSoftObjectPath LeaderboardTexturePath = "/Game/UM3/Textures/GUI/Test/SymbolLeaderboards.SymbolLeaderboards";
	FSoftObjectPath MenuTexturePath = "/Game/UM3/Textures/GUI/Test/SymbolMenu.SymbolMenu";
	FSoftObjectPath StarTexturePath = "/Game/UM3/Textures/GUI/Test/SymbolRateUs.SymbolRateUs";
	FSoftObjectPath SettingTexturePath = "/Game/UM3/Textures/GUI/Test/SymbolSettings.SymbolSettings";
	FSoftObjectPath RightArrowTexturePath = "/Game/UM3/Textures/GUI/Test/Symbol_Arrow.Symbol_Arrow";
	FSoftObjectPath ShutDownTexturePath = "/Game/Resources/Textures/T_ShutDown.T_ShutDown";
	FSoftObjectPath LogOutTexturePath = "/Game/Resources/Textures/T_LogOut.T_LogOut";
	FSoftObjectPath SoundTexturePath = "/Game/Resources/Textures/T_Speaker_Mono.T_Speaker_Mono";

public: // Icon
	UFUNCTION(BlueprintCallable)
		bool SetCustomIconTexture(UTexture2D* NewTexture);

protected:
	UFUNCTION(BlueprintCallable)
		bool SetIconTextureVariation(EIconTextureVariation NewVariation, bool bDoesForceToSet = false);

	UFUNCTION(BlueprintCallable)
		bool SetIconType(EIconType NewIconType);

	UFUNCTION(BlueprintCallable)
		void SetIconStatus(EIconStatus NewStatus);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		EIconType OnProgressFinishedEffect(EIconType NewIconType = EIconType::NONE);
	EIconType OnProgressFinishedEffect_Implementation(EIconType NewIconType = EIconType::NONE);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void UnavailbleEffect();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool OnHighlightedEffect(bool bIsActive);
	bool OnHighlightedEffect_Implementation(bool bIsActive);

	UFUNCTION()
		void OnRemainingProgressNotified(float NewRemainingProgress, float NewTotalProgress);

	UFUNCTION()
		void OnProgressFinished();

	class UMaterialInstanceDynamic* GetIconMIDynamic(EIconInteraction NewIconInteraction);
	bool SetMIProgressRatio(float CurrentValue, float MaxValue, UMaterialInstanceDynamic* NewMIDynamic = nullptr);
	bool GetWorldSpaceOfWidget(FTransform& OutTransform);

public:
	FOnProgressFinishedDelegate OnProgressFinishedDelegate;

protected:
	UPROPERTY(BlueprintReadOnly)
		EIconType IconType = EIconType::NONE;

	UPROPERTY(Transient, BlueprintReadOnly)
		EIconStatus IconStatus;

	UPROPERTY(BlueprintReadOnly)
		EIconTextureVariation IconTextureVariation = EIconTextureVariation::Locked;

	UPROPERTY(BlueprintReadOnly)
		class UButton* IconButton;

	UPROPERTY(BlueprintReadOnly)
		class UBorder* IconBorder;

	UPROPERTY(BlueprintReadOnly)
		class UBorder* IntBorder;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* IntTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class USizeBox* ExclamationSizeBox;

	UPROPERTY(Transient, BlueprintReadOnly)
		bool bIsProgressing;

	UPROPERTY(Transient, BlueprintReadOnly)
		bool bIsHighlighted;

	UPROPERTY(Transient, BlueprintReadOnly)
		float CurrentRemainingProgress;

	UPROPERTY(Transient, BlueprintReadOnly)
		float TotalProgress;

	FName IconButtonName = "IconButton";
	FName IconBorderName = "IconBorder";

	FName RadialGradientDensityName = "RadialGradientDensity";
	FName IconTextureName = "IconTexture";
	FName ProgressRatioName = "ProgressRatio";

protected: // Icon Textures
	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* DefaultTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* CustomTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* LockedTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* ConfirmTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* CancleTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* RestoreTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* AchievementTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* QuestionMarkTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* DocumentTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* LeaderboardTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* MenuTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* StarTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* SettingTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* RightArrowTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* ShutDownTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* LogOutTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* SoundTexture;

protected: // Tooltip
	UFUNCTION()
		class UWidget* CreateTooltipWidgetAndBindToButton();

	bool BindTooltipByIconType(bool bIsCalledByAsyncLoad = false);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVIconTooltipUserWidget* IconTooltipWidget;

	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<class UKRVIconTooltipUserWidget> IconTooltipWidgetClass;

public: // Input
	UFUNCTION(BlueprintCallable)
		void ResetConsecutiveClicks();

protected:
	UFUNCTION()
		void OnIconHovered();

	UFUNCTION()
		void OnIconUnhovered();

	UFUNCTION()
		void OnIconPressed();

	UFUNCTION()
		void OnIconReleased();

	UFUNCTION()
		virtual void OnIconClicked();

	UFUNCTION(BlueprintCallable)
		bool SetBorderScale(EIconInteraction NewInteraction);

	UFUNCTION()
		void AfterClicked();

public:
	FOnIconHoveredDelegate OnIconHoveredDelegate;
	FOnIconUnhoveredDelegate OnIconUnhoveredDelegate;
	FOnIconPressedDelegate OnIconPressedDelegate;
	FOnIconReleasedDelegate OnIconReleasedDelegate;
	FOnIconClickedDelegate OnIconClickedDelegate;
	FOnIconDoubleClickedDelegate OnIconDoubleClickedDelegate;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		bool bIsHoverable;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		bool bIsClickable;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		EIconInteraction IconInteraction = EIconInteraction::Normal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float IconBorderScale_Normal = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float IconBorderScale_Hovered = 1.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float IconBorderScale_Pressed = 0.95f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MIRadialGradientDensity_NoInteraction = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MIRadialGradientDensity_Normal = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MIRadialGradientDensity_Hovered = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MIRadialGradientDensity_Pressed = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MIRadialGradientDensity_Disabled = 1.0f;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		int32 ConsecutiveClicks;

	FTimerHandle AfterClickedTimer;

public: // binding;
	bool IsBound() const;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		bool bIsBound;

public: // Skills
	bool BindToSkill(const FGameplayTag& SkillTagToBind, class AKRVCombatUnit* CasterToBind);
	bool BindToItem(const FGameplayTag& ItemTagToBind, AKRVCombatUnit* CasterToBind);

protected:
	UFUNCTION()
		void OnSkillAcquired(class AKRVSkill* NewSkill, class AKRVCombatUnit* NewCaster);

	UFUNCTION()
		void OnItemStockChanged(int32 NewStock, class AKRVItem* NewItem);

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class AKRVCombatUnit* BoundCaster;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class AKRVSkill* BoundSkill;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		FGameplayTag BoundSkillTag;

public: // Towers
	UFUNCTION(BlueprintCallable)
		class UKRVTowerUpgradeDataAsset* GetTowerUpgradeDA();

	UFUNCTION(BlueprintCallable)
		int32 GetTowerUpgradeDAIndex();

	UFUNCTION(BlueprintCallable)
		class AKRVTower* GetBoundTower() const;

	UFUNCTION(BlueprintCallable)
		bool CheckAndSetValidTowerUpgradeDAIndex();

	bool BindToTowerUpgrade(int32 UpgradeIndex, class AKRVTower* NewTower);
	bool BindToTowerSelling(class AKRVTower* NewTower);

protected:
	UFUNCTION()
		void OnUpgradeBegin(class UKRVTowerUpgradeDataAsset* NewTowerUpgradeDA, class AKRVTower* NewTower);

	UFUNCTION()
		void OnUpgradeFinished(class UKRVTowerUpgradeDataAsset* NewTowerUpgradeDA, class AKRVTower* NewTower);

	UFUNCTION()
		void OnSellingBegin(class AKRVTower* NewTower);

	UFUNCTION()
		void OnSellingFinished(class AKRVTower* NewTower);

public:
	FOnTowerUpgradeConfirmedDelegate OnTowerUpgradeConfirmedDelegate;
	FOnTowerSellingConfirmedDelegate OnTowerSellingConfirmedDelegate;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class AKRVTower* BoundTower;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVTowerUpgradeDataAsset* BoundTowerUpgradeDA;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		int32 BoundTowerUpgradeDAIndex;

public: // Hero
	bool BindToHero(class AKRVHero* NewHero);

protected:
	UFUNCTION()
		void OnReviveTimeNotified(float RemainingTime, float TotalTime);

	UFUNCTION()
		void OnUnitSelected(bool bIsSelected, class AKRVCombatUnit* NewUnit);

	UFUNCTION()
		void OnLevelChanged(int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent)
		void OnLevelChanged_BP(int32 NewLevel);

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class AKRVHero* BoundHero;

public: // Menu
	bool SetCustomMenuIcon(EIconTextureVariation NewVariation, const FText& NewTitle = FText::GetEmpty(), const FText& NewText = FText::GetEmpty(), const FText& NewWarning = FText::GetEmpty());

protected:
	void ClearCustomMenuTexts();

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		FText CustomTitleText;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		FText CustomTextText;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		FText CustomWarningText;

public: // Gateway
	bool BindToGateway(class AKRVGateway* NewGateway);

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void OnNextWaveCalled();
	void OnNextWaveCalled_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void OnNextWaveIncoming(float CallTimeLimit = -1.f);
	void OnNextWaveIncoming_Implementation(float CallTimeLimit = -1.f);

	UFUNCTION()
		void OnNextWaveCallRequested();

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class AKRVGateway* BoundGateway;

public: // Selection in map
	bool GetItemName(FName& OutName) const;
	bool SetItemVacant();

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		bool bIsInInventorySample;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		FName BoundItemName;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		bool bIsItemEquipped;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		bool bIsItemUnlocked;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		bool bIsItemRecentlyUnlocked;

public: // Hero selection in map
	bool BindToHeroInMap(class UKRVHeroDataAsset* HeroDAToBind, const FKRVHeroSaveData& HeroSaveDataToBind, bool bWouldBeInInventorySample = false);

	UFUNCTION()
		void OnHeroPicked(const FName& PickedHero);

public:
	FOnHeroInMapClickedDelegate OnHeroInMapClickedDelegate;

public: // Skill learning in map
	bool BindToSkillInMap(TSubclassOf<class AKRVSkill> SkillClassToBind, const FName& HeroToBind);
	bool OnPointGivenToHeroSkill(int32 NewSkillPoint, class UKRVDataAsset* NewDA, const FName& NewHeroName);

protected:
	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<class AKRVSkill> BoundSkillClass;

public:
	bool BindToSkillLearning(class UKRVDataAsset* SKillDAToBind, const FName& HeroToBind);

public: // Item selection in map
	UFUNCTION()
		void OnItemEquipped(const FName& ItemEquipped);

	bool BindToItemInMap(class UKRVItemDataAsset* ItemDAToBind, const FKRVItemStockData& ItemStockDataToBind, bool bWouldBeInInventorySample = false);

protected:
	UFUNCTION()
		void OnItemUnequipped(const FName& ItemUnequipped);

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVItemDataAsset* BoundItemDataAsset;

public: // Item purchase button
	bool BindToItemPurchase(class UKRVItemDataAsset* ItemDAToBind, const FKRVItemStockData& ItemStockDataToBind);

public: // Tower Selection In map
	bool BindToTowerInMap(class UKRVTowerDataAsset* TowerDAToBind, const FKRVTowerSaveData& TowerSaveDataToBind, bool bWouldBeInInventorySample = false);

public: // Level Selection In map
	bool BindToBattlefield(class UKRVLevelDataAsset* KRVLevelDataAsset, const FKRVLevelSaveData& LevelSaveDataToBind);

};
