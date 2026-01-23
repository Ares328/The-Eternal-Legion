// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MinionMovementStrategy.h"
#include "Entities/BaseUnit.h"
#include "Entities/Minion.h"
#include "AITypes.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

void UMinionMovementStrategy::UpdateMovement(ABaseUnit* Unit, ABaseUnit* Target)
{
	if (!Unit || !Target) return;

	float AcceptanceRadius = 150.0f;

	EMinionState MinionState = EMinionState::Idle;
	if (AMinion* Minion = Cast<AMinion>(Unit))
	{
		MinionState = Minion->GetState();
		if (MinionState == EMinionState::Idle || MinionState == EMinionState::Dead)
		{
			return;
		}

		AcceptanceRadius = FMath::Clamp(
			Minion->GetSimpleCollisionRadius() + 50.0f,
			100.0f,
			Minion->GetAggroRange()
		);
	}

	const float DistSq = FVector::DistSquared(Unit->GetActorLocation(), Target->GetActorLocation());
	const FString DebugMsg = FString::Printf(
		TEXT("Minion State: %d, DistanceSq: %.2f, AcceptanceRadius: %.2f"),
		static_cast<int32>(MinionState),
		DistSq,
		AcceptanceRadius
	);
	GEngine->AddOnScreenDebugMessage(
		-1,
		0.2f,
		FColor::Cyan,
		DebugMsg
	);

	if (MinionState == EMinionState::Following)
	{
		const float DesiredRange = 150.0f;
		const float ReacquireRange = 250.0f;

		if (DistSq <= FMath::Square(DesiredRange))
		{
			return;
		}

		AcceptanceRadius = ReacquireRange;
	}
	else if (MinionState == EMinionState::Attacking)
	{
		if (DistSq <= FMath::Square(AcceptanceRadius))
		{
			return;
		}
	}
	else
	{
		if (DistSq <= FMath::Square(AcceptanceRadius))
		{
			return;
		}
	}

	if (AAIController* AIC = Cast<AAIController>(Unit->GetController()))
	{
		FAIMoveRequest MoveReq;
		MoveReq.SetGoalActor(Target);
		MoveReq.SetAcceptanceRadius(AcceptanceRadius);
		MoveReq.SetReachTestIncludesAgentRadius(true);

		AIC->MoveTo(MoveReq);
	}
}