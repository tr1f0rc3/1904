// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVUnitInfoUserWidget.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/Texture2D.h"

#include "WidgetBlueprintLibrary.h"
#include "TextBlock.h"
#include "Image.h"
#include "ProgressBar.h"
#include "HorizontalBox.h"
#include "VerticalBox.h"

#include "ActorComponents/KRVStatComponent.h"
#include "ActorComponents/KRVCharacterStatComponent.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVInGamePlayerController.h"

void UKRVUnitInfoUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UKRVUnitInfoUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UnitNameTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_UnitName_1")));
	DamageBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HB_Damage_1")));
	DamageTypeImage = Cast<UImage>(GetWidgetFromName(TEXT("IM_DamageType_1")));
	DamageTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Damage_1")));
	RangeBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HB_Range_1")));
	RangeTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Range_1")));
	ArmorsBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("VB_Armors_1")));
	ArmorProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_Armor_1")));
	MRProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_MR_1")));

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(TrueDamageTexturePath, FStreamableDelegate::CreateUObject(this,
		&UKRVUnitInfoUserWidget::OnTrueDamageTextureAsyncLoaded));
	Streamable.RequestAsyncLoad(MagicDamageTexturePath, FStreamableDelegate::CreateUObject(this,
		&UKRVUnitInfoUserWidget::OnMagicDamageTextureAsyncLoaded));
	Streamable.RequestAsyncLoad(PhysicalDamageTexturePath, FStreamableDelegate::CreateUObject(this,
		&UKRVUnitInfoUserWidget::OnPhysicalDamageTextureAsyncLoaded));

	SetVisibility(ESlateVisibility::Hidden);

}

void UKRVUnitInfoUserWidget::OnTrueDamageTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(TrueDamageTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		TrueDamageTexture = AssetPathPtr.Get();
	}

}

void UKRVUnitInfoUserWidget::OnPhysicalDamageTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(PhysicalDamageTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		PhysicalDamageTexture = AssetPathPtr.Get();
	}

}

void UKRVUnitInfoUserWidget::OnMagicDamageTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(MagicDamageTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		MagicDamageTexture = AssetPathPtr.Get();
	}

}

