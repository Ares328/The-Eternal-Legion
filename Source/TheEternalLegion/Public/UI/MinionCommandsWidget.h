// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinionCommandsWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEETERNALLEGION_API UMinionCommandsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Legion | UI")
	void OnSummonTriggered();

	UFUNCTION(BlueprintImplementableEvent, Category = "Legion | UI")
	void OnConvertTriggered();

	UFUNCTION(BlueprintImplementableEvent, Category = "Legion | UI")
	void OnAttackTriggered();

	UFUNCTION(BlueprintImplementableEvent, Category = "Legion | UI")
	void OnDefendTriggered();

	UFUNCTION(BlueprintImplementableEvent, Category = "Legion | UI")
	void SetAggroRange(float NewAggroRange);

	UFUNCTION(BlueprintImplementableEvent, Category = "Legion | UI")
	void SetHealthPercentage(float NewHealthPercentage);
	
};
