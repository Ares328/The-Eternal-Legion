// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Entities/BaseUnit.h"
#include "WaveManager.generated.h"

class ABaseUnit;

USTRUCT(BlueprintType)
struct FEnemyWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSubclassOf<ABaseUnit> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 Count = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float SpawnInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float StartDelay = 2.0f;
};

UCLASS()
class THEETERNALLEGION_API AWaveManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaveManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<FEnemyWave> Waves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	bool bWaitForClearBeforeNextWave = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	bool bLoopWaves = false;

	int32 CurrentWaveIndex = 0;
	int32 EnemiesSpawnedInCurrentWave = 0;
	int32 AliveEnemiesInCurrentWave = 0;

	FTimerHandle WaveStartTimerHandle;
	FTimerHandle SpawnTimerHandle;

	void StartNextWave();
	void StartWaveSpawning();
	void SpawnOneEnemy();

	UFUNCTION()
	void OnEnemyDeath(ABaseUnit* DeadUnit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartWaves();

	UFUNCTION(BlueprintCallable, Category = "Wave")
	int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }

	UFUNCTION(BlueprintCallable, Category = "Wave")
	int32 GetAliveEnemiesInCurrentWave() const { return AliveEnemiesInCurrentWave; }
};
