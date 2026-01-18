// Fill out your copyright notice in the Description page of Project Settings.

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Entities/Necromancer.h"
#include "Components/PlayerMovementStrategy.h"
#include "Components/InputProcessorComponent.h"

ANecromancer::ANecromancer()
{
    MovementStrategy = CreateDefaultSubobject<UPlayerMovementStrategy>(TEXT("MovementStrategy"));
    InputProcessor = CreateDefaultSubobject<UInputProcessorComponent>(TEXT("InputProcessor"));
    CurrentTeam = ETeam::Player;

    ConversionRange = 1000.0f;
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

        ABaseUnit* NewMinion = World->SpawnActor<ABaseUnit>(MinionClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (NewMinion)
        {
            NewMinion->SetTeam(ETeam::Player);
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, TEXT("Minion Summoned!"));
        }
    }
}