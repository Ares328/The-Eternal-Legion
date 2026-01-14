// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerMovementStrategy.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"

void UPlayerMovementStrategy::Move(float ForwardValue, float RightValue)
{
    ACharacter* Owner = Cast<ACharacter>(GetOwner());
    if (Owner && Owner->GetController())
    {
        // Get the direction the camera is looking
        const FRotator Rotation = Owner->GetController()->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        Owner->AddMovementInput(ForwardDirection, ForwardValue);
        Owner->AddMovementInput(RightDirection, RightValue);
    }
}
