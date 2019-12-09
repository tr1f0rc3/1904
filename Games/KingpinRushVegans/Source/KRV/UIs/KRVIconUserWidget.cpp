// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVIconUserWidget.h"
#include "TimerManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

#include "Button.h"
#include "Border.h"
#include "TextBlock.h"
#include "SizeBox.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "DataAssets/KRVDataAsset.h"
#include "DataAssets/KRVTowerUpgradeDataAsset.h"
#include "DataAssets/KRVHeroDataAsset.h"
#include "DataAssets/KRVItemDataAsset.h"
#include "DataAssets/KRVTowerDataAsset.h"
#include "DataAssets/KRVLevelDataAsset.h"

#include "Actors/KRVSkill.h"
#include "Actors/KRVItem.h"
#include "Actors/KRVGateway.h"
#include "Pawns/KRVHero.h"
#include "Pawns/KRVTower.h"
#include "Pawns/KRVCombatUnit.h"
#include "PlayerStates/KRVInGamePlayerState.h"
#include "PlayerStates/KRVMapPlayerState.h"

#include "Controllers/KRVInGamePlayerController.h"
#include "Controllers/KRVMapPlayerController.h"
#include "GameModes/KRVInGameGameMode.h"

#include "KRVIconTooltipUserWidget.h"
#include "Actors/KRVBattlefield.h"

void UKRVIconUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeThisWidget();

}

void UKRVIconUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	InitializeThisWidget();

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(IconTooltipWidgetClassPath, FStreamableDelegate::CreateUObject(this,
		&UKRVIconUserWidget::IconTooltipWidgetClassAsyncLoaded));
	Streamable.RequestAsyncLoad(LockedTexturePath, FStreamableDelegate::CreateUObject(this,
		&UKRVIconUserWidget::LockedTextureAsyncLoaded));

}

void UKRVIconUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsProgressing)
	{
		if (CurrentRemainingProgress > 0)
		{
			CurrentRemainingProgress -= InDeltaTime;
		}
		else
		{
			OnProgressFinished();
		}
		SetMIProgressRatio(TotalProgress - CurrentRemainingProgress, TotalProgress);
	}

}

void UKRVIconUserWidget::InitializeThisWidget()
{
	if (!bIsThisWidgetInitialized)
	{
		IconButton = Cast<UButton>(GetWidgetFromName(TEXT("BT_IconButton_1")));
		if (IconButton)
		{
			IconButton->SetVisibility(ESlateVisibility::Visible);

			IconButton->OnHovered.AddDynamic(this, &UKRVIconUserWidget::OnIconHovered);
			IconButton->OnUnhovered.AddDynamic(this, &UKRVIconUserWidget::OnIconUnhovered);
			IconButton->OnPressed.AddDynamic(this, &UKRVIconUserWidget::OnIconPressed);
			IconButton->OnReleased.AddDynamic(this, &UKRVIconUserWidget::OnIconReleased);
			IconButton->OnClicked.AddDynamic(this, &UKRVIconUserWidget::OnIconClicked);

			IconButton->WidgetStyle.Normal.DrawAs = ESlateBrushDrawType::Image;
			UMaterialInterface* ButtonBrushMI = UWidgetBlueprintLibrary::GetBrushResourceAsMaterial(IconButton->WidgetStyle.Normal);
			if (ButtonBrushMI)
			{
				UMaterialInstanceDynamic* IconMIDynamic_Normal = UMaterialInstanceDynamic::Create(ButtonBrushMI, this);
				if (IconMIDynamic_Normal)
				{
					IconButton->WidgetStyle.Normal.DrawAs = ESlateBrushDrawType::Image;
					SetMIProgressRatio(1.f, 1.f, IconMIDynamic_Normal);
					UWidgetBlueprintLibrary::SetBrushResourceToMaterial(IconButton->WidgetStyle.Normal, IconMIDynamic_Normal);
					DefaultTexture = Cast<UTexture2D>(IconMIDynamic_Normal->K2_GetTextureParameterValue(IconTextureName));
				}
				UMaterialInstanceDynamic* IconMIDynamic_Hovered = UMaterialInstanceDynamic::Create(ButtonBrushMI, this);
				if (IconMIDynamic_Hovered)
				{
					IconButton->WidgetStyle.Hovered.DrawAs = ESlateBrushDrawType::Image;
					SetMIProgressRatio(1.f, 1.f, IconMIDynamic_Hovered);
					UWidgetBlueprintLibrary::SetBrushResourceToMaterial(IconButton->WidgetStyle.Hovered, IconMIDynamic_Hovered);
				}
				UMaterialInstanceDynamic* IconMIDynamic_Pressed = UMaterialInstanceDynamic::Create(ButtonBrushMI, this);
				if (IconMIDynamic_Pressed)
				{
					IconButton->WidgetStyle.Pressed.DrawAs = ESlateBrushDrawType::Image;
					SetMIProgressRatio(1.f, 1.f, IconMIDynamic_Pressed);
					UWidgetBlueprintLibrary::SetBrushResourceToMaterial(IconButton->WidgetStyle.Pressed, IconMIDynamic_Pressed);
				}
			}
		}

		IconBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BD_IconBorder_1")));
		IntBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BD_Int_1")));
		IntTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Int_1")));
		ExclamationSizeBox = Cast<USizeBox>(GetWidgetFromName(TEXT("SB_Exclamation_1")));
		if (ExclamationSizeBox)
		{
			ExclamationSizeBox->SetVisibility(ESlateVisibility::Hidden);
		}

		SetIconStatus(EIconStatus::DisabledAndNotHovered);
		bIsThisWidgetInitialized = true;
	}

}

UKRVDataAsset* UKRVIconUserWidget::GetDataAsset_Implementation() const
{
	return BoundDataAsset;

}

bool UKRVIconUserWidget::LoadFromDataAsset_Implementation()
{
	if (!BoundDataAsset)
	{
		KRVCHECK(BoundDataAsset);
	}
	else
	{
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		DAIconTexturePath = GetDataAsset()->IconTexture.ToSoftObjectPath();
		Streamable.RequestAsyncLoad(DAIconTexturePath, FStreamableDelegate::CreateUObject(this,
			&UKRVIconUserWidget::OnIconTextureAsyncLoaded));
		return true;
	}
	return false;

}

void UKRVIconUserWidget::OnIconTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(DAIconTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		DAIconTexture = AssetPathPtr.Get();
		SetIconTextureVariation(EIconTextureVariation::DataAsset);
	}

}


void UKRVIconUserWidget::IconTooltipWidgetClassAsyncLoaded()
{
	TSoftClassPtr<UKRVIconTooltipUserWidget> AssetPathPtr(IconTooltipWidgetClassPath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		IconTooltipWidgetClass = AssetPathPtr.Get();
		CreateTooltipWidgetAndBindToButton();
		BindTooltipByIconType(true);
	}

}

