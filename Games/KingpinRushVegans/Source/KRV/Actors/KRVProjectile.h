// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KRVEnums.h"
#include "Structs/KRVAttackStat.h"
#include "Interfaces/KRVDataAssetInterface.h"
#include "KRVProjectile.generated.h"

class USoundCue;
class UPooledComponent;
class UProjectileMovementComponent;
class UAudioComponent;
class UBoxComponent;
class UParticleSystemComponent;

class UKRVDamageType;
class UKRVProjectileDataAsset;
class AKRVCombatUnit;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnProjectileHitDelegate, AKRVCharacter*, AKRVCombatUnit*);

UCLASS()
class KRV_API AKRVProjectile : public AActor, public IKRVDataAssetInterface
{
	GENERATED_BODY()

public:
	AKRVProjectile();

public:
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;


public: // DataAsset
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		class UKRVDataAsset* GetDataAsset() const;
	UKRVDataAsset* GetDataAsset_Implementation() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool LoadFromDataAsset();
	virtual bool LoadFromDataAsset_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Load")
		UKRVProjectileDataAsset* GetProjectileDataAsset() const;

protected:
	void OnBodyMeshAsyncLoaded();
	void OnHeadParticleAsyncLoaded();
	void OnTailParticleAsyncLoaded();
	void OnFlySoundAsyncLoaded();
	void OnHitParticleAsyncLoaded();
	void OnHitSoundAsyncLoaded();
	void OnOnDestroyedParticleAsyncLoaded();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Load")
		TSoftObjectPtr<UKRVProjectileDataAsset> MyDataAsset;

	UPROPERTY()
		UKRVProjectileDataAsset* HardPtr_MyDataAsset;

	FSoftObjectPath BodyMeshPath;
	FSoftObjectPath HeadParticlePath;
	FSoftObjectPath TailParticlePath;
	FSoftObjectPath FlySoundPath;
	FSoftObjectPath HitParticlePath;
	FSoftObjectPath HitSoundPath;
	FSoftObjectPath OnDestroyedParticlePath;

protected: // Body & Hitbox
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision")
		UBoxComponent* HitBoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Body")
		UStaticMeshComponent* BodyMeshComponent;

	FName HeadSocketName = "Head";
	FName TailSocketName = "Tail";
	FName HitBoxSocketName = "HitBox";

public: // Spawn & Destroy
	UFUNCTION(BlueprintCallable, Category = "Spawn")
		virtual void DestroyThis();

protected:
	UFUNCTION()
		void OnSpawned();

	UFUNCTION()
		void OnDestroyed();

public:
	UPROPERTY()
		UPooledComponent* PooledComponent;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Spawn")
		UParticleSystem* OnDestroyedParticle;

	FTimerHandle DeadbodyLifeSpanTimer;

public: // Fly
	UFUNCTION(BlueprintCallable, Category = "Movement")
		UProjectileMovementComponent* GetMovementComponent() const;

protected:
	void OnDeparted();
	void RotateBody(float DeltaSeconds);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Fly")
		UParticleSystemComponent* HeadParticleComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Fly")
		UParticleSystemComponent* TailParticleComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Fly")
		UAudioComponent* FlyAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fly")
		UProjectileMovementComponent* MovementComponent;

	FTimerHandle OnDepartedTimer;

protected: // Hit
	UFUNCTION()
		void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void CheckHitToFlyTarget();
	void CheckMineActivated(float DeltaTime);
	void StopProjectileMovement();

public:
	FOnProjectileHitDelegate OnProjectileHitDelegate;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Hit")
		USoundCue* HitSound;

	UPROPERTY(BlueprintReadOnly, Category = "Hit")
		UParticleSystem* HitParticle;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Hit")
		bool bIsFlying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
		float DistToHitToTarget = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit")
		float DistToActivateMine = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float IntervalToCheckMineActivation = 0.1;

	float CurrentTimeToCheckMineActivation;

public: // Shooter & Target
	UFUNCTION()
		void SetTargetUnit(AKRVCombatUnit* NewTarget);

public:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Shooter")
		EKRVTeam MyTeam;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Shooter")
		TWeakObjectPtr<AKRVCombatUnit> Shooter;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Shooter")
		ETargetType TargetType;

protected:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Target")
		bool bHasTarget;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Target")
		TWeakObjectPtr<AKRVCombatUnit> FlyTarget;

public: // Ranged
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Ranged")
		FKRVAttackStat RangedAttackStat;

public: // Splash
	UFUNCTION(BlueprintCallable, Category = "Splash")
		void Explode();

	UPROPERTY(Transient, BlueprintReadWrite, Category = "Splash")
		FKRVAttackStat SplashAttackStat;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Splash")
		bool bShouldExplode;

	FTimerHandle ExplosionTimer;

};

