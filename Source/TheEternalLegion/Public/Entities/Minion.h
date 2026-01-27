// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/BaseUnit.h"
#include "Components/SphereComponent.h"
#include "Minion.generated.h"

UENUM(BlueprintType)
enum class EMinionState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Following   UMETA(DisplayName = "Following"),
	Attacking   UMETA(DisplayName = "Attacking"),
	Dead        UMETA(DisplayName = "Dead")
};

UCLASS()
class THEETERNALLEGION_API AMinion : public ABaseUnit
{
	GENERATED_BODY()

public:
	AMinion();

protected:
	virtual void OnTeamChanged_Implementation(ETeam NewTeam) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | AI")
	float AggroRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Legion | AI")
	EMinionState CurrentState = EMinionState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | AI")
	TObjectPtr<ABaseUnit> OwnerUnit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Legion | AI")
	TObjectPtr<ABaseUnit> CurrentTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Legion | AI")
	TObjectPtr<USphereComponent> DetectionSphere;

	UFUNCTION()
	void OnDetectionBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnDetectionEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void HandleActorEnteredDetection(AActor* OtherActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | Combat")
	float AttackRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | Combat")
	float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | Combat")
	float AttackInterval = 1.0f;

	UPROPERTY() 
	TArray<TObjectPtr<ABaseUnit>> EnemiesInRange;

	ABaseUnit* FindBestEnemyTarget() const;

	void ScanInitialOverlaps();

	FTimerHandle AttackTimerHandle;

	UFUNCTION()
	void PerformAttack();

	void StartAttack();
	void StopAttack();

	virtual void OnDamaged(ABaseUnit* DamageCauser) override;

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Legion | AI")
	void SetState(EMinionState NewState);

	UFUNCTION(BlueprintCallable, Category = "Legion | AI")
	EMinionState GetState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable, Category = "Legion | AI")
	void SetOwnerUnit(ABaseUnit* NewOwner);

	UFUNCTION(BlueprintCallable, Category = "Legion | AI")
	ABaseUnit* GetOwnerUnit() const { return OwnerUnit; }

	UFUNCTION(BlueprintCallable, Category = "Legion | AI")
	ABaseUnit* GetCurrentTarget() const { return CurrentTarget; }

	UFUNCTION(BlueprintCallable, Category = "Legion | AI")
	void SetCurrentTarget(ABaseUnit* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Legion | AI")
	float GetAggroRange() const { return AggroRange; }

	UFUNCTION(BlueprintCallable, Category = "Legion | AI")
	void SetAggroRange(float NewRange);

	virtual void Tick(float DeltaTime) override;
};
