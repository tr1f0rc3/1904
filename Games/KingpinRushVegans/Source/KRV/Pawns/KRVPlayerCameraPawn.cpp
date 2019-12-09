// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVPlayerCameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Controllers/KRVPlayerController.h"

AKRVPlayerCameraPawn::AKRVPlayerCameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT_SCENE_COMPONENT"));
	SetRootComponent(RootScene);

	MainSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRING_ARM_COMPONENT"));
	MainSpringArm->SetupAttachment(RootScene);
	MainSpringArm->bDoCollisionTest = false;
	MainSpringArm->SetRelativeRotation(FRotator(-90.f + CameraTiltAngle, 0.f, 0.f));

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA_COMPONENT"));
	MainCamera->SetupAttachment(MainSpringArm);

	if (MinCamHeightFromStatic > MaxCamHeightFromStatic)
	{
		Swap(MinCamHeightFromStatic, MaxCamHeightFromStatic);
	}

}

void AKRVPlayerCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	UpdateResolution();
	InitPos();

}

void AKRVPlayerCameraPawn::ResetCameraPos()
{
	CameraPosToMove = DefaultCameraPos;

}

void AKRVPlayerCameraPawn::InitPos()
{
	FHitResult GroundHit;
	GetWorld()->LineTraceSingleByChannel(GroundHit, GetActorLocation() + 10000.f * FVector::UpVector,
		GetActorLocation() + 10000.f * FVector::DownVector, ECC_WorldStatic);
	WorldStaticHeight = GroundHit.Location.Z;
	DefaultCameraPos = GetActorLocation();
	DefaultCameraPos.Z = GetCameraHeight();
	CameraPosToMove = DefaultCameraPos;
	SetActorLocation(DefaultCameraPos);

}

// Called every frame
void AKRVPlayerCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCameraPos(DeltaTime);

}


void AKRVPlayerCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &AKRVPlayerCameraPawn::ZoomIn);
	InputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &AKRVPlayerCameraPawn::ZoomOut);
	InputComponent->BindAction("KeyboardR", IE_Pressed, this, &AKRVPlayerCameraPawn::ResetCameraPos);

}

void AKRVPlayerCameraPawn::UpdateCameraPos(float DeltaTime)
{
	if (bIsCameraLocked)
	{
		return;
	}
	else
	{
		CameraPosToMove += GetCameraPanDirection();
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), CameraPosToMove, DeltaTime, 3.f));
	}

}


AKRVPlayerController* AKRVPlayerCameraPawn::GetKRVPlayerController()
{
	if (!KRVPlayerController)
	{
		KRVPlayerController = Cast<AKRVPlayerController>(GetController());
	}
	return KRVPlayerController;

}

FVector AKRVPlayerCameraPawn::GetCameraPanDirection()
{
	if (!GetKRVPlayerController())
	{
		KRVCHECK(GetKRVPlayerController());
	}
	else if (!bIsCameraLocked)
	{
		float MousePosX = 0.f;
		float MousePosY = 0.f;
		float CamDirectionX = 0.f;
		float CamDirectionY = 0.f;

		GetKRVPlayerController()->GetMousePosition(MousePosX, MousePosY);
		if (MousePosX <= PanMargin && GetActorLocation().Y >= -1.f * ScreenSizeX * PanLimitRatioX)
		{
			CamDirectionY = -1.f;
		}
		else if (MousePosX >= ScreenSizeX - PanMargin && GetActorLocation().Y <= ScreenSizeX * PanLimitRatioX)
		{
			CamDirectionY = 1.f;
		}

		if (MousePosY <= PanMargin && GetActorLocation().X <= ScreenSizeY * PanLimitRatioY)
		{
			CamDirectionX = 1.f;
		}
		else if (MousePosY >= ScreenSizeY - PanMargin && GetActorLocation().X <= -1.f * ScreenSizeY * PanLimitRatioY)
		{
			CamDirectionX = -1.f;
		}

		return PanSpeed * FVector(CamDirectionX, CamDirectionY, 0.f);
	}
	return FVector::ZeroVector;

}


float AKRVPlayerCameraPawn::GetPanMargin()
{
	return PanMargin;

}

void AKRVPlayerCameraPawn::UpdateResolution()
{
	if (!GetKRVPlayerController())
	{
		KRVCHECK(GetKRVPlayerController());
	}
	else
	{
		int32 OutScreenSizeX;
		int32 OutScreenSizeY;
		GetKRVPlayerController()->GetViewportSize(OutScreenSizeX, OutScreenSizeY);
		ScreenSizeX = float(OutScreenSizeX);
		ScreenSizeY = float(OutScreenSizeY);
		CurrentCamHeightFromStatic = MaxCamHeightFromStatic;
	}

}

float AKRVPlayerCameraPawn::GetCameraHeight()
{
	if (bIsCameraLocked)
	{
		return GetActorLocation().Z;
	}
	else
	{
		return WorldStaticHeight + CurrentCamHeightFromStatic;
	}

}

void AKRVPlayerCameraPawn::ZoomIn()
{
	if (!bIsCameraLocked)
	{
		CurrentCamHeightFromStatic = FMath::Clamp(CurrentCamHeightFromStatic - ZoomSpeed,
			MinCamHeightFromStatic, MaxCamHeightFromStatic);
		CameraPosToMove.Z = GetCameraHeight();
	}
	
}

void AKRVPlayerCameraPawn::ZoomOut()
{
	if (!bIsCameraLocked)
	{
		CurrentCamHeightFromStatic = FMath::Clamp(CurrentCamHeightFromStatic + ZoomSpeed,
			MinCamHeightFromStatic, MaxCamHeightFromStatic);
		CameraPosToMove.Z = GetCameraHeight();
	}

}

