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

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetSphereRadius(AggroRange);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMinion::OnDetectionBeginOverlap);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AMinion::OnDetectionEndOverlap);
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

void AMinion::SetAggroRange(float NewRange)
{
	AggroRange = NewRange;

	if (DetectionSphere)
	{
		DetectionSphere->SetSphereRadius(AggroRange);
	}
}

void AMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MovementStrategy) return;

	if (CurrentTarget)
	{
		MovementStrategy->UpdateMovement(this, CurrentTarget);
	}
	else if (OwnerUnit)
	{
		MovementStrategy->UpdateMovement(this, OwnerUnit);
	}
}

void AMinion::OnDetectionBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	ABaseUnit* OtherUnit = Cast<ABaseUnit>(OtherActor);
	if (!OtherUnit)
	{
		return;
	}

	// Only enemies
	if (OtherUnit->GetTeam() == GetTeam())
	{
		return;
	}

	if (CurrentTarget && CurrentTarget != OwnerUnit)
	{
		return;
	}

	SetCurrentTarget(OtherUnit);
}

void AMinion::OnDetectionEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (!OtherActor)
	{
		return;
	}

	ABaseUnit* OtherUnit = Cast<ABaseUnit>(OtherActor);
	if (!OtherUnit)
	{
		return;
	}

	if (CurrentTarget == OtherUnit)
	{
		if (OwnerUnit)
		{
			SetCurrentTarget(OwnerUnit);
		}
		else
		{
			SetCurrentTarget(nullptr);
		}
	}
}