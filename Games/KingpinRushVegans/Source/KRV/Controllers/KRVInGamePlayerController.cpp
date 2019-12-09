// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVInGamePlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "WidgetBlueprintLibrary.h"
#include "Camera/PlayerCameraManager.h"
#include "UserWidget.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Actors/KRVSkill.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVHero.h"
#include "Pawns/KRVTower.h"
#include "GameModes/KRVInGameGameMode.h"
#include "SaveGames/KRVGameSetting.h"

#include "UIs/KRVInGameMainUserWidget.h"
#include "UIs/KRVIconUserWidget.h"
#include "UIs/KRVGameResultUserWidget.h"

AKRVInGamePlayerController::AKRVInGamePlayerController()
{
	static ConstructorHelpers::FClassFinder<UKRVInGameMainUserWidget> IN_GAME_MAIN_UI_C
	(TEXT("/Game/Resources/UI/UI_InGameMain.UI_InGameMain_C"));
	if (IN_GAME_MAIN_UI_C.Succeeded())
	{
		InGameMainWidgetClass = IN_GAME_MAIN_UI_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UKRVGameResultUserWidget> GAME_RESULT_UI_C
	(TEXT("/Game/Resources/UI/UI_GameResult.UI_GameResult_C"));
	if (GAME_RESULT_UI_C.Succeeded())
	{
		GameResultWidgetClass = GAME_RESULT_UI_C.Class;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ON_STATIC_CLICKED_PARTICLE_OBJ
	(TEXT("/Game/InfinityBladeEffects/Effects/FX_Cines/cinema03/P_CIN_Dust_drag_01.P_CIN_Dust_drag_01"));
	if (ON_STATIC_CLICKED_PARTICLE_OBJ.Succeeded())
	{
		OnStaticClickedParticle = ON_STATIC_CLICKED_PARTICLE_OBJ.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MOVE_TO_LOCATION_PARTICLE_OBJ
	(TEXT("/Game/InfinityBladeEffects/Effects/FX_Ability/Defense/P_Shield_Spawn.P_Shield_Spawn"));
	if (MOVE_TO_LOCATION_PARTICLE_OBJ.Succeeded())
	{
		ConfirmToLocationParticle = MOVE_TO_LOCATION_PARTICLE_OBJ.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> MOVE_TO_LOCATION_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Jumping/Cues/retro_jump_bounce_12_Cue.retro_jump_bounce_12_Cue"));
	if (MOVE_TO_LOCATION_SOUND_OBJ.Succeeded())
	{
		ConfirmToLocationSound = MOVE_TO_LOCATION_SOUND_OBJ.Object;
	}

}

void AKRVInGamePlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetWorld())
	{
		AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
		if (KRVInGameGameMode)
		{
			KRVInGameGameMode->OnGameStoppedAfterGameEndDelegate.AddUObject(this, &AKRVInGamePlayerController::OnGameStoppedAfterGameEnd);
		}
	}
}

void AKRVInGamePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

}

void AKRVInGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &AKRVInGamePlayerController::OnLMBClicked);

}

void AKRVInGamePlayerController::BeginPlay()
{
	Super::BeginPlay();
	// not implemented

}

void AKRVInGamePlayerController::RequestToStartGame()
{
	bShowMouseCursor = false;
	FadeInTheScreen(true);
	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(FadingTimer, this,
			&AKRVInGamePlayerController::StartGameAfterFadeIn, FadeDuration, false);
	}

	if (!InGameMainWidget && InGameMainWidgetClass)
	{
		InGameMainWidget = CreateWidget<UKRVInGameMainUserWidget>(this, InGameMainWidgetClass);
		if (InGameMainWidget)
		{
			InGameMainWidget->SetVisibility(ESlateVisibility::Hidden);
			AddWidgetFromViewport(InGameMainWidget, true, 1);
		}
	}

}

void AKRVInGamePlayerController::ReturnToMainMenu()
{
	FadeInTheScreen(false);
	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(FadingTimer, this,
			&AKRVInGamePlayerController::ReturnToMainMenuAfterFadeOut, FadeDuration, false);
	}

}

void AKRVInGamePlayerController::OnGameStoppedAfterGameEnd()
{
	ShowGameResultWidget();

}

void AKRVInGamePlayerController::StartGameAfterFadeIn()
{
	ShowInGameMainWidget(true);
	SetGameInputMode(true);
	if (GetWorld())
	{
		AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
		if (KRVInGameGameMode)
		{
			KRVInGameGameMode->CallIncoming();
		}
	}

}

void AKRVInGamePlayerController::ReturnToMainMenuAfterFadeOut()
{
	OpenLevelByName(MainMenuLevelName);

}

