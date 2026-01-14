// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/BaseUnit.h"
#include "Minion.generated.h"

/**
 * 
 */
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
};
