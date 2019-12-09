// Fill out your copyright notice in the Description page of Project Settings.
// This class is based on ARCHITYPE - RTS Camera Edge Panning / Scrolling https://www.youtube.com/watch?v=6JAuSV4WZuc

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "KRVPlayerCameraPawn.generated.h"

UCLASS()
class KRV_API AKRVPlayerCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	AKRVPlayerCameraPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UFUNCTION(BlueprintCallable)
		AKRVPlayerController* GetKRVPlayerController();

protected:
	UPROPERTY()
		class USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class AKRVPlayerController* KRVPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USpringArmComponent* MainSpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UCameraComponent* MainCamera;

public:
	UFUNCTION(BlueprintCallable)
		void ResetCameraPos();

protected:
	void InitPos();
	void UpdateCameraPos(float DeltaTime);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CameraTiltAngle = 30.f;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite)
		bool bIsCameraLocked;



public: // Pan
	UFUNCTION(BlueprintCallable)
		void UpdateResolution();

	UFUNCTION(BlueprintCallable)
		FVector GetCameraPanDirection();

	UFUNCTION(BlueprintCallable)
		float GetPanMargin();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PanMargin = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PanSpeed = 15.f;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
		FVector DefaultCameraPos;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
		FVector CameraPosToMove;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
		float ScreenSizeX;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
		float ScreenSizeY;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
		float PanLimitRatioX = 0.2f;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
		float PanLimitRatioY = 0.2f;


public: // Zoom
	UFUNCTION(BlueprintCallable)
		float GetCameraHeight();

	UFUNCTION(BlueprintCallable)
		void ZoomIn();

	UFUNCTION(BlueprintCallable)
		void ZoomOut();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxCamHeightFromStatic = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinCamHeightFromStatic = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ZoomSpeed = 100.f;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite)
		float CurrentCamHeightFromStatic;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite)
		float WorldStaticHeight;


};
