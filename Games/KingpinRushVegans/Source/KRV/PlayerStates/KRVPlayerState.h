// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "KRVPlayerState.generated.h"

UCLASS(Abstract)
class KRV_API AKRVPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AKRVPlayerState();

public: // save and load
	virtual bool ResetInGameSave();
	virtual bool LoadInGameSave();
	virtual bool SaveInGameSave();
	void SetSaveSlotString(const FString& NewString);
	class UKRVInGameSave* GetInGameSave();

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVInGameSave* LoadedInGameSave;

	UPROPERTY(BlueprintReadWrite)
		FString SaveSlotString = TEXT("DefaultInGame");

};
