// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Minion.h"
#include "Components/MinionMovementStrategy.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

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

	AttackRange = 150.0f;
	AttackDamage = 10.0f;
	AttackInterval = 1.0f;
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

	// Handle exit of old state
	if (CurrentState == EMinionState::Attacking)
	{
		StopAttack();
	}

	CurrentState = NewState;

	// Handle enter of new state
	if (CurrentState == EMinionState::Attacking)
	{
		StartAttack();
	}
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

void AMinion::StartAttack()
{
	if (AttackInterval <= 0.0f)
	{
		return;
	}

	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&AMinion::PerformAttack,
		AttackInterval,
		true,
		0.0f
	);
}

void AMinion::StopAttack()
{
	if (AttackTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	}
}

void AMinion::PerformAttack()
{
	if (CurrentState != EMinionState::Attacking || !CurrentTarget)
	{
		return;
	}

	if (CurrentTarget->IsDead())
	{
		if (OwnerUnit)
		{
			SetCurrentTarget(OwnerUnit);
		}
		else
		{
			SetCurrentTarget(nullptr);
		}
		return;
	}

	const float DistSq = FVector::DistSquared(GetActorLocation(), CurrentTarget->GetActorLocation());
	if (DistSq > FMath::Square(AttackRange))
	{
		return;
	}

	CurrentTarget->ApplyDamage(AttackDamage, this);

	#if WITH_EDITOR
		if (GEngine)
		{
			const FString Msg = FString::Printf(
				TEXT("Minion %s attacked %s for %.1f damage (HP: %.1f / %.1f)"),
				*GetName(),
				*CurrentTarget->GetName(),
				AttackDamage,
				CurrentTarget->GetCurrentHealth(),
				CurrentTarget->GetMaxHealth()
			);
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, Msg);
		}
	#endif
}