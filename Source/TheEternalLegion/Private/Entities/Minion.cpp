// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Minion.h"
#include "Components/MinionMovementStrategy.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/MinionAnimInstance.h"

AMinion::AMinion()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentTeam = ETeam::Neutral;
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

	AttackRange = 200.0f;
	AttackDamage = 10.0f;
	AttackInterval = 1.0f;
}

void AMinion::BeginPlay()
{
	Super::BeginPlay();

	if (DetectionSphere)
	{
		#if WITH_EDITOR
			if (GEngine)
			{
				const FString Msg = FString::Printf(
					TEXT("MINION DETECTION SPHERE RADIUS SET TO: %f"),
					DetectionSphere->GetScaledSphereRadius()
				);
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Msg);
			}
		#endif
		FTimerHandle TmpHandle;
		GetWorldTimerManager().SetTimer(
			TmpHandle,
			this,
			&AMinion::ScanInitialOverlaps,
			0.5f,
			false
		);
	}
}

void AMinion::ScanInitialOverlaps()
{
	if (!DetectionSphere)
	{
		return;
	}

	TArray<UPrimitiveComponent*> OverlappingComps;
	DetectionSphere->GetOverlappingComponents(OverlappingComps);

	#if WITH_EDITOR
		if (GEngine)
		{
			const FString Msg = FString::Printf(
				TEXT("INITIAL OVERLAPS FOUND: %d"), OverlappingComps.Num());
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Msg);
		}
	#endif

	for (UPrimitiveComponent* Comp : OverlappingComps)
	{
		if (!Comp) continue;
		AActor* OtherActor = Comp->GetOwner();
		HandleActorEnteredDetection(OtherActor);
	}
}

void AMinion::HandleActorEnteredDetection(AActor* OtherActor)
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

	if (OtherUnit->GetTeam() == GetTeam())
	{
		return;
	}

	EnemiesInRange.AddUnique(OtherUnit);
	if (ABaseUnit* NewEnemy = FindBestEnemyTarget())
	{
		if (CurrentTarget != NewEnemy)
		{
		#if WITH_EDITOR
					if (GEngine)
					{
						const FString Msg = FString::Printf(
							TEXT("TARGET SET: %s (%s)"),
							*NewEnemy->GetName(),
							*NewEnemy->GetClass()->GetName()
						);
						GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, Msg);
					}
		#endif
			SetCurrentTarget(NewEnemy);
		}
	}
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

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UMinionAnimInstance* Anim = Cast<UMinionAnimInstance>(MeshComp->GetAnimInstance()))
		{
			Anim->Speed = GetVelocity().Size2D();
			Anim->bIsAttacking = (CurrentState == EMinionState::Attacking);
		}
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
	#if WITH_EDITOR
		if (GEngine)
		{
			const FString Msg = FString::Printf(
				TEXT("Overlap Begin: %s (%s)"),
				OtherActor ? *OtherActor->GetName() : TEXT("NULL"),
				OtherActor ? *OtherActor->GetClass()->GetName() : TEXT("None")
			);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, Msg);
		}
	#endif

	HandleActorEnteredDetection(OtherActor);
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
	EnemiesInRange.Remove(OtherUnit);
	if (CurrentTarget == OtherUnit)
	{
		if (ABaseUnit* NewEnemy = FindBestEnemyTarget())
		{
			SetCurrentTarget(NewEnemy);
		}
		else if (OwnerUnit)
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

ABaseUnit* AMinion::FindBestEnemyTarget() const
{
	ABaseUnit* BestEnemy = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (ABaseUnit* Enemy : EnemiesInRange)
	{
		if (!IsValid(Enemy) || Enemy->IsDead() || Enemy->GetTeam() == GetTeam())
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(GetActorLocation(), Enemy->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestEnemy = Enemy;
		}
	}

	return BestEnemy;
}

void AMinion::PerformAttack()
{
	if (CurrentState != EMinionState::Attacking || !IsValid(CurrentTarget))
	{
		if (CurrentTarget)
		{
			EnemiesInRange.Remove(CurrentTarget);
			CurrentTarget = nullptr;
		}

		if (ABaseUnit* NewEnemy = FindBestEnemyTarget())
		{
			SetCurrentTarget(NewEnemy);
		}
		else if (OwnerUnit)
		{
			SetCurrentTarget(OwnerUnit);
		}
		else
		{
			SetCurrentTarget(nullptr);
		}
		return;
	}

	if (CurrentTarget->IsPendingKillPending())
	{
		EnemiesInRange.Remove(CurrentTarget);
		CurrentTarget = nullptr;
		SetCurrentTarget(nullptr);
		return;
	}

	if (CurrentTarget->IsDead())
	{
		EnemiesInRange.Remove(CurrentTarget);

		if (ABaseUnit* NewEnemy = FindBestEnemyTarget())
		{
			SetCurrentTarget(NewEnemy);
		}
		else if (OwnerUnit)
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
		if (GEngine && IsValid(CurrentTarget) && !CurrentTarget->IsPendingKillPending())
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

void AMinion::OnDamaged(ABaseUnit* DamageCauser)
{
	if (!DamageCauser || DamageCauser == this || DamageCauser->GetTeam() == GetTeam())
	{
		return;
	}

	const float DistSq = FVector::DistSquared(GetActorLocation(), DamageCauser->GetActorLocation());
	const float RadiusSq = FMath::Square(
		DetectionSphere ? DetectionSphere->GetScaledSphereRadius() : AggroRange
	);

	if (DistSq > RadiusSq)
	{
		return;
	}

	EnemiesInRange.AddUnique(DamageCauser);

	if (CurrentTarget != DamageCauser)
	{
	#if WITH_EDITOR
			if (GEngine)
			{
				const FString Msg = FString::Printf(
					TEXT("Minion %s RETALIATES against %s"),
					*GetName(),
					*DamageCauser->GetName()
				);
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, Msg);
			}
	#endif
		SetCurrentTarget(DamageCauser);
	}
}