AKRVCombatUnit* AKRVInGamePlayerController::GetCurrentSelectedUnit() const
{
	if (CurrentSelectedUnit.IsValid())
	{
		return CurrentSelectedUnit.Get();
	}
	return nullptr;

}

void AKRVInGamePlayerController::OnLMBClicked()
{
	bool bDidClickWork = false;

	switch (ClickToDo)
	{
	case EClickToDo::SetTargetOfTargeter:
	{
		switch (TargetingType)
		{
		case ETargetingType::Target:
			bDidClickWork = MakeTargeterCastSkillToUnitUnderCursor();
			break;
		case ETargetingType::Location:
			bDidClickWork = MakeTargeterCastSkillToPointUnderCursor();
			break;
		default:
			break;
		}
		break;
	}
	default:
		bDidClickWork = SelectUnitUnderCursor();
		break;
	}

	if (!bDidClickWork)
	{
		switch (ClickToDo)
		{
		case EClickToDo::SetWaypoint:
			bDidClickWork = SetRecruitWaypoint();
			if (!bDidClickWork)
			{
				UnselectUnit();
			}
			break;
		case EClickToDo::MoveHero:
			bDidClickWork = MoveHeroToLocation();
			if (!bDidClickWork)
			{
				UnselectUnit();
			}
			break;
		default:
			break;
		}
	}

	if (!bDidClickWork)
	{
		ClickStaticUnderCursor();
	}

}

void AKRVInGamePlayerController::SetMouseCursorInTick()
{
	if (!GetWorld() || !bShowMouseCursor)
	{
		return;
	}
	else
	{
		Super::SetMouseCursorInTick();

		if (CurrentKRVCursor != EKRVCursor::Panning)
		{
			EKRVCursor NewKRVCursor = CurrentKRVCursor;

			if (InGameMainWidget && InGameMainWidget->IsHovered())
			{
				NewKRVCursor = EKRVCursor::Default;
			}
			else if (ClickToDo == EClickToDo::SetTargetOfTargeter)
			{
				NewKRVCursor = EKRVCursor::Targeting;
			}
			else
			{
				AKRVCombatUnit* UnitUnderCursor = GetUnitUnderCursor();
				if (UnitUnderCursor)
				{
					switch (UnitUnderCursor->GetTeam())
					{
					case EKRVTeam::Player:
					{
						NewKRVCursor = EKRVCursor::PlayerUnit;
						break;
					}
					case EKRVTeam::Enemy:
					{
						NewKRVCursor = EKRVCursor::EnemyUnit;
						break;
					}
					default:
						break;
					}
				}
			}

			CurrentKRVCursor = NewKRVCursor;
			switch (NewKRVCursor)
			{
			case EKRVCursor::Default:
				CurrentMouseCursor = EMouseCursor::Default;
				break;
			case EKRVCursor::PlayerUnit:
				CurrentMouseCursor = EMouseCursor::ResizeLeftRight;
				break;
			case EKRVCursor::EnemyUnit:
				CurrentMouseCursor = EMouseCursor::ResizeUpDown;
				break;
			case EKRVCursor::Targeting:
				CurrentMouseCursor = EMouseCursor::Crosshairs;
				break;
			default:
				break;
			}

		}

	}

}

bool AKRVInGamePlayerController::ShowInGameMainWidget(bool bIsTrue)
{
	if (!InGameMainWidget)
	{
		KRVCHECK(InGameMainWidget);
	}
	else
	{
		InGameMainWidget->ShowThisWidget(bIsTrue);
		return true;
	}
	return false;

}