void UKRVUnitInfoUserWidget::OnUnitSelected(bool bIsSelected, class AKRVCombatUnit* SelectedUnit)
{
	if (!SelectedUnit)
	{
		KRVCHECK(SelectedUnit);
	}
	else if (!bIsSelected)
	{
		HideThisWidget();
	}
	else
	{
		{ // Set name
			UKRVDataAsset* KRVDataAsset = SelectedUnit->GetDataAsset_Implementation();
			if (!UnitNameTextBlock)
			{
				KRVCHECK(UnitNameTextBlock);
			}
			else if (!KRVDataAsset)
			{
				KRVCHECK(KRVDataAsset);
				UnitNameTextBlock->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				SetUnitName(KRVDataAsset->ClassFText);
				UnitNameTextBlock->SetVisibility(ESlateVisibility::Visible);
			}
		}

		UKRVStatComponent* KRVStatComponent = SelectedUnit->GetModifiedStat();
		if (!KRVStatComponent)
		{
			KRVCHECK(KRVStatComponent);

			if (DamageBox)
			{
				DamageBox->SetVisibility(ESlateVisibility::Hidden);
			}
			if (RangeBox)
			{
				RangeBox->SetVisibility(ESlateVisibility::Hidden);
			}
			if (ArmorsBox)
			{
				ArmorsBox->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			AKRVCharacter* SelectedCharacter = Cast<AKRVCharacter>(SelectedUnit);
			UKRVCharacterStatComponent* KRVCharaceterStatComponent = nullptr;

			if (ArmorsBox)
			{
				if (SelectedCharacter) // Set armors
				{
					KRVCharaceterStatComponent = SelectedCharacter->GetModifiedCharacterStat();

					int32 ArmorToSet = KRVCharaceterStatComponent->DefenceStat.Armor;
					int32 MRToSet = KRVCharaceterStatComponent->DefenceStat.MagicResistance;
					SetArmors(ArmorToSet, MRToSet);

					ArmorsBox->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					ArmorsBox->SetVisibility(ESlateVisibility::Collapsed);
				}
			}

			{ // Set damage and range begin
				EDamageReductionType DamageReductionTypeToSet = EDamageReductionType::NONE;
				int32 MinDamageToSet = -1;
				int32 MaxDamageToSet = -1;
				bool bIsMeleeRange = false;
				int32 RangeToSet = -1;

				switch (KRVStatComponent->RangedMediumType)
				{
				case ERangedMediumType::NoWeapon:
				{
					if (SelectedCharacter)
					{
						DamageReductionTypeToSet =
							KRVCharaceterStatComponent->MeleeAttackStat.DamageType->
							GetDefaultObject<UKRVDamageType>()->DamageReductionType;
						MinDamageToSet = KRVCharaceterStatComponent->MeleeAttackStat.MinDamage;
						MaxDamageToSet = KRVCharaceterStatComponent->MeleeAttackStat.MaxDamage;
						bIsMeleeRange = true;
					}
					break;
				}
				case ERangedMediumType::Arrow:
				{
					DamageReductionTypeToSet =
						KRVStatComponent->RangedAttackStat.DamageType->
						GetDefaultObject<UKRVDamageType>()->DamageReductionType;
					MinDamageToSet = KRVStatComponent->RangedAttackStat.MinDamage;
					MaxDamageToSet = KRVStatComponent->RangedAttackStat.MaxDamage;
					RangeToSet = KRVStatComponent->RangedAttackStat.AttackRange;
					break;
				}
				case ERangedMediumType::Bomb:
				{
					DamageReductionTypeToSet =
						KRVStatComponent->SplashAttackStat.DamageType->
						GetDefaultObject<UKRVDamageType>()->DamageReductionType;
					MinDamageToSet = KRVStatComponent->SplashAttackStat.MinDamage;
					MaxDamageToSet = KRVStatComponent->SplashAttackStat.MaxDamage;
					RangeToSet = KRVStatComponent->RangedAttackStat.AttackRange;
					break;
				}
				case ERangedMediumType::Mine:
				{
					DamageReductionTypeToSet =
						KRVStatComponent->SplashAttackStat.DamageType->
						GetDefaultObject<UKRVDamageType>()->DamageReductionType;
					MinDamageToSet = KRVStatComponent->SplashAttackStat.MinDamage;
					MaxDamageToSet = KRVStatComponent->SplashAttackStat.MaxDamage;
					RangeToSet = KRVStatComponent->SplashAttackStat.AttackRange;
					break;
				}
				default:
					break;

				}

				if (!DamageBox)
				{
					KRVCHECK(DamageBox);
				}
				else if (MaxDamageToSet > 0 && MinDamageToSet >= 0)
				{
					SetDamage(DamageReductionTypeToSet, MinDamageToSet, MaxDamageToSet);
					DamageBox->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					DamageBox->SetVisibility(ESlateVisibility::Collapsed);
				}

				if (!RangeBox)
				{
					KRVCHECK(RangeBox);
				}
				else if (bIsMeleeRange || RangeToSet > 0)
				{
					if (bIsMeleeRange)
					{
						SetRangeToMelee();
					}
					else
					{
						SetRange(RangeToSet);
					}

					RangeBox->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					RangeBox->SetVisibility(ESlateVisibility::Collapsed);
				}
			} // Set damage and range end
		}

		if (UnitNameTextBlock && UnitNameTextBlock->GetVisibility() == ESlateVisibility::Visible ||
			DamageBox && DamageBox->GetVisibility() == ESlateVisibility::Visible ||
			RangeBox && RangeBox->GetVisibility() == ESlateVisibility::Visible ||
			ArmorsBox && ArmorsBox->GetVisibility() == ESlateVisibility::Visible)
		{
			ShowThisWidget();
		}
		else
		{
			HideThisWidget();
		}
	}

}

void UKRVUnitInfoUserWidget::ShowThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

}

void UKRVUnitInfoUserWidget::HideThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);

}

bool UKRVUnitInfoUserWidget::BindToPC(class AKRVInGamePlayerController* NewPC)
{
	if (!NewPC)
	{
		KRVCHECK(NewPC);
	}
	else
	{
		NewPC->OnUnitSelectedDelegate.AddUObject(this, &UKRVUnitInfoUserWidget::OnUnitSelected);
		return true;
	}
	return false;

}

