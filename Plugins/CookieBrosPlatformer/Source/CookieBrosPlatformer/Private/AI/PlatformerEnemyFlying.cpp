#include "AI/PlatformerEnemyFlying.h"

#include "GameFramework/CharacterMovementComponent.h"

APlatformerEnemyFlying::APlatformerEnemyFlying()
{
	// Flying enemies ignore gravity and stay in MOVE_Flying by default.
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->GravityScale = 0.0f;
		MovementComponent->DefaultLandMovementMode = MOVE_Flying;
	}
}
