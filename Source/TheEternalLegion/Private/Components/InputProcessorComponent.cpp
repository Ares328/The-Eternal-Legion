
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Entities/Necromancer.h"
#include "Components/InputProcessorComponent.h"

UInputProcessorComponent::UInputProcessorComponent(const FObjectInitializer& ObjectInitializer)
{

}

void UInputProcessorComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<ANecromancer>(GetOwner());

    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("InputProcessorComponent: FAILED to cast owner to ABaseCharacter. Input will not work."));
        return; // Stop execution if cast fails
    }

    UE_LOG(LogTemp, Warning, TEXT("InputProcessorComponent: Successfully found ABaseCharacter owner. Binding inputs..."));
    if (OwnerCharacter->GetMovementStrategy())
    {
        UE_LOG(LogTemp, Warning, TEXT("BeginPlay Check: MovementStrategy is ALIVEE! Address: %p"), OwnerCharacter->GetMovementStrategy());
        SetupInputBindings();
    }
    else
    {
        // This confirms deletion happened immediately after constructor
        UE_LOG(LogTemp, Error, TEXT("BeginPlay Check: MovementStrategy is NULL! GC deleted it BEFORE BeginPlay2."));
    }
}

void UInputProcessorComponent::SetupInputBindings()
{
    APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PlayerController) return;

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultIMC, 0);
    }

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
    {
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UInputProcessorComponent::HandleMoveInput);
        }

        if (JumpAction)
        {
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &UInputProcessorComponent::HandleJumpInput);
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &UInputProcessorComponent::HandleJumpInput);
        }
        if (LookAction)
        {
            EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &UInputProcessorComponent::HandleLookInput);
        }
    }
}

void UInputProcessorComponent::HandleMoveInput(const FInputActionValue& Value)
{
    if (OwnerCharacter)
    {
        UMovementStrategy* CurrentStrategy = Cast<UMovementStrategy>(OwnerCharacter->GetMovementStrategy());
        if (CurrentStrategy) // <--- THIS IS THE CHECK
        {
            FVector2D MovementVector = Value.Get<FVector2D>();
            FVector Direction(MovementVector.X, MovementVector.Y, 0.0f);
            // Delegate to the concrete strategy (e.g., UPlayerMovementStrategy)
            CurrentStrategy->ExecuteMovement(OwnerCharacter, Direction);
        }
        else
        {
            // CRITICAL: Log if the pointer is null
            UE_LOG(LogTemp, Error, TEXT("InputProcessorComponent: MovementStrategy is NULL at input time! Expected address: %p"), OwnerCharacter->GetMovementStrategy());
        }
    }
    else
    {
        // Log if the OwnerCharacter is suddenly null
        UE_LOG(LogTemp, Error, TEXT("InputProcessorComponent: OwnerCharacter is NULL during input execution!"));
    }
}

void UInputProcessorComponent::HandleJumpInput(const FInputActionValue& Value)
{
    // Jump is handled by the base ACharacter function, triggered by the component
    if (OwnerCharacter)
    {
        if (OwnerCharacter->GetMovementStrategy())
        {
            // SUCCESS PATH
            UE_LOG(LogTemp, Warning, TEXT("Jump Check: MovementStrategy is ALIVEE! Address: %p"), OwnerCharacter->GetMovementStrategy());

            // Execute the character's jump logic
            OwnerCharacter->Jump();
        }
        else
        {
            // FAILURE PATH (The pointer is NULL)
            UE_LOG(LogTemp, Error, TEXT("Jump Check: MovementStrategy is NULL! GC has deleted it."));
        }
        if (Value.Get<bool>())
        {
            OwnerCharacter->Jump();
        }
        else
        {
            OwnerCharacter->StopJumping();
        }
    }
}

void UInputProcessorComponent::HandleLookInput(const FInputActionValue& Value)
{
    if (OwnerCharacter)
    {
        FVector2D LookVector = Value.Get<FVector2D>();
        // Apply yaw (turn) and pitch (look up/down) input
        OwnerCharacter->AddControllerYawInput(LookVector.X);
        OwnerCharacter->AddControllerPitchInput(LookVector.Y);
    }
}