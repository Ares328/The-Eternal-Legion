// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Minion.h"

AMinion::AMinion()
{
	CurrentTeam = ETeam::Enemy;
	AggroRange = 500.0f;
}

void AMinion::OnTeamChanged_Implementation(ETeam NewTeam)
{

	Super::OnTeamChanged_Implementation(NewTeam);
}