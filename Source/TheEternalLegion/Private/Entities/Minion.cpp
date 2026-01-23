// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Minion.h"
#include "Components/MinionMovementStrategy.h"

AMinion::AMinion()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentTeam = ETeam::Enemy;
	MovementStrategy = CreateDefaultSubobject<UMinionMovementStrategy>(TEXT("MovementStrategy"));
	AggroRange = 500.0f;

	CurrentState = EMinionState::Idle;
	OwnerUnit = nullptr;
	CurrentTarget = nullptr;
}

void AMinion::OnTeamChanged_Implementation(ETeam NewTeam)
{

	Super::OnTeamChanged_Implementation(NewTeam);
}

void AMinion::SetState(EMinionState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
}

void AMinion::SetOwnerUnit(ABaseUnit* NewOwner)
{
	OwnerUnit = NewOwner;

	// follow your owner
	if (OwnerUnit)
	{
		SetCurrentTarget(OwnerUnit);
		SetState(EMinionState::Following);
	}
	else
	{
		SetCurrentTarget(nullptr);
		SetState(EMinionState::Idle);
	}
}

void AMinion::SetCurrentTarget(ABaseUnit* NewTarget)
{
	CurrentTarget = NewTarget;

	// - If target is owner -> Following
	// - If target is non-null and not owner -> Attacking
	// - If null -> Idle
	if (!CurrentTarget)
	{
		SetState(EMinionState::Idle);
	}
	else if (CurrentTarget == OwnerUnit)
	{
		SetState(EMinionState::Following);
	}
	else
	{
		SetState(EMinionState::Attacking);
	}
}

void AMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (MovementStrategy)
	{
		MovementStrategy->UpdateMovement(this, CurrentTarget);
	}
}