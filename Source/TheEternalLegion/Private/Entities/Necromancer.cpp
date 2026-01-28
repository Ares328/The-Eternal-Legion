// Fill out your copyright notice in the Description page of Project Settings.

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Entities/Necromancer.h"
#include "Entities/Minion.h"
#include "Entities/BaseUnit.h"
#include "Components/PlayerMovementStrategy.h"
#include "AIController.h"
#include "Components/InputProcessorComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/MinionCommandsWidget.h"
#include "UObject/ConstructorHelpers.h"

ANecromancer::ANecromancer()
{
    MovementStrategy = CreateDefaultSubobject<UPlayerMovementStrategy>(TEXT("MovementStrategy"));
    InputProcessor = CreateDefaultSubobject<UInputProcessorComponent>(TEXT("InputProcessor"));
    CurrentTeam = ETeam::Player;

    ConversionRange = 1000.0f;
    MinionCommandsWidget = nullptr;
}

void ANecromancer::BeginPlay()
{
    Super::BeginPlay();

    #if WITH_EDITOR
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1, 3.0f, FColor::Green,
                FString::Printf(TEXT("Necromancer BeginPlay, Controller=%s"),
                    *GetNameSafe(GetController()))
            );
        }
    #endif

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        static TSubclassOf<UUserWidget> MinionCommandsClass;

        #if WITH_EDITOR
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(
                        -1, 3.0f, FColor::Cyan,
                        TEXT("Necromancer has PlayerController, creating UI"));
                }
        #endif

        if (!MinionCommandsClass)
        {
            MinionCommandsClass = LoadClass<UUserWidget>(
                nullptr,
                TEXT("/Game/UI/WBP_MinionCommands.WBP_MinionCommands_C")
            );
        }

        if (MinionCommandsClass)
        {
            MinionCommandsWidget = CreateWidget<UMinionCommandsWidget>(PC, MinionCommandsClass);
            if (MinionCommandsWidget)
            {
                MinionCommandsWidget->AddToViewport();
            }
        }
    }
}

void ANecromancer::ConvertTarget()
{
    FVector Start = GetActorLocation();
    FVector ForwardVector = GetActorForwardVector();
    FVector End = Start + (ForwardVector * ConversionRange);

    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);

    if (bHit)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1, 2.f, FColor::Yellow,
                FString::Printf(TEXT("ConvertTarget: Hit %s (class %s)"),
                    *GetNameSafe(Hit.GetActor()),
                    *GetNameSafe(Hit.GetActor() ? Hit.GetActor()->GetClass() : nullptr))
            );
        }

        ABaseUnit* TargetUnit = Cast<ABaseUnit>(Hit.GetActor());
        if (TargetUnit)
        {
            TargetUnit->SetTeam(ETeam::Player);
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Unit Converted!"));

            if (MinionCommandsWidget)
            {
                MinionCommandsWidget->OnConvertTriggered();
            }
        }
        else
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(
                    -1, 2.f, FColor::Red,
                    TEXT("ConvertTarget: Hit actor is NOT an ABaseUnit")
                );
            }
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1, 2.f, FColor::Red,
                TEXT("ConvertTarget: No hit")
            );
        }
    }

    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 2.0f);
}

void ANecromancer::SummonMinion()
{
    if (!MinionClass) return;

    UWorld* World = GetWorld();
    if (World)
    {
        FVector SpawnLocation = GetActorLocation() + (GetActorForwardVector() * 200.0f);
        FRotator SpawnRotation = GetActorRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        AMinion* NewMinion = World->SpawnActor<AMinion>(MinionClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (NewMinion)
        {
            NewMinion->SetTeam(ETeam::Player);
			NewMinion->SetOwnerUnit(this);
			NewMinion->SetAggroRange(GetMinionAggroRange());
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, TEXT("Minion Summoned!"));

            ControlledMinions.Add(NewMinion);

            NewMinion->OnUnitDeath.AddDynamic(this, &ANecromancer::OnControlledMinionDeath);
            
            if (NewMinion->GetMovementStrategy())
            {
                NewMinion->GetMovementStrategy()->UpdateMovement(NewMinion, this);
            }

            if (MinionCommandsWidget)
            {
                MinionCommandsWidget->OnSummonTriggered();
            }
        }
    }
}

void ANecromancer::OnControlledMinionDeath(ABaseUnit* DeadUnit)
{
    if (!DeadUnit)
    {
        return;
    }

    AMinion* DeadMinion = Cast<AMinion>(DeadUnit);
    if (!DeadMinion)
    {
        return;
    }

    ControlledMinions.Remove(DeadMinion);

    #if WITH_EDITOR
        if (GEngine)
        {
            const FString Msg = FString::Printf(
                TEXT("Necromancer: Minion %s died, remaining: %d"),
                *DeadMinion->GetName(),
                ControlledMinions.Num()
            );
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, Msg);
        }
    #endif
}

void ANecromancer::CommandMinionsIncreaseAggro()
{
    for (AMinion* Minion : ControlledMinions)
    {
        if (!IsValid(Minion))
        {
            continue;
        }

        const float NewRange = Minion->GetAggroRange() + AggroRangeStep;
        Minion->SetAggroRange(NewRange);

        if (MinionCommandsWidget)
        {
            MinionCommandsWidget->OnAttackTriggered();
            MinionCommandsWidget->SetAggroRange(NewRange);
        }
    }
}

void ANecromancer::CommandMinionsDecreaseAggro()
{
    for (AMinion* Minion : ControlledMinions)
    {
        if (!IsValid(Minion))
        {
            continue;
        }

        const float NewRange = FMath::Max(0.0f, Minion->GetAggroRange() - AggroRangeStep);
        Minion->SetAggroRange(NewRange);

        if (MinionCommandsWidget)
        {
            MinionCommandsWidget->OnDefendTriggered();
            MinionCommandsWidget->SetAggroRange(NewRange);
        }
    }
}

float ANecromancer::GetMinionAggroRange() const
{
    if (ControlledMinions.Num() == 0)
    {
        return 0.0f;
    }

    float Sum = 0.0f;
    int32 Count = 0;

    for (AMinion* Minion : ControlledMinions)
    {
        if (!IsValid(Minion))
        {
            continue;
        }

        Sum += Minion->GetAggroRange();
        ++Count;
    }

    return (Count > 0) ? (Sum / Count) : 0.0f;
}

void ANecromancer::UpdateHealthOnWidget()
{
    if (!MinionCommandsWidget)
    {
        return;
    }

    const float Current = CurrentHealth;
    const float Max = MaxHealth;

    const float Percent = (Max > 0.0f)
        ? FMath::Clamp(Current / Max, 0.0f, 1.0f)
        : 0.0f;

    MinionCommandsWidget->SetHealthPercentage(Percent);
}

void ANecromancer::OnDamaged(ABaseUnit* DamageCauser)
{
    Super::OnDamaged(DamageCauser);

    UpdateHealthOnWidget();
}