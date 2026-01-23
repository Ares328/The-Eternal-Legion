// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/MovementStrategy.h"
#include "MinionMovementStrategy.generated.h"

class AAIController;

UCLASS()
class THEETERNALLEGION_API UMinionMovementStrategy : public UMovementStrategy
{
	GENERATED_BODY()

public:

	virtual void UpdateMovement(ABaseUnit* Unit, ABaseUnit* Target) override;
};