bool UKRVUnitInfoUserWidget::SetUnitName(const FText& NewUnitName)
{
	if (!UnitNameTextBlock)
	{
		KRVCHECK(UnitNameTextBlock);
	}
	else if (NewUnitName.IsEmptyOrWhitespace())
	{
		UnitNameTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return true;
	}
	else
	{
		UnitNameTextBlock->SetText(NewUnitName);
		UnitNameTextBlock->SetVisibility(ESlateVisibility::Visible);
		return true;
	}
	return false;

}

bool UKRVUnitInfoUserWidget::SetDamage(EDamageReductionType NewDamageReductionType, int32 MinDamage, int32 MaxDamage)
{
	bool bDidSetDamage = true;
	if (!DamageTypeImage)
	{
		KRVCHECK(DamageTypeImage);
		bDidSetDamage = false;
	}
	else
	{
		DamageTypeImage->SetVisibility(ESlateVisibility::Visible);

		switch (NewDamageReductionType)
		{
		case EDamageReductionType::Physical:
		{
			if (PhysicalDamageTexture)
			{
				UWidgetBlueprintLibrary::SetBrushResourceToTexture(DamageTypeImage->Brush, PhysicalDamageTexture);
			}
			break;
		}
		case EDamageReductionType::Magic:
		{
			if (MagicDamageTexture)
			{
				UWidgetBlueprintLibrary::SetBrushResourceToTexture(DamageTypeImage->Brush, MagicDamageTexture);
			}
			break;
		}
		case EDamageReductionType::TrueDamage:
		{
			if (TrueDamageTexture)
			{
				UWidgetBlueprintLibrary::SetBrushResourceToTexture(DamageTypeImage->Brush, TrueDamageTexture);
			}
			break;
		}
		default:
		{
			DamageTypeImage->SetVisibility(ESlateVisibility::Hidden);
			bDidSetDamage = false;
			break;
		}
		}
	}

	if (!DamageTextBlock)
	{
		KRVCHECK(DamageTextBlock);
		bDidSetDamage = false;
	}
	else if (MinDamage == 0 && MaxDamage == 0)
	{
		DamageTextBlock->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		DamageTextBlock->SetVisibility(ESlateVisibility::Visible);

		if (MinDamage > MaxDamage)
		{
			Swap(MinDamage, MaxDamage);
		}

		FString DamageString = FString::FromInt(MinDamage) + FString(TEXT("-")) + FString::FromInt(MaxDamage);
		DamageTextBlock->SetText(FText::FromString(DamageString));
	}

	return bDidSetDamage;

}

bool UKRVUnitInfoUserWidget::SetRange(int32 NewRange)
{
	if (!RangeTextBlock)
	{
		KRVCHECK(RangeTextBlock);
	}
	else
	{
		RangeTextBlock->SetText(FText::FromString(FString::FromInt(NewRange)));
		return true;
	}
	return false;

}

bool UKRVUnitInfoUserWidget::SetRangeToMelee()
{
	if (!RangeTextBlock)
	{
		KRVCHECK(RangeTextBlock);
	}
	else
	{
		RangeTextBlock->SetText(FText::FromString(MeleeString));
		return true;
	}
	return false;

}

bool UKRVUnitInfoUserWidget::SetArmors(int32 NewArmor, int32 NewMR)
{
	float PBRatio = 0.f;
	bool bDidSetArmors = true;

	if (!ArmorProgressBar)
	{
		KRVCHECK(ArmorProgressBar);
		bDidSetArmors = false;
	}
	else
	{
		PBRatio = FMath::Clamp((float(NewArmor) / 100.f), 0.f, 1.f);
		ArmorProgressBar->SetPercent(PBRatio);
	}

	if (!MRProgressBar)
	{
		KRVCHECK(MRProgressBar);
		bDidSetArmors = false;
	}
	else
	{
		PBRatio = FMath::Clamp((float(NewMR) / 100.f), 0.f, 1.f);
		MRProgressBar->SetPercent(PBRatio);
	}

	return bDidSetArmors;

}