bool UKRVIconUserWidget::BindTooltipByIconType(bool bIsCalledByAsyncLoad /*= false*/)
{
	if (!IconTooltipWidget)
	{
		KRVCHECK(IconTooltipWidget);
	}
	else if (bIsCalledByAsyncLoad && IconTooltipWidget->IsBoundTo())
	{
		// do nothing
	}
	else
	{
		switch (IconType)
		{
		case EIconType::Skill:
		{
			if (!BoundSkill)
			{
				KRVCHECK(BoundSkill);
			}
			else
			{
				return IconTooltipWidget->BindToSkill(BoundSkill);
			}
			break;
		}
		case EIconType::Item:
		{
			if (!BoundDataAsset)
			{
				KRVCHECK(BoundDataAsset);
			}
			else
			{
				return IconTooltipWidget->BindToDataAsset(BoundDataAsset);
			}
			break;
		}
		case EIconType::Hero:
		{
			if (!BoundDataAsset)
			{
				KRVCHECK(BoundDataAsset);
			}
			else
			{
				return IconTooltipWidget->BindToHero(BoundDataAsset);
			}
			break;
		}
		case EIconType::TowerUpgrade:
		{
			return IconTooltipWidget->BindToTowerUpgrade(BoundTowerUpgradeDAIndex, BoundTower);
		}
		case EIconType::TowerSelling:
		{
			return IconTooltipWidget->BindToTowerSelling(BoundTower);
		}
		case EIconType::Selection:
		{
			return IconTooltipWidget->BindToDataAsset(BoundDataAsset);
		}
		case EIconType::Menu:
		{
			if (IconTooltipWidget->SetTexts(CustomTitleText, CustomTextText, CustomWarningText))
			{
				ClearCustomMenuTexts();
				return true;
			}
			return false;
		}
		case EIconType::Gateway:
		{
			return IconTooltipWidget->BindtoGateway(BoundGateway);
		}
		case EIconType::HeroInMap:
		{
			if (!BoundDataAsset)
			{
				KRVCHECK(BoundDataAsset);
			}
			else
			{
				return IconTooltipWidget->BindToHeroInMap(Cast<UKRVHeroDataAsset>(BoundDataAsset), bIsItemUnlocked);
			}
			break;
		}
		case EIconType::ItemInMap:
		{
			if (!BoundDataAsset)
			{
				KRVCHECK(BoundDataAsset);
			}
			else
			{
				return IconTooltipWidget->BindToItemInMap(BoundItemDataAsset, bIsItemUnlocked);
			}
			break;
		}
		case EIconType::TowerInMap:
		{
			if (!BoundDataAsset)
			{
				KRVCHECK(BoundDataAsset);
			}
			else
			{
				return IconTooltipWidget->BindToTowerInMap(Cast<UKRVTowerDataAsset>(BoundDataAsset), bIsItemUnlocked);
			}
			break;
		}
		case EIconType::Battlefield:
		{
			return IconTooltipWidget->BindToDataAsset(BoundDataAsset);
		}
		case EIconType::SkillInMap:
		{
			if (!BoundSkill && BoundSkillClass)
			{
				BoundSkill = BoundSkillClass.GetDefaultObject();
			}

			if (!BoundSkill)
			{
				KRVCHECK(BoundSkill);
			}
			else
			{
				return IconTooltipWidget->BindToSkill(BoundSkill);
			}
			break;
		}
		default:
			break;
		}
	}
	return false;

}

UWidget* UKRVIconUserWidget::CreateTooltipWidgetAndBindToButton()
{
	if (!IconTooltipWidgetClass)
	{
		KRVCHECK(IconTooltipWidgetClass);
	}
	else
	{
		IconTooltipWidget = CreateWidget<UKRVIconTooltipUserWidget>(this, IconTooltipWidgetClass);
		if (!IconTooltipWidget)
		{
			KRVCHECK(IconTooltipWidget);
		}
		else
		{
			IconButton->SetToolTip(IconTooltipWidget);
		}
		return IconTooltipWidget;
	}
	return nullptr;

}

void UKRVIconUserWidget::LockedTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(LockedTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		LockedTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::Locked)
		{
			SetIconTextureVariation(EIconTextureVariation::Locked, true);
		}
	}

}

void UKRVIconUserWidget::ConfirmTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(ConfirmTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		ConfirmTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::Confirm)
		{
			SetIconTextureVariation(EIconTextureVariation::Confirm, true);
		}
	}

}

void UKRVIconUserWidget::CancleTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(CancleTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		CancleTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::Cancle)
		{
			SetIconTextureVariation(EIconTextureVariation::Cancle, true);
		}
	}

}

void UKRVIconUserWidget::RestoreTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(RestoreTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		RestoreTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::Restore)
		{
			SetIconTextureVariation(EIconTextureVariation::Restore, true);
		}
	}

}

void UKRVIconUserWidget::AchievementTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(AchievementTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		AchievementTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::Achievement)
		{
			SetIconTextureVariation(EIconTextureVariation::Achievement, true);
		}
	}

}

void UKRVIconUserWidget::QuestionMarkTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(QuestionMarkTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		QuestionMarkTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::QuestionMark)
		{
			SetIconTextureVariation(EIconTextureVariation::QuestionMark, true);
		}
	}

}

void UKRVIconUserWidget::DocumentTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(DocumentTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		DocumentTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::Document)
		{
			SetIconTextureVariation(EIconTextureVariation::Document, true);
		}
	}

}

void UKRVIconUserWidget::LeaderboardTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(LeaderboardTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		LeaderboardTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::Leaderboard)
		{
			SetIconTextureVariation(EIconTextureVariation::Leaderboard, true);
		}
	}

}

void UKRVIconUserWidget::MenuTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(MenuTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		MenuTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::Menu)
		{
			SetIconTextureVariation(EIconTextureVariation::Menu, true);
		}
	}

}

void UKRVIconUserWidget::StarTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(StarTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		StarTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::Star)
		{
			SetIconTextureVariation(EIconTextureVariation::Star, true);
		}
	}

}

void UKRVIconUserWidget::SettingTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(SettingTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		SettingTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::Setting)
		{
			SetIconTextureVariation(EIconTextureVariation::Setting, true);
		}
	}

}

void UKRVIconUserWidget::RightArrowTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(RightArrowTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		RightArrowTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::RightArrow)
		{
			SetIconTextureVariation(EIconTextureVariation::RightArrow, true);
		}
	}

}

void UKRVIconUserWidget::ShutDownTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(ShutDownTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		ShutDownTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::ShutDown)
		{
			SetIconTextureVariation(EIconTextureVariation::ShutDown, true);
		}
	}

}

void UKRVIconUserWidget::LogOutTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(LogOutTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		LogOutTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::LogOut)
		{
			SetIconTextureVariation(EIconTextureVariation::LogOut, true);
		}
	}

}

void UKRVIconUserWidget::SoundTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(SoundTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		SoundTexture = AssetPathPtr.Get();
		if (IconTextureVariation == EIconTextureVariation::Sound)
		{
			SetIconTextureVariation(EIconTextureVariation::Sound, true);
		}
	}

}

bool UKRVIconUserWidget::BindToDataAsset(class UKRVDataAsset* NewDA, bool bDoesBindToTooltip/* = true*/)
{
	if (!IconButton || !NewDA)
	{
		KRVCHECK(IconButton);
		KRVCHECK(NewDA);
	}
	else
	{
		bool bBindingResult = true;
		BoundDataAsset = NewDA;
		bBindingResult &= LoadFromDataAsset_Implementation();
		if (bDoesBindToTooltip)
		{
			BindTooltipByIconType();
		}
		return bBindingResult;
	}
	return false;

}

bool UKRVIconUserWidget::OnHighlightedEffect_Implementation(bool bIsActive)
{
	if (bIsHighlighted != bIsActive)
	{
		bIsHighlighted = bIsActive;
		if (bIsActive && GetWorld())
		{
			AKRVPlayerController* KRVPlayerController = UKRVBlueprintFunctionLibrary::GetKRVPlayerController(GetWorld());
			if (KRVPlayerController)
			{
				KRVPlayerController->PlayOnHightlightedSound();
			}
		}
		return true;
	}
	return false;

}

void UKRVIconUserWidget::OnRemainingProgressNotified(float NewRemainingProgress, float NewTotalProgress)
{
	CurrentRemainingProgress = NewRemainingProgress;
	TotalProgress = NewTotalProgress;
	if (NewRemainingProgress > 0.f)
	{
		bIsProgressing = true;
	}
	SetMIProgressRatio(NewTotalProgress - NewRemainingProgress, NewTotalProgress);

}

UMaterialInstanceDynamic* UKRVIconUserWidget::GetIconMIDynamic(EIconInteraction NewIconInterAction)
{
	if (!IconButton)
	{
		KRVCHECK(IconButton)
	}
	else
	{
		switch (NewIconInterAction)
		{
		case EIconInteraction::Normal:
			return UWidgetBlueprintLibrary::GetDynamicMaterial(IconButton->WidgetStyle.Normal);
		case EIconInteraction::Pressed:
			return UWidgetBlueprintLibrary::GetDynamicMaterial(IconButton->WidgetStyle.Pressed);
		case EIconInteraction::Hovered:
			return UWidgetBlueprintLibrary::GetDynamicMaterial(IconButton->WidgetStyle.Hovered);
		default:
			break;
		}
	}
	return nullptr;

}

bool UKRVIconUserWidget::SetIconType(EIconType NewIconType)
{
	if (!IconButton || IconType == NewIconType)
	{
		KRVCHECK(IconButton);
		return false;
	}
	else
	{
		IconType = NewIconType;
	}
	return true;

}


