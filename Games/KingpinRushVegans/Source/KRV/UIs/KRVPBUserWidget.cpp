// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVPBUserWidget.h"
#include "ProgressBar.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "ActorComponents\KRVCharacterStatComponent.h"
#include "PlayerStates/KRVInGamePlayerState.h"
#include "Pawns/KRVCombatUnit.h"
#include "Actors/KRVSkill.h"

void UKRVPBUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UKRVPBUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_1")));

}

void UKRVPBUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PBType == EPBType::Casting && bIsCasting)
	{
		if (ElapsedCastingTime <= TotalCastingTime)
		{
			ElapsedCastingTime += InDeltaTime;
			ProgressBar->SetPercent(FMath::Clamp(ElapsedCastingTime / TotalCastingTime, 0.f, 1.f));
		}
		else
		{
			OnCastingEnd(BoundSkill, BoundCaster);
		}
	}

}

bool UKRVPBUserWidget::BindToHealth(class UKRVCharacterStatComponent* NewComponent)
{
	if (!NewComponent || !ProgressBar)
	{
		KRVCHECK(ProgressBar);
		KRVCHECK(NewComponent);
	}
	else
	{
		PBType = EPBType::HP;
		NewComponent->OnHPChangedDelegate.AddUObject(this, &UKRVPBUserWidget::OnHPChanged);
		OnHPChanged(NewComponent->GetHealth(), NewComponent->DefenceStat.MaxHealth);
		return true;
	}
	return false;

}

void UKRVPBUserWidget::OnHPChanged(float CurrentHealth, float MaxHealth)
{
	if (!ProgressBar)
	{
		KRVCHECK(ProgressBar);
	}
	else
	{
		ProgressBar->SetPercent(FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f));
		if (CurrentHealth == 0 || CurrentHealth == MaxHealth)
		{
			NeedToShowDelegate.ExecuteIfBound(false);
		}
		else
		{
			NeedToShowDelegate.ExecuteIfBound(true);
		}
	}

}

bool UKRVPBUserWidget::BindToExp(AKRVInGamePlayerState* NewPlayerState /*= nullptr*/)
{
	if (!NewPlayerState)
	{
		NewPlayerState = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerState(GetWorld());
	}

	if (!NewPlayerState || !ProgressBar)
	{
		KRVCHECK(ProgressBar);
		KRVCHECK(NewPlayerState);
	}
	else
	{
		PBType = EPBType::Exp;
		NewPlayerState->OnExpChangeDelegate.AddUObject(this, &UKRVPBUserWidget::OnExpChanged);
		OnExpChanged(NewPlayerState->GetCurrentExp(), NewPlayerState->GetExpToLevelUp());
		return true;
	}
	return false;

}

void UKRVPBUserWidget::OnExpChanged(int32 NewExp, int32 ExpToLevelUp)
{
	if (!ProgressBar)
	{
		KRVCHECK(ProgressBar);
	}
	else
	{
		ProgressBar->SetPercent(FMath::Clamp(float(NewExp) / float(ExpToLevelUp), 0.f, 1.f));
	}

}

bool UKRVPBUserWidget::BindToCasting(AKRVCombatUnit* NewCaster)
{
	if (!NewCaster || !ProgressBar)
	{
		KRVCHECK(ProgressBar);
		KRVCHECK(NewCaster);
	}
	else
	{
		PBType = EPBType::Casting;
		NewCaster->OnSkillCastBeginDelegate.AddUObject(this, &UKRVPBUserWidget::OnCastingBegin);
		NewCaster->OnSkillCastCancleDelegate.AddUObject(this, &UKRVPBUserWidget::OnCastingCancle);
		NewCaster->OnSkillCastEndDelegate.AddUObject(this, &UKRVPBUserWidget::OnCastingEnd);
		return true;
	}
	return false;

}

void UKRVPBUserWidget::OnCastingBegin(AKRVSkill* NewSkill, AKRVCombatUnit* NewCaster)
{
	if (!NewSkill || !NewCaster)
	{
		KRVCHECK(NewSkill);
		KRVCHECK(NewCaster);
	}
	else
	{
		bIsCasting = true;
		BoundCaster = NewCaster;
		BoundSkill = NewSkill;

		OnCastingBeginEffect();
		ElapsedCastingTime = 0.f;
		TotalCastingTime = NewCaster->GetAttackRemainingTime();
		ProgressBar->SetPercent(FMath::Clamp(ElapsedCastingTime / TotalCastingTime, 0.f, 1.f));

		NeedToShowDelegate.ExecuteIfBound(true);
	}
	
}

void UKRVPBUserWidget::OnCastingCancle(AKRVSkill* NewSkill, AKRVCombatUnit* NewCaster)
{
	if (BoundSkill == NewSkill && BoundCaster == NewCaster)
	{
		bIsCasting = false;
		OnCastingCancleEffect();
	}

}

void UKRVPBUserWidget::OnCastingEnd(AKRVSkill* NewSkill, AKRVCombatUnit* NewCaster)
{
	if (BoundSkill == NewSkill && BoundCaster == NewCaster && bIsCasting)
	{
		bIsCasting = false;
		ProgressBar->SetPercent(1.f);
		OnCastingEndEffect();
	}

}
