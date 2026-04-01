#include "AI/PlatformerEnemyFlying.h"

#include "GameFramework/CharacterMovementComponent.h"

APlatformerEnemyFlying::APlatformerEnemyFlying()
{
	// Flying enemies ignore gravity and stay in MOVE_Flying by default.
	if (UCharacterMovementComponent* CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->GravityScale = 0.0f;
		CharacterMovement->DefaultLandMovementMode = MOVE_Flying;
	}
}
