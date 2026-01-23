// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/BaseUnit.h"
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

	virtual void Tick(float DeltaTime) override;
};