void UKRVIconUserWidget::SetIconStatus(EIconStatus NewStatus)
{
	IconStatus = NewStatus;

	switch (NewStatus)
	{
	case EIconStatus::NONE:
		break;
	case EIconStatus::NoInteraction:
	{
		bIsHoverable = false;
		bIsClickable = false;
		GetIconMIDynamic(EIconInteraction::Normal)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_NoInteraction);
		GetIconMIDynamic(EIconInteraction::Hovered)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_NoInteraction);
		GetIconMIDynamic(EIconInteraction::Pressed)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_NoInteraction);
		break;
	}
	case EIconStatus::Normal:
	{
		bIsHoverable = true;
		bIsClickable = true;
		GetIconMIDynamic(EIconInteraction::Normal)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_Normal);
		GetIconMIDynamic(EIconInteraction::Hovered)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_Hovered);
		GetIconMIDynamic(EIconInteraction::Pressed)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_Pressed);
		break;
	}
	case EIconStatus::Disabled:
	{
		bIsHoverable = true;
		bIsClickable = false;
		GetIconMIDynamic(EIconInteraction::Normal)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_Disabled);
		GetIconMIDynamic(EIconInteraction::Hovered)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_Normal);
		GetIconMIDynamic(EIconInteraction::Pressed)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_Disabled);
		break;
	}
	case EIconStatus::OnlyHovered:
	{
		bIsHoverable = true;
		bIsClickable = false;
		GetIconMIDynamic(EIconInteraction::Normal)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_NoInteraction);
		GetIconMIDynamic(EIconInteraction::Hovered)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_NoInteraction);
		GetIconMIDynamic(EIconInteraction::Pressed)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_NoInteraction);
		break;
	}
	case EIconStatus::DisabledAndNotHovered:
	{
		bIsHoverable = false;
		bIsClickable = false;
		GetIconMIDynamic(EIconInteraction::Normal)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_Disabled);
		GetIconMIDynamic(EIconInteraction::Hovered)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_Disabled);
		GetIconMIDynamic(EIconInteraction::Pressed)->SetScalarParameterValue(RadialGradientDensityName, MIRadialGradientDensity_Disabled);
		break;
	}
	default:
		break;
	}

}

EIconType UKRVIconUserWidget::OnProgressFinishedEffect_Implementation(EIconType NewIconType/*= EIconType::NONE*/)
{
	if (NewIconType == EIconType::NONE)
	{
		NewIconType = IconType;
	}

	AKRVPlayerController* KRVPlayerController = nullptr;
	if (GetWorld())
	{
		KRVPlayerController = UKRVBlueprintFunctionLibrary::GetKRVPlayerController(GetWorld());
	}

	switch (NewIconType)
	{
	case EIconType::TowerUpgrade:
	{
		if (KRVPlayerController)
		{
			KRVPlayerController->PlayUpgradeSound();
		}
		break;
	}
	case EIconType::Gateway:
	{
		if (KRVPlayerController)
		{
			KRVPlayerController->PlayWaveCallSound();
		}
		break;
	}
	default:
		break;
	}
	return NewIconType;

}

void UKRVIconUserWidget::UnavailbleEffect_Implementation()
{
	if (GetWorld())
	{
		AKRVPlayerController* KRVPlayerController = UKRVBlueprintFunctionLibrary::GetKRVPlayerController(GetWorld());
		if (KRVPlayerController)
		{
			KRVPlayerController->PlayRejectedSound();
		}
	}

}

bool UKRVIconUserWidget::SetMIProgressRatio(float CurrentValue, float MaxValue, UMaterialInstanceDynamic* NewMIDynamic /*= nullptr*/)
{
	if (!IconButton)
	{
		KRVCHECK(IconButton);
	}
	else
	{
		float ProgressRatio = FMath::Clamp((CurrentValue / MaxValue), 0.f, 1.f);
		if (!NewMIDynamic)
		{
			NewMIDynamic = GetIconMIDynamic(IconInteraction);
		}
		if (NewMIDynamic)
		{
			NewMIDynamic->SetScalarParameterValue(ProgressRatioName, ProgressRatio);
			return true;
		}
	}
	return false;

}

void UKRVIconUserWidget::OnProgressFinished()
{
	if (!bIsProgressing)
	{
		return;
	}
	else
	{
		bIsProgressing = false;

		SetMIProgressRatio(1.f, 1.f, GetIconMIDynamic(EIconInteraction::Normal));
		SetMIProgressRatio(1.f, 1.f, GetIconMIDynamic(EIconInteraction::Hovered));
		SetMIProgressRatio(1.f, 1.f, GetIconMIDynamic(EIconInteraction::Pressed));

		OnProgressFinishedEffect();
		OnProgressFinishedDelegate.Broadcast();
	}

}

bool UKRVIconUserWidget::GetWorldSpaceOfWidget(FTransform& OutTransform)
{
	APlayerController* PlayerCtrl = GetGameInstance()->GetEngine()->GetFirstLocalPlayerController(GetWorld());
	if (PlayerCtrl)
	{
		FVector2D ParentViewportPos = FVector2D(0.f, 0.f);
		UPanelWidget* ParentPanelWidget = GetParent();
		while (ParentPanelWidget)
		{
			ParentViewportPos += GetParent()->GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(0.f, 0.f));
			ParentPanelWidget = ParentPanelWidget->GetParent();
		}

		FVector2D IconCenterViewportPos = GetCachedGeometry().GetLocalPositionAtCoordinates(FVector2D(0.5f, 0.5f)) + ParentViewportPos;
		FVector IconCenterWorldLoc;
		FVector IconDir;
		PlayerCtrl->DeprojectScreenPositionToWorld
		(IconCenterViewportPos.X, IconCenterViewportPos.Y, IconCenterWorldLoc, IconDir);
		FQuat IconRot = FRotationMatrix::MakeFromX(IconDir).ToQuat();
		OutTransform.SetRotation(IconRot);
		OutTransform.SetLocation(IconCenterWorldLoc);
		return true;
	}
	return false;
}

