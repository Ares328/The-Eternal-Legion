// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerMovementStrategy.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Entities/BaseUnit.h"

void UPlayerMovementStrategy::ExecuteMovement(ABaseUnit* Owner, const FVector& Direction)
{
    // The player movement logic (same as before)
    if (Owner && Direction.SizeSquared() > 0.0f)
    {
        const FRotator Rotation = Owner->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        Owner->AddMovementInput(ForwardDirection, Direction.Y);
        Owner->AddMovementInput(RightDirection, Direction.X);
    }
}