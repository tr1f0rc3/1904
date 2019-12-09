// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVEndOfPath.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Pawns/KRVCharacter.h"
#include "GameModes/KRVInGameGameMode.h"

AKRVEndOfPath::AKRVEndOfPath()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BOX_COMPONENT"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetCollisionProfileName("KRVEndOfPath");
	BoxComponent->InitBoxExtent(BoxComponentSize);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AKRVEndOfPath::OnKRVCharacterBeginOverlap);

	BodyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY_MESH_COMPONENT"));
	BodyMeshComponent->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BODY_MESH_OBJ
	(TEXT("/Game/Resources/StaticMeshes/SM_SignPost_1.SM_SignPost_1"));
	if (BODY_MESH_OBJ.Succeeded())
	{
		BodyMeshComponent->SetStaticMesh(BODY_MESH_OBJ.Object);
		BodyMeshComponent->SetRelativeRotation(FQuat(FRotator(0.f, 180.f, 0.f)));

	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ON_BEGIN_OVERLAP_PARTICLE_OBJ
	(TEXT("/Game/InfinityBladeEffects/Effects/FX_Ability/Heal/P_Health_Player_Buff_Ping_01.P_Health_Player_Buff_Ping_01"));
	if (ON_BEGIN_OVERLAP_PARTICLE_OBJ.Succeeded())
	{
		OnBeginOverlapParticle = ON_BEGIN_OVERLAP_PARTICLE_OBJ.Object;
	}

	OnHeartTakenAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ON_HEART_TAKEN_AUDIO_COMPONENT"));
	OnHeartTakenAudioComponent->SetupAttachment(BodyMeshComponent, BodyMeshCenterSocketName);
	static ConstructorHelpers::FObjectFinder<USoundCue> ON_HEART_TAKEN_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Menu_UI_Beeps/Cues/retro_ui_menu_blip_click_07_Cue.retro_ui_menu_blip_click_07_Cue"));
	if (ON_HEART_TAKEN_SOUND_OBJ.Succeeded())
	{
		OnHeartTakenAudioComponent->SetSound(ON_HEART_TAKEN_SOUND_OBJ.Object);
		OnHeartTakenAudioComponent->SetAutoActivate(false);
	}

	OnBrokenParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ON_BROKEN_PARTICLE_COMPONENT"));
	OnBrokenParticleComponent->SetupAttachment(BodyMeshComponent, BodyMeshCenterSocketName);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ON_BROKEN_PARTICLE_OBJ
	(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (ON_BROKEN_PARTICLE_OBJ.Succeeded())
	{
		OnBrokenParticleComponent->SetTemplate(ON_BEGIN_OVERLAP_PARTICLE_OBJ.Object);
		OnBrokenParticleComponent->SetAutoActivate(false);
	}

	AfterBrokenParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AFTER_BROKEN_PARTICLE_COMPONENT"));
	AfterBrokenParticleComponent->SetupAttachment(BodyMeshComponent, BodyMeshCenterSocketName);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> AFTER_BROKEN_PARTICLE_OBJ
	(TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"));
	if (AFTER_BROKEN_PARTICLE_OBJ.Succeeded())
	{
		AfterBrokenParticleComponent->SetTemplate(ON_BEGIN_OVERLAP_PARTICLE_OBJ.Object);
		AfterBrokenParticleComponent->SetAutoActivate(false);
	}


}

void AKRVEndOfPath::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(this);
	if (!KRVInGameGameMode)
	{
		KRVCHECK(KRVInGameGameMode);
	}
	else
	{
		OnEnemyEscapeDelegate.BindUObject(KRVInGameGameMode, &AKRVInGameGameMode::OnEnemyEscape);
		KRVInGameGameMode->OnGameEndDelegate.AddUObject(this, &AKRVEndOfPath::OnGameEnd);
	}

}

void AKRVEndOfPath::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKRVEndOfPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKRVEndOfPath::OnKRVCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKRVCharacter* KRVCharacter = Cast<AKRVCharacter>(OtherActor);
	if (!KRVCharacter || !GetWorld())
	{
		KRVCHECK(GetWorld());
	}
	else if (KRVCharacter->GetTeam() == EKRVTeam::Enemy)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnBeginOverlapParticle, KRVCharacter->GetTransform());
		OnHeartTakenAudioComponent->Play();
		OnEnemyEscapeDelegate.ExecuteIfBound(KRVCharacter);
	}

}

void AKRVEndOfPath::OnGameEnd(EKRVTeam Winner)
{
	switch (Winner)
	{
	case EKRVTeam::Player:
		break;
	case EKRVTeam::Enemy:
	{
		OnBrokenParticleComponent->ActivateSystem();
		if (GetWorld())
		{
			GetWorldTimerManager().SetTimer(OnBrokenParticleDelayTimer, this, 
				&AKRVEndOfPath::ActivateAfterBrokenParticleComponent, AfterBrokenParticleActivationDelay, false);
		}
		break;
	}
	default:
		break;
	}

}

void AKRVEndOfPath::ActivateAfterBrokenParticleComponent()
{
	AfterBrokenParticleComponent->ActivateSystem();

}

