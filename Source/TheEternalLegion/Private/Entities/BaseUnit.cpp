// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/BaseUnit.h"

// Sets default values
ABaseUnit::ABaseUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Default team enemy
	CurrentTeam = ETeam::Enemy;

}

// Called when the game starts or when spawned
void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseUnit::SetTeam(ETeam NewTeam)
{
	if (CurrentTeam != NewTeam)
	{
		CurrentTeam = NewTeam;
		OnTeamChanged(NewTeam);
	}
}

void ABaseUnit::OnTeamChanged(ETeam NewTeam)
{
	if (NewTeam == ETeam::Player)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%s joined the Legion!"), *GetName()));
	}
}

