#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputProcessorComponent.generated.h"

class ANecromancer;
class UInputAction;
struct FInputActionValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEETERNALLEGION_API UInputProcessorComponent : public UActorComponent
{
    GENERATED_BODY()

public:	
	UInputProcessorComponent(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    ANecromancer* OwnerCharacter;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Legion | Input |Actions")
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Legion | Input | Actions")
    UInputAction* JumpAction;

    UPROPERTY(EditDefaultsOnly, Category = "Legion | Input | Actions")
    UInputAction* LookAction;

    UPROPERTY(EditDefaultsOnly, Category = "Legion | Input | Actions")
    class UInputMappingContext* DefaultIMC;

    void HandleMoveInput(const FInputActionValue& Value);
    void HandleJumpInput(const FInputActionValue& Value);
    void HandleLookInput(const FInputActionValue& Value);

    void SetupInputBindings();
};