void UKRVIconUserWidget::OnIconClicked()
{
	if (!bIsClickable || IconInteraction == EIconInteraction::Pressed)
	{
		return;
	}
	else
	{
		++ConsecutiveClicks;
		SetBorderScale(IconInteraction);

		switch (IconType)
		{
		case EIconType::Skill:
		{
			if (!bIsProgressing)
			{
				AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
				if (KRVInGamePlayerController)
				{
					if (ConsecutiveClicks == 1)
					{
						if (!KRVInGamePlayerController->TryCastSkill(BoundSkillTag, BoundCaster))
						{
							UnavailbleEffect();
							ResetConsecutiveClicks();
						}
						else if (!KRVInGamePlayerController->IsTargeting())
						{
							ResetConsecutiveClicks();
						}
						else
						{
							KRVInGamePlayerController->OnTargetingEndDelegate.BindUObject(this, &UKRVIconUserWidget::ResetConsecutiveClicks);
						}
					}
					else if (ConsecutiveClicks == 2)
					{
						KRVInGamePlayerController->CancleTargeting();
						ResetConsecutiveClicks();
					}
				}
			}
			break;
		}
		case EIconType::TowerUpgrade:
		{
			if (CheckAndSetValidTowerUpgradeDAIndex() && !bIsProgressing)
			{
				if (ConsecutiveClicks == 1)
				{
					if (BoundTower->IsAbleToUpgradeTower(BoundTowerUpgradeDAIndex))
					{
						SetIconTextureVariation(EIconTextureVariation::Confirm);
					}
					else
					{
						UnavailbleEffect();
						ResetConsecutiveClicks();
					}
				}
				else if (ConsecutiveClicks == 2)
				{
					if (BoundTower->UpgradeTower(BoundTowerUpgradeDAIndex))
					{
						SetIconTextureVariation(EIconTextureVariation::DataAsset);
						OnTowerUpgradeConfirmedDelegate.Broadcast();
					}
					ResetConsecutiveClicks();
				}
			}
			break;
		}
		case EIconType::TowerSelling:
		{
			if (!bIsProgressing)
			{
				if (!BoundTower->IsSellable())
				{
					UnavailbleEffect();
					ResetConsecutiveClicks();
				}
				else if (ConsecutiveClicks == 1)
				{
					SetIconTextureVariation(EIconTextureVariation::Confirm);
				}
				else if (ConsecutiveClicks == 2)
				{
					SetIconTextureVariation(EIconTextureVariation::Restore);
					BoundTower->SellThis();
					OnTowerSellingConfirmedDelegate.Broadcast();
					ResetConsecutiveClicks();
				}
			}
			break;

		}
		case EIconType::Hero:
		{
			if (GetWorld() && !bIsProgressing)
			{
				AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
				if (!KRVInGamePlayerController || !BoundHero)
				{
					KRVCHECK(KRVInGamePlayerController);
					KRVCHECK(BoundHero);
				}
				else if (BoundHero->GetActionStatus() == EActionStatus::Dead)
				{
					UnavailbleEffect();
				}
				else if (BoundHero->IsCurrentlySelected())
				{
					KRVInGamePlayerController->UnselectUnit();
				}
				else
				{
					KRVInGamePlayerController->SelectUnit(Cast<AKRVCombatUnit>(BoundHero));
				}
			}
			break;
		}
		case EIconType::Gateway:
		{
			if (!BoundGateway)
			{
				KRVCHECK(BoundGateway);
			}
			else if (ConsecutiveClicks == 1)
			{
				SetIconTextureVariation(EIconTextureVariation::Confirm);
				BoundGateway->RequestToShowNextWavePaths(true);
			}
			else if (ConsecutiveClicks == 2)
			{
				BoundGateway->RequestNextWaveCall();
				SetIconTextureVariation(EIconTextureVariation::Default);
				OnNextWaveCalled();
				ResetConsecutiveClicks();
			}
			break;
		}
		case EIconType::HeroInMap:
		{
			if (!BoundDataAsset)
			{
				KRVCHECK(BoundDataAsset);
			}
			else if (ConsecutiveClicks == 1)
			{
				SetIconTextureVariation(EIconTextureVariation::Confirm);
				OnHeroInMapClickedDelegate.Broadcast(BoundItemName);
			}
			else if (ConsecutiveClicks == 2)
			{
				AKRVMapPlayerController* KRVMapPlayerController = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(GetWorld());
				if (KRVMapPlayerController)
				{
					KRVMapPlayerController->PickHero(BoundItemName);
				}
				ResetConsecutiveClicks();
			}
			else
			{
				ResetConsecutiveClicks();
			}
			break;
		}
		case EIconType::ItemInMap:
		{
			if (!BoundDataAsset)
			{
				KRVCHECK(BoundDataAsset);
			}
			else if (ConsecutiveClicks == 1)
			{
				if (!bIsItemEquipped)
				{
					SetIconTextureVariation(EIconTextureVariation::Confirm);
				}
				else
				{
					SetIconTextureVariation(EIconTextureVariation::Cancle);
				}
			}
			else if (ConsecutiveClicks == 2)
			{
				AKRVMapPlayerController* KRVMapPlayerController = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(GetWorld());
				if (KRVMapPlayerController)
				{
					if (!bIsItemEquipped)
					{
						if (!KRVMapPlayerController->EquipItem(BoundItemName))
						{
							UnavailbleEffect();
						}
					}
					else
					{
						KRVCHECK(KRVMapPlayerController->UnequipItem(BoundItemName));
					}
				}
				ResetConsecutiveClicks();
			}
			else
			{
				ResetConsecutiveClicks();
			}
			break;
		}
		case EIconType::TowerInMap:
		{
			if (!BoundDataAsset)
			{
				KRVCHECK(BoundDataAsset);
			}
			else if (ConsecutiveClicks == 1)
			{
				if (!bIsItemEquipped)
				{
					SetIconTextureVariation(EIconTextureVariation::Confirm);
				}
				else
				{
					SetIconTextureVariation(EIconTextureVariation::Cancle);
				}
			}
			else if (ConsecutiveClicks == 2)
			{
				AKRVMapPlayerController* KRVMapPlayerController = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(GetWorld());
				if (KRVMapPlayerController)
				{
					if (!bIsItemEquipped)
					{
						if (!KRVMapPlayerController->EquipTower(BoundItemName))
						{
							UnavailbleEffect();
						}
					}
					else
					{
						KRVCHECK(KRVMapPlayerController->UnequipTower(BoundItemName));
					}
				}
				ResetConsecutiveClicks();
			}
			else
			{
				ResetConsecutiveClicks();
			}
			break;

		}
		case EIconType::ItemPurchase:
		{
			AKRVMapPlayerController* KRVMapPlayerController = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(GetWorld());
			if (KRVMapPlayerController)
			{
				if (!KRVMapPlayerController->BuyItem(BoundItemDataAsset))
				{
					UnavailbleEffect();
				}
			}
			ResetConsecutiveClicks();
		}
		case EIconType::SkillLearning:
		{
			AKRVMapPlayerController* KRVMapPlayerController = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(GetWorld());
			if (KRVMapPlayerController)
			{
				if (!KRVMapPlayerController->GivePointToHeroSkill(BoundDataAsset, BoundItemName))
				{
					UnavailbleEffect();
				}
			}
		}
		default:
		{
			ConsecutiveClicks = 0;
			break;
		}
		}

		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(AfterClickedTimer, this, &UKRVIconUserWidget::AfterClicked, 0.1f, false);
		}
		else
		{
			AfterClicked();
		}
		OnIconClickedDelegate.Broadcast();
	}

}

void UKRVIconUserWidget::ResetConsecutiveClicks()
{
	ConsecutiveClicks = 0;

}


bool UKRVIconUserWidget::SetCustomIconTexture(UTexture2D* NewTexture)
{
	if (!IconButton || !NewTexture)
	{
		KRVCHECK(IconButton);
		KRVCHECK(NewTexture);
	}
	else
	{
		CustomTexture = NewTexture;
		SetIconTextureVariation(EIconTextureVariation::Custom);

		GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, NewTexture);
		GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, NewTexture);
		GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, NewTexture);
		return true;
	}
	return false;
}

