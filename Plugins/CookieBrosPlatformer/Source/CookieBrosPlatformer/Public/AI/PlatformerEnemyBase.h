#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/Damageable.h"
#include "PlatformerEnemyBase.generated.h"

class UAbilitySystemComponent;
class UAIPerceptionComponent;
class UPlatformerCharacterAttributeSet;
class UPlatformerEnemyArchetypeAsset;
class UStateTreeComponent;
struct FOnAttributeChangeData;

/**
 * APlatformerEnemyBase
 * Generic GAS-aware enemy shell with perception and StateTree hooks.
 */
UCLASS(Abstract)
class COOKIEBROSPLATFORMER_API APlatformerEnemyBase : public ACharacter, public IAbilitySystemInterface, public IDamageable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStateTreeComponent> StateTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAIPerceptionComponent> PerceptionComponent;

protected:
	UPROPERTY()
	TObjectPtr<UPlatformerCharacterAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category="Archetype")
	TObjectPtr<UPlatformerEnemyArchetypeAsset> DefaultArchetype;

	float StaggerThreshold = 2.0f;

public:
	APlatformerEnemyBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void ApplyDamage(const FGameplayEffectSpecHandle& DamageSpec, const FHitResult& HitResult) override;
	virtual bool IsAlive() const override;

	virtual void InitializeFromArchetype(const UPlatformerEnemyArchetypeAsset* Archetype);

	FORCEINLINE UPlatformerCharacterAttributeSet* GetPlatformerAttributeSet() const { return AttributeSet; }
	FORCEINLINE UStateTreeComponent* GetStateTreeComponent() const { return StateTreeComponent; }
	FORCEINLINE UAIPerceptionComponent* GetPerceptionComponent() const { return PerceptionComponent; }

protected:
	virtual void BeginPlay() override;

	virtual void OnHealthChanged(const FOnAttributeChangeData& Data);
	virtual void HandleDeath();
};
