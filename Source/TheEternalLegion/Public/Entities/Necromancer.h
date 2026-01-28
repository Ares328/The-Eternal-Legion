// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/BaseUnit.h"
#include "Entities/Minion.h"
#include "Necromancer.generated.h"

class UInputProcessorComponent;
class AAIController;
class UMinionCommandsWidget;

UCLASS()
class THEETERNALLEGION_API ANecromancer : public ABaseUnit
{
	GENERATED_BODY()

public:
	ANecromancer();

	UFUNCTION(BlueprintCallable, Category = "Legion | Abilities")
	void SummonMinion();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | Abilities")
	float ConversionRange;

	UFUNCTION(BlueprintCallable, Category = "Legion | Abilities")
	void ConvertTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | Minions")
	float AggroRangeStep = 200.0f;

	UFUNCTION(BlueprintCallable, Category = "Legion | Minions")
	void CommandMinionsIncreaseAggro();

	UFUNCTION(BlueprintCallable, Category = "Legion | Minions")
	void CommandMinionsDecreaseAggro();

	UFUNCTION(BlueprintCallable, Category = "Legion | Minions")
	float GetMinionAggroRange() const;

	UFUNCTION(BlueprintCallable, Category = "Legion | UI")
	void UpdateHealthOnWidget();

	void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Legion | Input", meta = (AllowPrivateAccess = "true"))
	UInputProcessorComponent* InputProcessor;

	UPROPERTY()
	UMinionCommandsWidget* MinionCommandsWidget;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | Abilities")
	TSubclassOf<class ABaseUnit> MinionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | Minions")
	TArray<TObjectPtr<AMinion>> ControlledMinions;

	virtual void OnDamaged(ABaseUnit* DamageCauser) override;

	UFUNCTION()
	void OnControlledMinionDeath(ABaseUnit* DeadUnit);
};
