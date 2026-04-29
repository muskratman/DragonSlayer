#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "PlatformerAnimInstance.generated.h"

class APlatformerCharacterBase;
class UAbilitySystemComponent;
class UPlatformerAnimDataAsset;
class UPlatformerTraversalMovementComponent;
class UAnimMontage;

/**
 * UPlatformerAnimInstance
 * Base C++ AnimInstance for platformer characters.
 *
 * Automatically reads movement, traversal, and combat state from
 * the owning character and its GAS AbilitySystemComponent every frame.
 *
 * Provides data-driven montage playback API used by Gameplay Abilities
 * to resolve animations through UPlatformerAnimDataAsset.
 *
 * Projects create Blueprint AnimBPs with this as the parent class.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API UPlatformerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// === Data-Driven Montage API ===

	/**
	 * Play an ability montage resolved from AnimData by GameplayTag.
	 * @param AnimTag - Tag to look up in AnimDataAsset
	 * @param PlayRate - Playback speed multiplier
	 * @return Duration of the montage, or 0 if not found/played
	 */
	UFUNCTION(BlueprintCallable, Category="Platformer|Animation")
	float PlayAbilityMontage(FGameplayTag AnimTag, float PlayRate = 1.0f);

	/**
	 * Stop an ability montage resolved from AnimData by GameplayTag.
	 * @param AnimTag - Tag to look up in AnimDataAsset
	 * @param BlendOutTime - Blend-out duration
	 */
	UFUNCTION(BlueprintCallable, Category="Platformer|Animation")
	void StopAbilityMontage(FGameplayTag AnimTag, float BlendOutTime = 0.25f);

	/**
	 * Resolve a montage from AnimData by GameplayTag without playing it.
	 * @return The montage, or nullptr if not found
	 */
	UFUNCTION(BlueprintPure, Category="Platformer|Animation")
	UAnimMontage* ResolveAbilityMontage(const FGameplayTag& AnimTag) const;

protected:
	/** Update movement-related properties from the owning character. */
	virtual void UpdateMovementProperties(float DeltaSeconds);

	/** Update gameplay tag-driven state properties from the ASC. */
	virtual void UpdateGameplayTagProperties();

	/** Update derived transition helpers after raw movement/tag values are refreshed. */
	virtual void UpdateDerivedStateProperties();

	/** Update dash phase helpers from traversal runtime data. */
	virtual void UpdateDashStateProperties(float DeltaSeconds);

	/** Update ladder phase helpers from the owning character runtime data. */
	virtual void UpdateLadderStateProperties();

	/** Update ledge grab phase helpers from traversal runtime data. */
	virtual void UpdateLedgeGrabStateProperties(float DeltaSeconds);

	/** Update montage-driven ability state helpers from AnimData mappings. */
	virtual void UpdateAbilityMontageProperties();

	// === Movement State ===

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement")
	float GroundSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement")
	float VerticalVelocity = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement")
	bool bIsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement")
	bool bIsCrouching = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement")
	bool bIsOnLadder = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement")
	float LadderClimbInput = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement")
	float MovementDirectionX = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement")
	bool bHasAcceleration = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Platformer|Movement", meta=(ClampMin="0.0", Units="cm/s"))
	float MovementStateSpeedThreshold = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement|Derived")
	bool bShouldIdle = true;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement|Derived")
	bool bShouldMove = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement|Derived")
	bool bShouldJump = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement|Derived")
	bool bShouldFall = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement|Derived")
	bool bShouldCrouchIdle = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Movement|Derived")
	bool bShouldCrouchMove = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Platformer|Traversal|Dash", meta=(ClampMin="0.0", ClampMax="1.0"))
	float DashStartProgressThreshold = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Platformer|Traversal|Dash", meta=(ClampMin="0.0", Units="s"))
	float DashEndStateDuration = 0.12f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Dash")
	float DashElapsedTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Dash")
	float DashExpectedDuration = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Dash")
	float DashTimeAlpha = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Dash")
	float DashDistanceAlpha = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Dash")
	float DashProgressAlpha = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Derived")
	bool bShouldDashStart = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Derived")
	bool bShouldDashLoop = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Derived")
	bool bShouldDashEnd = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Derived")
	bool bShouldLadderStart = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Derived")
	bool bShouldLadderLoop = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Derived")
	bool bShouldLadderEnd = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Platformer|Traversal|Ledge", meta=(ClampMin="0.0", Units="s"))
	float LedgeGrabStartStateDuration = 0.16f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Platformer|Traversal|Ledge", meta=(ClampMin="0.0", Units="s"))
	float LedgeGrabEndStateDuration = 0.12f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Ledge")
	float LedgeGrabEndElapsedTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Ledge")
	float LedgeGrabEndExpectedDuration = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Ledge")
	float LedgeGrabEndTimeAlpha = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Derived")
	bool bShouldLedgeGrabStart = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Derived")
	bool bShouldLedgeGrabLoop = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Derived")
	bool bShouldLedgeGrabEnd = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Derived")
	bool bShouldWallSlide = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal|Derived")
	bool bShouldWallJump = false;

	// === Traversal State (from Gameplay Tags) ===

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal")
	bool bIsDashing = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal")
	bool bIsWallSliding = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal")
	bool bIsWallJumping = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal")
	bool bIsLedgeHanging = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Traversal")
	bool bIsLedgeClimbing = false;

	// === Combat State (from Gameplay Tags) ===

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Combat")
	bool bIsCharging = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Combat")
	bool bIsChargePartial = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Combat")
	bool bIsChargeFull = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Combat")
	bool bIsDead = false;

	// === Ability Animation State (from AnimData montage playback) ===

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Ability Animation")
	bool bIsMeleeAttacking = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Ability Animation")
	bool bIsRangedAttacking = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Ability Animation")
	bool bIsMeleeCharging = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Ability Animation")
	bool bIsRangedCharging = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Ability Animation")
	bool bIsHitReacting = false;

	UPROPERTY(BlueprintReadOnly, Category="Platformer|Ability Animation")
	bool bIsPlayingAbilityMontage = false;

	// === Animation Data ===

	/** Data asset providing GameplayTag → AnimMontage mapping. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Platformer|Data")
	TObjectPtr<UPlatformerAnimDataAsset> AnimData;

private:
	/** Initialize AnimData from the owning character's AnimDataAsset, if available. */
	void CacheAnimDataFromCharacter();
	bool IsAbilityMontagePlaying(const FGameplayTag& AnimTag) const;
	bool IsAnyAbilityMontagePlaying() const;

	UPROPERTY(Transient)
	TObjectPtr<APlatformerCharacterBase> CachedCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> CachedASC;

	UPROPERTY(Transient)
	TObjectPtr<UPlatformerTraversalMovementComponent> CachedTraversalMovementComponent;

	bool bCachedDataInitialized = false;
	bool bWasDashingLastFrame = false;
	bool bWasLedgeHangingLastFrame = false;
	bool bWasLedgeClimbingLastFrame = false;
	float DashEndStateTimeRemaining = 0.0f;
	float LedgeGrabStartStateTimeRemaining = 0.0f;
	float LedgeGrabEndStateTimeRemaining = 0.0f;
	float PreviousFrameVerticalVelocity = 0.0f;
};
