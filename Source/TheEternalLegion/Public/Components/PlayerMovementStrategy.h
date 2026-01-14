// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/MovementStrategy.h"
#include "PlayerMovementStrategy.generated.h"

/**
 * 
 */
UCLASS()
class THEETERNALLEGION_API UPlayerMovementStrategy : public UMovementStrategy
{
	GENERATED_BODY()
	
public:

	virtual void Move(float ForwardValue, float RightValue) override;
};
