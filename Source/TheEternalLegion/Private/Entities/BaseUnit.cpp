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
	#if WITH_EDITOR
		if (GEngine)
		{
			const FString Msg = FString::Printf(
				TEXT("%s spawned with Team: %d"),
				*GetName(),
				static_cast<int32>(CurrentTeam)
			);
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, Msg);
		}
	#endif

	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	bIsDead = (CurrentHealth <= 0.0f);
}

// Called every frame
void ABaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseUnit::SetTeam(ETeam NewTeam)
{
	if (CurrentTeam != NewTeam)
	{
		CurrentTeam = NewTeam;
		OnTeamChanged(NewTeam);
	}
}

void ABaseUnit::OnTeamChanged_Implementation(ETeam NewTeam)
{
	if (NewTeam == ETeam::Player)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%s joined the Legion!"), *GetName()));
	}
}

void ABaseUnit::ApplyDamage(float DamageAmount, ABaseUnit* DamageCauser)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

	OnDamaged(DamageCauser);

	if (CurrentHealth <= 0.0f)
	{
		HandleDeath();
	}
}

void ABaseUnit::Heal(float HealAmount)
{
	if (bIsDead || HealAmount <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
}

void ABaseUnit::HandleDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	OnUnitDeath.Broadcast(this);

	Destroy();
}

void ABaseUnit::OnDamaged(ABaseUnit* DamageCauser)
{
	// Base does nothing
}