bool AKRVInGamePlayerController::ShowGameResultWidget()
{
	if (!GameResultWidget && GameResultWidgetClass)
	{
		GameResultWidget = CreateWidget<UKRVGameResultUserWidget>(this, GameResultWidgetClass);
		if (GameResultWidget)
		{
			AddWidgetFromViewport(GameResultWidget, true, 4);
			GameResultWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (!GameResultWidget)
	{
		KRVCHECK(GameResultWidget);
	}
	else
	{
		GameResultWidget->BindToGameState();
		GameResultWidget->ShowThisWidget();
		return true;
	}
	return false;

}

bool AKRVInGamePlayerController::BindHeroToWidget(class AKRVHero* NewHero)
{
	if (!InGameMainWidget)
	{
		return false;
	}
	else
	{
		InGameMainWidget->BindToHero(NewHero);
		return true;
	}

}

bool AKRVInGamePlayerController::BindCommanderToWidget(class AKRVCombatUnit* NewCommander)
{
	if (!InGameMainWidget)
	{
		return false;
	}
	else
	{
		InGameMainWidget->BindToCommander(NewCommander);
		return true;
	}


}

AKRVCombatUnit* AKRVInGamePlayerController::GetUnitUnderCursor() const
{
	FHitResult HitResult;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQueries;
	ObjectTypeQueries.Emplace(EObjectTypeQuery::ObjectTypeQuery3);
	if (!GetHitResultUnderCursorForObjects(ObjectTypeQueries, false, HitResult))
	{
		return nullptr;
	}
	else
	{
		AKRVCombatUnit* KRVCombatUnit = Cast<AKRVCombatUnit>(HitResult.GetActor());
		return KRVCombatUnit;
	}

}

bool AKRVInGamePlayerController::GetPointUnderCursor(FVector& OutVector)
{
	FHitResult HitResult;
	if (!GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		return false;
	}
	else
	{
		OutVector = HitResult.Location;
		return true;
	}

}

bool AKRVInGamePlayerController::SelectUnitUnderCursor()
{
	return SelectUnit(GetUnitUnderCursor());

}

bool AKRVInGamePlayerController::SelectUnit(AKRVCombatUnit* KRVCombatUnit)
{
	if (KRVCombatUnit)
	{
		if (GetCurrentSelectedUnit())
		{
			GetCurrentSelectedUnit()->OnSelected(false);
		}
		CurrentSelectedUnit = KRVCombatUnit;
		KRVCombatUnit->OnSelected(true);

		AKRVHero* KRVHero = Cast<AKRVHero>(KRVCombatUnit);
		AKRVTower* KRVTower = Cast<AKRVTower>(KRVCombatUnit);
		if (KRVHero)
		{
			ClickToDo = EClickToDo::MoveHero;
		}
		else if (KRVTower && KRVTower->IsRecruitableTower())
		{
			ClickToDo = EClickToDo::SetWaypoint;
		}

		OnUnitSelectedDelegate.Broadcast(true, KRVCombatUnit);
		return true;
	}
	return false;

}

bool AKRVInGamePlayerController::UnselectUnit()
{
	ClickToDo = EClickToDo::SelectUnit;

	if (!GetCurrentSelectedUnit())
	{
		return false;
	}
	else
	{
		GetCurrentSelectedUnit()->OnSelected(false);
		OnUnitSelectedDelegate.Broadcast(false, GetCurrentSelectedUnit());
		CurrentSelectedUnit = nullptr;
		return true;
	}

}

bool AKRVInGamePlayerController::MoveHeroToLocation()
{
	AKRVHero* SelectedHero = Cast<AKRVHero>(GetCurrentSelectedUnit());
	if (!SelectedHero || !SelectedHero->IsAbleToMove())
	{
		return false;
	}
	else
	{
		FVector TargetPoint;
		if (!GetPointUnderCursor(TargetPoint))
		{
			return false;
		}
		else
		{
			SelectedHero->SetTargetPoint(TargetPoint);
			if (SelectedHero->MoveToTargetPoint(ECombatStatus::ForcedMoving))
			{
				PlayConfirmToLocationEffect(TargetPoint);
				return true;
			}
		}
	}
	return false;

}

void AKRVInGamePlayerController::PlayConfirmToLocationEffect(const FVector& LocationToPlay)
{
	if (GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ConfirmToLocationParticle,
			FTransform(FRotator::ZeroRotator, LocationToPlay, FVector::OneVector), true);
		UGameplayStatics::PlaySoundAtLocation(this, ConfirmToLocationSound, LocationToPlay, 1.f);
	}

}

bool AKRVInGamePlayerController::SetRecruitWaypoint()
{
	AKRVTower* SelectedTower = Cast<AKRVTower>(GetCurrentSelectedUnit());
	if (!SelectedTower || !SelectedTower->IsRecruitableTower())
	{
		return false;
	}
	else if (SelectedTower->GetActionStatus() == EActionStatus::Dead)
	{
		return false;
	}
	else
	{
		FHitResult HitResult;
		if (!GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
		{
			return false;
		}
		else if (SelectedTower->SetRecruitWaypoint(HitResult.Location))
		{
			PlayConfirmToLocationEffect(HitResult.Location);
			return true;
		}
	}
	return false;

}

bool AKRVInGamePlayerController::ClickStaticUnderCursor()
{
	FHitResult HitResult;
	if (!GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		return false;
	}
	else if (GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnStaticClickedParticle,
			FTransform(FRotator::ZeroRotator, HitResult.Location, FVector::OneVector), true);
		return true;
	}
	return false;

}

bool AKRVInGamePlayerController::TryCastSkill(const FGameplayTag& SkillTagToCast, class AKRVCombatUnit* Caster)
{
	if (!Caster)
	{
		KRVCHECK(Caster);
	}
	else
	{
		AKRVSkill* SkillToCast = Caster->GetSkillByTag(SkillTagToCast);
		if (!Caster->IsAbleToMove() || !SkillToCast)
		{
			KRVCHECK(SkillToCast);
		}
		else
		{
			ReservedTargeter = Caster;
			ReservedSkillTagToCast = SkillTagToCast;

			bool bIsAbleToCastWithoutRetargeting =
				(SkillToCast->GetTargetingType() == ETargetingType::Self) ||
				(SkillToCast->GetTargetingType() == ETargetingType::Target &&
					Caster->IsInSkillCastRange(SkillTagToCast));

			if (bIsAbleToCastWithoutRetargeting)
			{
				GetReservedTargeter()->CastSkill(SkillTagToCast);
			}
			else
			{
				BeginTargeting(SkillToCast->GetTargetingType());
			}
			return true;
		}
	}
	return false;

}

bool AKRVInGamePlayerController::IsTargeting() const
{
	if (ClickToDo == EClickToDo::SetTargetOfTargeter)
	{
		return true;
	}
	return false;

}

AKRVCombatUnit* AKRVInGamePlayerController::GetReservedTargeter() const
{
	if (ReservedTargeter.IsValid())
	{
		return ReservedTargeter.Get();
	}
	return nullptr;

}

bool AKRVInGamePlayerController::BeginTargeting(ETargetingType NewTargetingType)
{
	AKRVCombatUnit* HardPtr_ReservedTargeter = GetReservedTargeter();
	if (!HardPtr_ReservedTargeter)
	{
		return false;
	}
	else if (!(NewTargetingType == ETargetingType::Location || NewTargetingType == ETargetingType::Target) ||
		HardPtr_ReservedTargeter->GetActionStatus() == EActionStatus::Dead)
	{
		return false;
	}
	else
	{
		ClickToDo = EClickToDo::SetTargetOfTargeter;
		TargetingType = NewTargetingType;
		OnTargetingBeginDelegate.Broadcast(TargetingType);
		return true;
	}

}

bool AKRVInGamePlayerController::MakeTargeterCastSkillToUnitUnderCursor()
{
	bool bIsSkillCasted = false;

	AKRVCombatUnit* HardPtr_ReservedTargeter = GetReservedTargeter();
	if (!HardPtr_ReservedTargeter)
	{
		KRVCHECK(HardPtr_ReservedTargeter);
	}
	else if (HardPtr_ReservedTargeter->GetActionStatus() != EActionStatus::Dead)
	{
		AKRVCombatUnit* UnitUnderCursor = GetUnitUnderCursor();
		if (UnitUnderCursor)
		{
			if (HardPtr_ReservedTargeter->CastSkill(ReservedSkillTagToCast, UnitUnderCursor))
			{
				PlayConfirmToLocationEffect(UnitUnderCursor->GetActorLocation());
				bIsSkillCasted = true;
			}
		}
	}

	CancleTargeting();
	return bIsSkillCasted;

}

bool AKRVInGamePlayerController::MakeTargeterCastSkillToPointUnderCursor()
{
	bool bIsSkillCasted = false;

	AKRVCombatUnit* HardPtr_ReservedTargeter = GetReservedTargeter();
	if (!HardPtr_ReservedTargeter)
	{
		KRVCHECK(HardPtr_ReservedTargeter);
	}
	else if (HardPtr_ReservedTargeter->GetActionStatus() != EActionStatus::Dead)
	{
		FVector PointUnderCursor;
		bool bDidGetThePoint = GetPointUnderCursor(PointUnderCursor);
		if (bDidGetThePoint)
		{
			if (HardPtr_ReservedTargeter->CastSkill(ReservedSkillTagToCast, nullptr, PointUnderCursor))
			{
				PlayConfirmToLocationEffect(PointUnderCursor);
				bIsSkillCasted = true;
			}
		}
	}

	CancleTargeting();
	return bIsSkillCasted;

}

bool AKRVInGamePlayerController::CancleTargeting()
{
	if (ClickToDo != EClickToDo::SetTargetOfTargeter)
	{
		return false;
	}
	else
	{
		ClickToDo = EClickToDo::SelectUnit;
		if (ReservedTargeter.IsValid())
		{
			SelectUnit(ReservedTargeter.Get());
		}
		ReservedTargeter = nullptr;
		ReservedSkillTagToCast = FGameplayTag();
		OnTargetingEndDelegate.ExecuteIfBound();
		OnTargetingEndDelegate.Unbind();
		OnTargetingEndMultiDelegate.Broadcast();
		return false;
	}

}
