#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/Damageable.h"
#include "PlatformerCharacterBase.generated.h"

class UAbilitySystemComponent;
class UPlatformerAbilitySet;
class UPlatformerCharacterAttributeSet;
class USpringArmComponent;
class UCameraComponent;

/**
 * APlatformerCharacterBase
 * Generic playable platformer shell with camera rig, side-view movement, and GAS bootstrap.
 */
UCLASS(Abstract)
class COOKIEBROSPLATFORMER_API APlatformerCharacterBase : public ACharacter, public IAbilitySystemInterface, public IDamageable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCameraComponent> FollowCamera;

protected:
	UPROPERTY()
	TObjectPtr<UPlatformerCharacterAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	TObjectPtr<UPlatformerAbilitySet> DefaultAbilitySet;

public:
	APlatformerCharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void ApplyDamage(const FGameplayEffectSpecHandle& DamageSpec, const FHitResult& HitResult) override;
	virtual bool IsAlive() const override;

	virtual void InitializeAbilities(const UPlatformerAbilitySet* AbilitySet);

	FORCEINLINE UPlatformerCharacterAttributeSet* GetPlatformerAttributeSet() const { return AttributeSet; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