bool UKRVIconUserWidget::SetIconTextureVariation(EIconTextureVariation NewVariation, bool bIsCalledByAsyncLoad /*= false*/)
{
	if (!IconButton || (!bIsCalledByAsyncLoad && IconTextureVariation == NewVariation))
	{
		KRVCHECK(IconButton);
	}
	else
	{
		IconTextureVariation = NewVariation;

		switch (NewVariation)
		{
		case EIconTextureVariation::Default:
		{
			if (DefaultTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, DefaultTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, DefaultTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, DefaultTexture);
			}
			break;
		}
		case EIconTextureVariation::Locked:
		{
			if (LockedTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, LockedTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, LockedTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, LockedTexture);
			}
			break;
		}
		case EIconTextureVariation::DataAsset:
		{
			if (DAIconTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, DAIconTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, DAIconTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, DAIconTexture);
			}
			break;
		}
		case EIconTextureVariation::Confirm:
		{
			if (ConfirmTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, ConfirmTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, ConfirmTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, ConfirmTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(ConfirmTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::ConfirmTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::Cancle:
		{
			if (CancleTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, CancleTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, CancleTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, CancleTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(CancleTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::CancleTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::Restore:
		{
			if (RestoreTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, RestoreTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, RestoreTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, RestoreTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(RestoreTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::RestoreTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::Achievement:
		{
			if (AchievementTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, AchievementTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, AchievementTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, AchievementTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(AchievementTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::AchievementTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::QuestionMark:
		{
			if (QuestionMarkTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, QuestionMarkTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, QuestionMarkTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, QuestionMarkTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(QuestionMarkTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::QuestionMarkTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::Document:
		{
			if (DocumentTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, DocumentTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, DocumentTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, DocumentTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(DocumentTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::DocumentTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::Leaderboard:
		{
			if (LeaderboardTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, LeaderboardTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, LeaderboardTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, LeaderboardTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(LeaderboardTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::LeaderboardTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::Menu:
		{
			if (MenuTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, MenuTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, MenuTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, MenuTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(MenuTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::MenuTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::Star:
		{
			if (StarTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, StarTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, StarTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, StarTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(StarTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::StarTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::Setting:
		{
			if (SettingTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, SettingTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, SettingTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, SettingTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(SettingTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::SettingTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::RightArrow:
		{
			if (RightArrowTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, RightArrowTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, RightArrowTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, RightArrowTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(RightArrowTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::RightArrowTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::ShutDown:
		{
			if (ShutDownTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, ShutDownTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, ShutDownTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, ShutDownTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(ShutDownTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::ShutDownTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::LogOut:
		{
			if (LogOutTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, LogOutTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, LogOutTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, LogOutTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(LogOutTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::LogOutTextureAsyncLoaded));
			}
			break;
		}
		case EIconTextureVariation::Sound:
		{
			if (SoundTexture)
			{
				GetIconMIDynamic(EIconInteraction::Normal)->SetTextureParameterValue(IconTextureName, SoundTexture);
				GetIconMIDynamic(EIconInteraction::Hovered)->SetTextureParameterValue(IconTextureName, SoundTexture);
				GetIconMIDynamic(EIconInteraction::Pressed)->SetTextureParameterValue(IconTextureName, SoundTexture);
			}
			else if (!bIsCalledByAsyncLoad)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				Streamable.RequestAsyncLoad(SoundTexturePath, FStreamableDelegate::CreateUObject(this,
					&UKRVIconUserWidget::SoundTextureAsyncLoaded));
			}
			break;
		}
		}
		return true;
	}
	return false;
}

void UKRVIconUserWidget::OnIconHovered()
{
	if (bIsHoverable)
	{
		IconInteraction = EIconInteraction::Hovered;
		SetBorderScale(IconInteraction);
		if (IconTooltipWidget)
		{
			IconTooltipWidget->ShowThisWidget(true);
		}

		OnIconHoveredDelegate.Broadcast();
	}

}

void UKRVIconUserWidget::OnIconUnhovered()
{
	if (!bIsHoverable)
	{
		return;
	}
	else
	{
		IconInteraction = EIconInteraction::Normal;
		SetBorderScale(IconInteraction);
		if (IconTooltipWidget)
		{
			IconTooltipWidget->ShowThisWidget(false);
		}

		switch (IconType)
		{
		case EIconType::Gateway:
		{
			if (BoundGateway)
			{
				BoundGateway->RequestToShowNextWavePaths(false);
			}
			SetIconTextureVariation(EIconTextureVariation::Default);
			ResetConsecutiveClicks();
			break;
		}
		case EIconType::HeroInMap:
		{
			SetIconTextureVariation(EIconTextureVariation::DataAsset);
			ResetConsecutiveClicks();
			break;
		}
		case EIconType::ItemInMap:
		{
			SetIconTextureVariation(EIconTextureVariation::DataAsset);
			ResetConsecutiveClicks();
			break;
		}
		case EIconType::TowerInMap:
		{
			SetIconTextureVariation(EIconTextureVariation::DataAsset);
			ResetConsecutiveClicks();
			break;
		}
		default:
			break;

		}
		OnIconUnhoveredDelegate.Broadcast();
	}

}

void UKRVIconUserWidget::OnIconPressed()
{
	if (bIsClickable)
	{
		IconInteraction = EIconInteraction::Pressed;
		SetBorderScale(IconInteraction);
		if (GetWorld())
		{
			AKRVPlayerController* KRVPlayerController = UKRVBlueprintFunctionLibrary::GetKRVPlayerController(GetWorld());
			if (KRVPlayerController)
			{
				KRVPlayerController->PlayOnPressedSound();
			}
		}
		OnIconPressedDelegate.Broadcast();
	}

}

bool UKRVIconUserWidget::SetBorderScale(EIconInteraction NewInteraction)
{
	if (!IconBorder)
	{
		KRVCHECK(IconBorder);
	}
	else
	{
		float NewScale = 1.0f;
		switch (NewInteraction)
		{
		case EIconInteraction::Normal:
			NewScale = IconBorderScale_Normal;
			break;
		case EIconInteraction::Pressed:
			NewScale = IconBorderScale_Pressed;
			break;
		case EIconInteraction::Hovered:
			NewScale = IconBorderScale_Hovered;
			break;
		default:
			break;
		}

		FWidgetTransform WidgetTransform;
		WidgetTransform.Scale = FVector2D(NewScale, NewScale);
		IconBorder->SetRenderTransform(WidgetTransform);
		return true;
	}
	return false;

}

void UKRVIconUserWidget::AfterClicked()
{
	if (IsHovered())
	{
		IconInteraction = EIconInteraction::Hovered;
	}
	else
	{
		IconInteraction = EIconInteraction::Normal;
	}
	SetBorderScale(IconInteraction);

}

void UKRVIconUserWidget::OnIconReleased()
{
	if (bIsClickable)
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(AfterClickedTimer);
			AfterClicked();
		}
		OnIconReleasedDelegate.Broadcast();
	}

}



bool UKRVIconUserWidget::IsBound() const
{
	return bIsBound;

}

bool UKRVIconUserWidget::BindToSkill(const FGameplayTag& SkillTagToBind, class AKRVCombatUnit* CasterToBind)
{
	if (!CasterToBind || !IconButton)
	{
		KRVCHECK(IconButton);
		KRVCHECK(CasterToBind);
	}
	else
	{
		AKRVSkill* NewSkill = CasterToBind->GetSkillByTag(SkillTagToBind);
		if (!NewSkill)
		{
			KRVCHECK(NewSkill);
		}
		else
		{
			// Bind and set IconType
			SetIconType(EIconType::Skill);
			BoundCaster = CasterToBind;
			BoundSkill = NewSkill;
			BoundSkillTag = SkillTagToBind;

			// Bind to icon
			BindToDataAsset(NewSkill->GetDataAsset(), false);
			if (IntBorder)
			{
				IntBorder->SetVisibility(ESlateVisibility::Hidden);
			}

			// Set whether it is clickable
			if (NewSkill->GetMasteryLevel() <= 0)
			{
				SetIconStatus(EIconStatus::Disabled);
			}
			else
			{
				SetIconStatus(EIconStatus::Normal);
			}

			// Tooltip
			BindTooltipByIconType();

			NewSkill->OnCooldownNotificationDelegate.AddUObject(this, &UKRVIconUserWidget::OnRemainingProgressNotified);
			CasterToBind->OnSkillAcquireDelegate.AddUObject(this, &UKRVIconUserWidget::OnSkillAcquired);
			bIsBound = true;
			return true;
		}
	}
	return false;

}

bool UKRVIconUserWidget::BindToItem(const FGameplayTag& ItemTagToBind, AKRVCombatUnit* CasterToBind)
{
	if (BindToSkill(ItemTagToBind, CasterToBind))
	{
		AKRVItem* ItemToBind = Cast<AKRVItem>(CasterToBind->GetSkillByTag(ItemTagToBind));
		if (ItemToBind)
		{
			if (IntBorder && IntTextBlock)
			{
				AKRVInGamePlayerState* KRVInGamePlayerState = CasterToBind->GetPlayerState<AKRVInGamePlayerState>();
				KRVCHECK(KRVInGamePlayerState);
				if (KRVInGamePlayerState)
				{
					OnItemStockChanged(KRVInGamePlayerState->GetRemainingItemStock(ItemToBind), ItemToBind);
					KRVInGamePlayerState->OnItemStockChangedDelegate.AddUObject(this, &UKRVIconUserWidget::OnItemStockChanged);
				}
				IntBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			return true;
		}
	}
	return false;

}

void UKRVIconUserWidget::OnSkillAcquired(AKRVSkill* NewSkill, AKRVCombatUnit* NewCaster)
{
	if (!NewSkill || !NewCaster)
	{
		KRVCHECK(NewSkill);
		KRVCHECK(NewCaster);
	}
	else if (!NewSkill->GetDataAsset_Implementation())
	{
		KRVCHECK(NewSkill->GetDataAsset_Implementation());
	}
	else if (BoundCaster == NewCaster)
	{
		// Update if the object is same.
		// 오브젝트가 같을 경우 업데이트함.
		FGameplayTag NewSkillTag = UKRVBlueprintFunctionLibrary::GetGameplayTagByName
		(NewSkill->GetDataAsset_Implementation()->TagContainer, "Name.Skill");
		if (BoundSkillTag == NewSkillTag)
		{
			bIsBound = false;
			BindToSkill(BoundSkillTag, NewCaster);
		}
	}

}

void UKRVIconUserWidget::OnItemStockChanged(int32 NewStock, class AKRVItem* NewItem)
{
	if (NewItem)
	{
		if (BoundSkill == Cast<AKRVSkill>(NewItem) && IntBorder&& IntTextBlock)
		{
			IntTextBlock->SetText(FText::FromString(FString::FromInt(NewStock)));
		}
	}

}

UKRVTowerUpgradeDataAsset* UKRVIconUserWidget::GetTowerUpgradeDA()
{
	if (CheckAndSetValidTowerUpgradeDAIndex())
	{
		return BoundTowerUpgradeDA;
	}
	return nullptr;

}

int32 UKRVIconUserWidget::GetTowerUpgradeDAIndex()
{
	if (CheckAndSetValidTowerUpgradeDAIndex())
	{
		return BoundTowerUpgradeDAIndex;
	}
	return -1;

}

AKRVTower* UKRVIconUserWidget::GetBoundTower() const
{
	return BoundTower;

}

bool UKRVIconUserWidget::BindToTowerUpgrade(int32 NewUpgradeIndex, AKRVTower* NewTower)
{
	if (!NewTower)
	{
		KRVCHECK(NewTower);
	}
	else
	{
		UKRVTowerUpgradeDataAsset* TowerUpgradeDA = NewTower->GetTowerUpgradeDA(NewUpgradeIndex);
		if (!TowerUpgradeDA)
		{
			KRVCHECK(TowerUpgradeDA);
		}
		else
		{
			SetIconType(EIconType::TowerUpgrade);
			BoundTower = NewTower;
			BoundTowerUpgradeDAIndex = NewUpgradeIndex;
			BoundTowerUpgradeDA = TowerUpgradeDA;

			FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
			//Streamable.RequestAsyncLoad(UpgradeSoundPath, FStreamableDelegate::CreateUObject(this,
			//	&UKRVIconUserWidget::UpgradeSoundAsyncLoaded));

			BindToDataAsset(TowerUpgradeDA);
			IntBorder->SetVisibility(ESlateVisibility::Hidden);
			SetIconStatus(EIconStatus::Normal);

			BindTooltipByIconType();

			NewTower->OnUpgradeBeginDelegate.AddUObject(this, &UKRVIconUserWidget::OnUpgradeBegin);
			NewTower->OnUpgradeFinishedDelegate.AddUObject(this, &UKRVIconUserWidget::OnUpgradeFinished);
			bIsBound = true;
			return true;
		}
	}
	return false;

}

bool UKRVIconUserWidget::BindToTowerSelling(AKRVTower* NewTower)
{
	if (!NewTower)
	{
		KRVCHECK(NewTower);
	}
	else
	{
		BoundTower = NewTower;
		SetIconType(EIconType::TowerSelling);
		IntBorder->SetVisibility(ESlateVisibility::Hidden);

		if (NewTower->IsSellable())
		{
			SetIconTextureVariation(EIconTextureVariation::Restore);
			NewTower->OnSellingBeginDelegate.AddUObject(this, &UKRVIconUserWidget::OnSellingBegin);
			NewTower->OnSellingFinishedDelegate.AddUObject(this, &UKRVIconUserWidget::OnSellingFinished);
			SetIconStatus(EIconStatus::Normal);
		}
		else
		{
			SetIconTextureVariation(EIconTextureVariation::Locked);
			SetIconStatus(EIconStatus::Disabled);
		}

		BindTooltipByIconType();
		bIsBound = true;
		return true;
	}
	return false;

}


bool UKRVIconUserWidget::CheckAndSetValidTowerUpgradeDAIndex()
{
	if (!BoundTower || !BoundTowerUpgradeDA)
	{
		KRVCHECK(BoundTower);
		KRVCHECK(BoundTowerUpgradeDA);
	}
	else if (BoundTower->GetTowerUpgradeDA(BoundTowerUpgradeDAIndex) == BoundTowerUpgradeDA)
	{
		return true;
	}
	else
	{
		int32 NewTowerUpgradeDAIndex = BoundTower->GetIndexByTowerUpgradeDA(BoundTowerUpgradeDA);
		if (NewTowerUpgradeDAIndex == INDEX_NONE)
		{
			BoundTowerUpgradeDAIndex = -1;
			SetIconTextureVariation(EIconTextureVariation::Locked);
		}
		else
		{
			BoundTowerUpgradeDAIndex = NewTowerUpgradeDAIndex;
			return true;
		}
	}
	return false;

}

void UKRVIconUserWidget::OnUpgradeBegin(class UKRVTowerUpgradeDataAsset* NewTowerUpgradeDA, class AKRVTower* NewTower)
{
	if (!NewTower || !NewTowerUpgradeDA || IconType != EIconType::TowerUpgrade)
	{
		KRVCHECK(NewTower);
		KRVCHECK(NewTowerUpgradeDA);
		KRVCHECK(IconType != EIconType::TowerUpgrade);
	}
	else if (NewTower == BoundTower && NewTowerUpgradeDA == BoundTowerUpgradeDA)
	{
		OnRemainingProgressNotified(NewTowerUpgradeDA->BuildTime, NewTowerUpgradeDA->BuildTime);
	}

}

void UKRVIconUserWidget::OnUpgradeFinished(class UKRVTowerUpgradeDataAsset* NewTowerUpgradeDA, class AKRVTower* NewTower)
{
	if (!NewTower || !NewTowerUpgradeDA || IconType != EIconType::TowerUpgrade)
	{
		KRVCHECK(NewTower);
		KRVCHECK(NewTowerUpgradeDA);
		KRVCHECK(IconType != EIconType::TowerUpgrade);
	}
	else if (NewTower == BoundTower && NewTowerUpgradeDA == BoundTowerUpgradeDA)
	{
		OnRemainingProgressNotified(0.f, NewTowerUpgradeDA->BuildTime);
	}

}

void UKRVIconUserWidget::OnSellingBegin(class AKRVTower* NewTower)
{
	if (!NewTower || IconType != EIconType::TowerSelling)
	{
		KRVCHECK(NewTower);
		KRVCHECK(IconType == EIconType::TowerSelling);
	}
	else if (NewTower == BoundTower)
	{
		OnRemainingProgressNotified(NewTower->GetSellingTime(), NewTower->GetSellingTime());
	}

}

void UKRVIconUserWidget::OnSellingFinished(class AKRVTower* NewTower)
{
	if (!NewTower || IconType != EIconType::TowerSelling)
	{
		KRVCHECK(NewTower);
		KRVCHECK(IconType == EIconType::TowerSelling);
	}
	else if (NewTower == BoundTower)
	{
		OnRemainingProgressNotified(0.f, NewTower->GetSellingTime());
	}

}

bool UKRVIconUserWidget::BindToHero(class AKRVHero* NewHero)
{
	if (!NewHero || !IconButton)
	{
		KRVCHECK(IconButton);
		KRVCHECK(NewHero);
	}
	else
	{
		BoundHero = NewHero;
		SetIconType(EIconType::Hero);
		SetIconStatus(EIconStatus::Normal);
		BindToDataAsset(NewHero->GetDataAsset_Implementation(), false);

		// Icon
		AKRVInGamePlayerState* KRVInGamePlayerState = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerState(GetWorld());
		if (IntBorder && IntTextBlock && KRVInGamePlayerState)
		{
			IntBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
			OnLevelChanged(KRVInGamePlayerState->GetHeroLevel());
			KRVInGamePlayerState->OnLevelUpDelegate.AddUObject(this, &UKRVIconUserWidget::OnLevelChanged);
			bIsBound = true;
		}

		// Tooltip
		BindTooltipByIconType();

		NewHero->ReviveTimeNotificationDelegate.AddUObject(this, &UKRVIconUserWidget::OnReviveTimeNotified);
		NewHero->OnSelectedDelegate.AddUObject(this, &UKRVIconUserWidget::OnUnitSelected);
		return true;
	}
	return false;

}

void UKRVIconUserWidget::OnReviveTimeNotified(float RemainingTime, float TotalTime)
{
	OnRemainingProgressNotified(RemainingTime, TotalTime);

}

void UKRVIconUserWidget::OnUnitSelected(bool bIsSelected, class AKRVCombatUnit* NewUnit)
{
	switch (IconType)
	{
	case EIconType::Hero:
	{
		AKRVHero* NewHero = Cast<AKRVHero>(NewUnit);
		if (NewHero && BoundHero == NewHero)
		{
			OnHighlightedEffect(bIsSelected);
		}
		break;
	}
	default:
		break;
	}

}

void UKRVIconUserWidget::OnLevelChanged(int32 NewLevel)
{
	if (IntTextBlock)
	{
		IntTextBlock->SetText(FText::FromString(FString::FromInt(NewLevel)));
		OnLevelChanged_BP(NewLevel);
	}
	if (bIsBound)
	{
		OnProgressFinishedEffect(EIconType::TowerUpgrade);
	}

}

bool UKRVIconUserWidget::SetCustomMenuIcon(EIconTextureVariation NewVariation, const FText& NewTitleText /*= FText::GetEmpty()*/, const FText& NewTextText /*= FText::GetEmpty()*/, const FText& NewWarningText /*= FText::GetEmpty()*/)
{
	if (!IconButton)
	{
		KRVCHECK(IconButton);
	}
	else
	{
		bool bBindingResult = true;
		SetIconType(EIconType::Menu);

		// Icon
		SetIconTextureVariation(NewVariation);
		SetIconStatus(EIconStatus::Normal);
		if (IntBorder)
		{
			IntBorder->SetVisibility(ESlateVisibility::Hidden);
		}

		// Tooltip
		CustomTitleText = NewTitleText;
		CustomTextText = NewTextText;
		CustomWarningText = NewWarningText;
		BindTooltipByIconType();
		return true;
	}
	return false;

}

void UKRVIconUserWidget::ClearCustomMenuTexts()
{
	CustomTitleText = FText::GetEmpty();
	CustomTextText = FText::GetEmpty();
	CustomWarningText = FText::GetEmpty();

}

bool UKRVIconUserWidget::BindToGateway(AKRVGateway* NewGateway)
{
	if (!NewGateway)
	{
		KRVCHECK(NewGateway);
	}
	else
	{
		BoundGateway = NewGateway;
		SetIconType(EIconType::Gateway);
		SetIconTextureVariation(EIconTextureVariation::Default);
		SetIconStatus(EIconStatus::Normal);

		SetVisibility(ESlateVisibility::Hidden);

		//FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		//Streamable.RequestAsyncLoad(WaveCallSoundPath, FStreamableDelegate::CreateUObject(this,
		//	&UKRVIconUserWidget::WaveCallSoundAsyncLoaded));

		if (GetWorld())
		{
			BoundGateway->OnNextWaveCalledWidgetDelegate.BindDynamic(this, &UKRVIconUserWidget::OnNextWaveCalled);
			BoundGateway->OnNextWaveIncomingWidgetDelegate.BindDynamic(this, &UKRVIconUserWidget::OnNextWaveIncoming);
			BoundGateway->RequestNextWaveCallDelegate.AddUObject(this, &UKRVIconUserWidget::OnNextWaveCallRequested);
		}

		// Tooltip
		BindTooltipByIconType();
		return true;
	}
	return false;

}

void UKRVIconUserWidget::OnNextWaveCalled_Implementation()
{
	BoundGateway->RequestToShowNextWavePaths(false);

}

void UKRVIconUserWidget::OnNextWaveIncoming_Implementation(float CallTimeLimit /*= -1.f*/)
{
	SetIconTextureVariation(EIconTextureVariation::Default);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	OnRemainingProgressNotified(CallTimeLimit, CallTimeLimit);
	BindTooltipByIconType();

}

void UKRVIconUserWidget::OnNextWaveCallRequested()
{
	OnRemainingProgressNotified(0.f, TotalProgress);
	OnProgressFinishedEffect();

}

bool UKRVIconUserWidget::GetItemName(FName& OutName) const
{
	if (!bIsBound || BoundItemName == FName())
	{
		return false;
	}
	else
	{
		OutName = BoundItemName;
		return true;
	}

}

bool UKRVIconUserWidget::SetItemVacant()
{
	if (!IconButton)
	{
		KRVCHECK(IconButton);
	}
	else
	{
		bIsBound = false;
		SetCustomMenuIcon(EIconTextureVariation::Menu);
		SetIconStatus(EIconStatus::DisabledAndNotHovered);

		bIsInInventorySample = false;
		BoundItemName = FName();
		bIsItemEquipped = false;
		bIsItemUnlocked = false;
		bIsItemRecentlyUnlocked = false;

		if (IntBorder)
		{
			IntBorder->SetVisibility(ESlateVisibility::Hidden);
		}
		if (ExclamationSizeBox)
		{
			ExclamationSizeBox->SetVisibility(ESlateVisibility::Hidden);
		}
		return true;
	}
	return false;

}

bool UKRVIconUserWidget::BindToHeroInMap(UKRVHeroDataAsset* HeroDAToBind, const FKRVHeroSaveData& HeroSaveDataToBind, bool bWouldBeInInventorySample/* = false*/)
{
	bIsBound = false;
	if (!IconButton)
	{
		KRVCHECK(IconButton);
	}
	else if (!HeroDAToBind)
	{
		KRVCHECK(HeroDAToBind);
		SetCustomMenuIcon(EIconTextureVariation::Locked);
		SetIconStatus(EIconStatus::DisabledAndNotHovered);
	}
	else
	{
		bIsBound = true;
		SetIconType(EIconType::HeroInMap);
		BindToDataAsset(Cast<UKRVDataAsset>(HeroDAToBind), false);
		BoundItemName = HeroSaveDataToBind.HeroName;
		bIsItemUnlocked = HeroSaveDataToBind.bIsUnlocked;
		bIsItemRecentlyUnlocked = HeroSaveDataToBind.bIsRecentlyUpdated;
		bIsInInventorySample = bWouldBeInInventorySample;

		if (bIsItemUnlocked)
		{
			SetIconStatus(EIconStatus::Normal);
			if (IntBorder && IntTextBlock)
			{
				IntBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
				IntTextBlock->SetText(FText::FromString(FString::FromInt(HeroSaveDataToBind.Level)));
			}
			else
			{
				bIsBound = false;
			}

			if (GetWorld())
			{
				AKRVMapPlayerState* KRVMapPlayerState = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerState(GetWorld());
				if (KRVMapPlayerState)
				{
					if (KRVMapPlayerState->IsPickedHero(BoundItemName))
					{
						OnHeroPicked(BoundItemName);
					}
					KRVMapPlayerState->OnHeroPickedDelegate.AddUObject(this, &UKRVIconUserWidget::OnHeroPicked);
				}
			}

			if (bIsItemRecentlyUnlocked && ExclamationSizeBox)
			{
				ExclamationSizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				ExclamationSizeBox->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			SetIconStatus(EIconStatus::Disabled);
		}

		// Tooltip
		bIsBound &= BindTooltipByIconType();
		return bIsBound;
	}
	return false;

}

void UKRVIconUserWidget::OnHeroPicked(const FName& PickedHero)
{
	if (BoundItemName == PickedHero)
	{
		if (!bIsInInventorySample)
		{
			OnHighlightedEffect(true);
		}
		bIsItemEquipped = true;
	}
	else
	{
		OnHighlightedEffect(false);
		bIsItemEquipped = false;
	}

}

bool UKRVIconUserWidget::BindToItemInMap(class UKRVItemDataAsset* ItemDAToBind, const FKRVItemStockData& ItemStockDataToBind, bool bWouldBeInInventorySample /*= false*/)
{
	bIsBound = false;
	if (!IconButton)
	{
		KRVCHECK(IconButton);
	}
	else if (!ItemDAToBind)
	{
		KRVCHECK(ItemDAToBind);
		SetCustomMenuIcon(EIconTextureVariation::Locked);
		SetIconStatus(EIconStatus::DisabledAndNotHovered);
	}
	else
	{
		bIsBound = true;
		SetIconType(EIconType::ItemInMap);
		BindToDataAsset(Cast<UKRVDataAsset>(ItemDAToBind), false);
		BoundItemName = ItemStockDataToBind.ItemName;
		bIsItemUnlocked = ItemStockDataToBind.bIsUnlocked;
		bIsItemRecentlyUnlocked = ItemStockDataToBind.bIsRecentlyUnlocked;
		BoundItemDataAsset = ItemDAToBind;
		bIsInInventorySample = bWouldBeInInventorySample;

		if (bIsItemUnlocked)
		{
			AKRVMapPlayerState* KRVMapPlayerState = nullptr;
			if (GetWorld())
			{
				KRVMapPlayerState = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerState(GetWorld());
			}

			SetIconStatus(EIconStatus::Normal);
			if (KRVMapPlayerState)
			{
				if (KRVMapPlayerState->IsEquippedItem(ItemStockDataToBind.ItemName))
				{
					bIsItemEquipped = true;
					OnItemEquipped(ItemStockDataToBind.ItemName);
				}
				else
				{
					bIsItemEquipped = false;
				}
				KRVMapPlayerState->OnItemEquippedDelegate.AddUObject(this, &UKRVIconUserWidget::OnItemEquipped);
				KRVMapPlayerState->OnItemUnequippedDelegate.AddUObject(this, &UKRVIconUserWidget::OnItemUnequipped);
			}

			if (IntBorder)
			{
				IntBorder->SetVisibility(ESlateVisibility::Hidden);
			}

			if (bIsItemRecentlyUnlocked && ExclamationSizeBox)
			{
				ExclamationSizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				ExclamationSizeBox->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			SetIconStatus(EIconStatus::Disabled);
		}

		// Tooltip
		bIsBound &= BindTooltipByIconType();
		return bIsBound;
	}
	return false;

}

bool UKRVIconUserWidget::BindToSkillInMap(TSubclassOf<class AKRVSkill> SkillClassToBind, const FName& HeroToBind)
{
	if (!SkillClassToBind)
	{
		KRVCHECK(SkillClassToBind);
		SetCustomMenuIcon(EIconTextureVariation::Locked);
		SetIconStatus(EIconStatus::DisabledAndNotHovered);
		bIsBound = false;
	}
	else
	{
		bIsBound = true;
		SetIconType(EIconType::SkillInMap);
		SetIconStatus(EIconStatus::Disabled);
		BoundItemName = HeroToBind;
		BoundSkillClass = SkillClassToBind;
		BoundSkill = SkillClassToBind.GetDefaultObject();

		if (BoundSkill)
		{
			BoundDataAsset = BoundSkill->GetDataAsset_Implementation();
		}
		bIsBound &= BindToDataAsset(BoundDataAsset, false);

		if (IntBorder)
		{
			IntBorder->SetVisibility(ESlateVisibility::Hidden);
		}

		return bIsBound;
	}
	return false;

}

bool UKRVIconUserWidget::OnPointGivenToHeroSkill(int32 NewSkillPoint, class UKRVDataAsset* NewDA, const FName& NewHeroName)
{
	if (NewDA)
	{
		KRVCHECK(NewDA);
	}
	else if (BoundDataAsset == NewDA)
	{
		if (!BoundSkill && BoundSkillClass)
		{
			BoundSkillClass.GetDefaultObject();
		}

		if (!BoundSkill)
		{
			KRVCHECK(BoundSkill);
		}
		else
		{
			BoundSkill->SetMasteryLevel(NewSkillPoint);
			BindTooltipByIconType();
			return true;
		}
	}
	return false;

}

bool UKRVIconUserWidget::BindToSkillLearning(class UKRVDataAsset* SkillDAToBind, const FName& HeroToBind)
{
	bIsBound = false;
	if (!SkillDAToBind)
	{
		KRVCHECK(SkillDAToBind);
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		bIsBound = true;
		SetIconType(EIconType::SkillLearning);
		BoundItemName = HeroToBind;
		BoundDataAsset = SkillDAToBind;

		if (IntBorder)
		{
			IntBorder->SetVisibility(ESlateVisibility::Hidden);
		}

		SetIconTextureVariation(EIconTextureVariation::Default);
		SetIconStatus(EIconStatus::Normal);
		return true;
	}
	return false;

}

void UKRVIconUserWidget::OnItemEquipped(const FName& ItemEquipped)
{
	if (BoundItemName == ItemEquipped)
	{
		if (!bIsInInventorySample)
		{
			OnHighlightedEffect(true);
		}
		bIsItemEquipped = true;
	}

}

void UKRVIconUserWidget::OnItemUnequipped(const FName& ItemUnequipped)
{
	if (BoundItemName == ItemUnequipped)
	{
		OnHighlightedEffect(false);
		bIsItemEquipped = false;

		if (bIsInInventorySample)
		{
			SetItemVacant();
		}
	}

}

bool UKRVIconUserWidget::BindToItemPurchase(UKRVItemDataAsset* ItemDAToBind, const FKRVItemStockData& ItemStockDataToBind)
{
	bIsBound = false;
	if (!IconButton)
	{
		KRVCHECK(IconButton);
		SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		bIsBound = true;
		SetIconType(EIconType::ItemPurchase);
		BoundItemName = ItemStockDataToBind.ItemName;
		BoundItemDataAsset = ItemDAToBind;

		if (IntBorder)
		{
			IntBorder->SetVisibility(ESlateVisibility::Hidden);
		}

		if (ItemStockDataToBind.bIsUnlocked)
		{
			SetIconTextureVariation(EIconTextureVariation::Default);
			SetIconStatus(EIconStatus::Normal);
		}
		else
		{
			SetIconTextureVariation(EIconTextureVariation::Locked);
			SetIconStatus(EIconStatus::Disabled);
			SetVisibility(ESlateVisibility::Hidden);
		}
		return true;
	}
	return false;

}

bool UKRVIconUserWidget::BindToTowerInMap(class UKRVTowerDataAsset* TowerDAToBind, const FKRVTowerSaveData& TowerSaveDataToBind, bool bWouldBeInInventorySample /*= false*/)
{
	bIsBound = false;
	if (!IconButton)
	{
		KRVCHECK(IconButton);
	}
	else if (!TowerDAToBind)
	{
		KRVCHECK(TowerDAToBind);
		SetCustomMenuIcon(EIconTextureVariation::Locked);
		SetIconStatus(EIconStatus::DisabledAndNotHovered);
	}
	else
	{
		bIsBound = true;
		SetIconType(EIconType::TowerInMap);
		BindToDataAsset(Cast<UKRVDataAsset>(TowerDAToBind), false);
		bIsItemUnlocked = TowerSaveDataToBind.bIsUnlocked;
		bIsItemRecentlyUnlocked = TowerSaveDataToBind.bIsRecentlyUnlocked;
		BoundItemName = TowerSaveDataToBind.TowerName;
		bIsInInventorySample = bWouldBeInInventorySample;

		if (IntBorder)
		{
			IntBorder->SetVisibility(ESlateVisibility::Hidden);
		}

		if (bIsItemUnlocked)
		{
			AKRVMapPlayerState* KRVMapPlayerState = nullptr;
			if (GetWorld())
			{
				KRVMapPlayerState = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerState(GetWorld());
			}

			SetIconStatus(EIconStatus::Normal);
			if (KRVMapPlayerState)
			{
				if (KRVMapPlayerState->IsEquippedTower(TowerSaveDataToBind.TowerName))
				{
					bIsItemEquipped = true;
					OnItemEquipped(TowerSaveDataToBind.TowerName);
				}
				else
				{
					bIsItemEquipped = false;
				}
				KRVMapPlayerState->OnTowerEquippedDelegate.AddUObject(this, &UKRVIconUserWidget::OnItemEquipped);
				KRVMapPlayerState->OnTowerUnequippedDelegate.AddUObject(this, &UKRVIconUserWidget::OnItemUnequipped);
			}

			if (bIsItemRecentlyUnlocked && ExclamationSizeBox)
			{
				ExclamationSizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				ExclamationSizeBox->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			SetIconStatus(EIconStatus::Disabled);
		}

		// Tooltip
		bIsBound &= BindTooltipByIconType();
		return bIsBound;
	}
	return false;

}

bool UKRVIconUserWidget::BindToBattlefield(UKRVLevelDataAsset* LevelDAToBind, const FKRVLevelSaveData& LevelSaveDataToBind)
{
	bIsBound = false;
	if (!LevelDAToBind)
	{
		KRVCHECK(LevelDAToBind);
	}
	else
	{
		bIsBound = true;
		SetIconType(EIconType::Battlefield);
		BoundItemName = *(LevelSaveDataToBind.LevelString);
		bIsItemUnlocked = LevelSaveDataToBind.bIsUnlocked;
		bIsItemRecentlyUnlocked = LevelSaveDataToBind.bIsRecentlyUnlocked;

		if (bIsItemUnlocked)
		{
			SetIconTextureVariation(EIconTextureVariation::Default);
			SetIconStatus(EIconStatus::Normal);
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			SetIconTextureVariation(EIconTextureVariation::Locked);
			SetIconStatus(EIconStatus::DisabledAndNotHovered);
			SetVisibility(ESlateVisibility::Hidden);
		}

		bIsBound &= BindToDataAsset(Cast<UKRVDataAsset>(LevelDAToBind), false);
		bIsBound &= BindTooltipByIconType();

		return bIsBound;
	}
	return false;

}
