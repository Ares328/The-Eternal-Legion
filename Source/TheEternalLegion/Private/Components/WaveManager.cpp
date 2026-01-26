// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WaveManager.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWaveManager::AWaveManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWaveManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (SpawnPoints.Num() == 0)
	{
		TArray<AActor*> FoundPoints;
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), FoundPoints);

		for (AActor* Actor : FoundPoints)
		{
			SpawnPoints.Add(Actor);
		}
	}

	StartWaves();
}

// Called every frame
void AWaveManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWaveManager::StartWaves()
{
	if (Waves.Num() == 0)
	{
		#if WITH_EDITOR
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
				-1, 3.0f, FColor::Red,
				TEXT("WaveManager: No waves configured!")
			);
		}
		#endif
		return;
	}

	CurrentWaveIndex = 0;
	StartNextWave();
}

void AWaveManager::StartNextWave()
{
	if (Waves.Num() == 0)
	{
		return;
	}

	if (CurrentWaveIndex >= Waves.Num())
	{
		if (bLoopWaves)
		{
			CurrentWaveIndex = 0;
		}
		else
		{
			#if WITH_EDITOR
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(
					-1, 3.0f, FColor::Green,
					TEXT("WaveManager: All waves completed.")
				);
			}
			#endif
			return;
		}
	}

	const FEnemyWave& Wave = Waves[CurrentWaveIndex];

	EnemiesSpawnedInCurrentWave = 0;
	AliveEnemiesInCurrentWave = 0;

	if (Wave.StartDelay > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
			WaveStartTimerHandle,
			this,
			&AWaveManager::StartWaveSpawning,
			Wave.StartDelay,
			false
		);
	}
	else
	{
		StartWaveSpawning();
	}
}

void AWaveManager::StartWaveSpawning()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		return;
	}

	const FEnemyWave& Wave = Waves[CurrentWaveIndex];

	if (Wave.Count <= 0 || !Wave.EnemyClass)
	{
		CurrentWaveIndex++;
		StartNextWave();
		return;
	}

	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&AWaveManager::SpawnOneEnemy,
		FMath::Max(Wave.SpawnInterval, 0.01f),
		true
	);
}

void AWaveManager::SpawnOneEnemy()
{
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	const FEnemyWave& Wave = Waves[CurrentWaveIndex];

	if (EnemiesSpawnedInCurrentWave >= Wave.Count)
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	if (SpawnPoints.Num() == 0)
	{
#if WITH_EDITOR
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 3.0f, FColor::Red,
				TEXT("WaveManager: No SpawnPoints assigned!")
			);
		}
#endif
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	AActor* SpawnPoint = SpawnPoints[FMath::RandHelper(SpawnPoints.Num())];
	if (!SpawnPoint)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ABaseUnit* SpawnedEnemy = World->SpawnActor<ABaseUnit>(
		Wave.EnemyClass,
		SpawnPoint->GetActorLocation(),
		SpawnPoint->GetActorRotation(),
		SpawnParams
	);

	if (SpawnedEnemy)
	{
		if (SpawnedEnemy->GetTeam() == ETeam::Neutral)
		{
			SpawnedEnemy->SetTeam(ETeam::Enemy);
		}

		EnemiesSpawnedInCurrentWave++;
		AliveEnemiesInCurrentWave++;

		SpawnedEnemy->OnUnitDeath.AddDynamic(this, &AWaveManager::OnEnemyDeath);
	}
}

void AWaveManager::OnEnemyDeath(ABaseUnit* DeadUnit)
{
	if (AliveEnemiesInCurrentWave > 0)
	{
		AliveEnemiesInCurrentWave--;
	}

	if (bWaitForClearBeforeNextWave && AliveEnemiesInCurrentWave == 0)
	{
		CurrentWaveIndex++;
		StartNextWave();
	}
}
