// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/MovementStrategy.h"
#include "BaseUnit.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	Neutral     UMETA(DisplayName = "Neutral"),
	Player      UMETA(DisplayName = "Necromancer Ally"),
	Enemy       UMETA(DisplayName = "Enemy")
};

UCLASS()
class THEETERNALLEGION_API ABaseUnit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseUnit();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | Stats")
	ETeam CurrentTeam;

	UFUNCTION(BlueprintNativeEvent, Category = "Legion | Events")
	void OnTeamChanged(ETeam NewTeam);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Legion | Movement", Instanced)
	TObjectPtr<UMovementStrategy> MovementStrategy;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// The main function to call when converting a unit
	UFUNCTION(BlueprintCallable, Category = "Legion | Actions")
	void SetTeam(ETeam NewTeam);

	// Getter for other systems to check allegiance
	UFUNCTION(BlueprintCallable, Category = "Legion | Stats")
	ETeam GetTeam() const { return CurrentTeam; }

	UMovementStrategy* GetMovementStrategy() const { return MovementStrategy.Get(); }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
