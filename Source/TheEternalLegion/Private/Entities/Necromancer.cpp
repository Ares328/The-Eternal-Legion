// Fill out your copyright notice in the Description page of Project Settings.

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Entities/Necromancer.h"
#include "Entities/Minion.h"
#include "Components/PlayerMovementStrategy.h"
#include "AIController.h"
#include "Components/InputProcessorComponent.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

ANecromancer::ANecromancer()
{
    MovementStrategy = CreateDefaultSubobject<UPlayerMovementStrategy>(TEXT("MovementStrategy"));
    InputProcessor = CreateDefaultSubobject<UInputProcessorComponent>(TEXT("InputProcessor"));
    CurrentTeam = ETeam::Player;

    ConversionRange = 1000.0f;
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
            if (UUserWidget* Widget = CreateWidget<UUserWidget>(PC, MinionCommandsClass))
            {
                Widget->AddToViewport();
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

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
    {
        ABaseUnit* TargetUnit = Cast<ABaseUnit>(Hit.GetActor());
        if (TargetUnit)
        {
            TargetUnit->SetTeam(ETeam::Player);
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Unit Converted!"));
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
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, TEXT("Minion Summoned!"));

            ControlledMinions.Add(NewMinion);

            NewMinion->OnUnitDeath.AddDynamic(this, &ANecromancer::OnControlledMinionDeath);
            
            if (NewMinion->GetMovementStrategy())
            {
                NewMinion->GetMovementStrategy()->UpdateMovement(NewMinion, this);
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
    }
}