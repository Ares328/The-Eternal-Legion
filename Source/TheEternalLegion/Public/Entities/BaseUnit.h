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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDeathSignature, ABaseUnit*, Unit);

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

	// health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legion | Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Legion | Stats")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Legion | Stats")
	bool bIsDead = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void HandleDeath();

public:
	// The main function to call when converting a unit
	UFUNCTION(BlueprintCallable, Category = "Legion | Actions")
	void SetTeam(ETeam NewTeam);

	// Getter for other systems to check allegiance
	UFUNCTION(BlueprintCallable, Category = "Legion | Stats")
	ETeam GetTeam() const { return CurrentTeam; }

	UMovementStrategy* GetMovementStrategy() const { return MovementStrategy.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Legion | Stats")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Legion | Stats")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Legion | Stats")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "Legion | Combat")
	virtual void ApplyDamage(float DamageAmount, ABaseUnit* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Legion | Combat")
	virtual void Heal(float HealAmount);

	UPROPERTY(BlueprintAssignable, Category = "Legion | Events")
	FOnUnitDeathSignature OnUnitDeath;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
