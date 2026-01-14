// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/BaseUnit.h"
#include "Necromancer.generated.h"

/**
 * 
 */
UCLASS()
class THEETERNALLEGION_API ANecromancer : public ABaseUnit
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | Abilities")
	TSubclassOf<class ABaseUnit> MinionClass;

	UFUNCTION(BlueprintCallable, Category = "Legion | Abilities")
	void SummonMinion();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | Abilities")
	float ConversionRange;

	UFUNCTION(BlueprintCallable, Category = "Legion | Abilities")
	void ConvertUnit(ABaseUnit* TargetUnit);
	
public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
