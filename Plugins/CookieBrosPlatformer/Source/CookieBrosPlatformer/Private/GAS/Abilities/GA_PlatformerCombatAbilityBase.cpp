#include "GAS/Abilities/GA_PlatformerCombatAbilityBase.h"

#include "AbilitySystemComponent.h"
#include "Animation/PlatformerAnimInstance.h"
#include "Character/PlatformerCharacterBase.h"
#include "Combat/PlatformerCombatCharacterBase.h"
#include "GAS/Attributes/PlatformerCharacterAttributeSet.h"
#include "Projectiles/Combat/CombatProjectile.h"
#include "UObject/UObjectGlobals.h"

APlatformerCombatCharacterBase* UGA_PlatformerCombatAbilityBase::GetPlatformerCombatCharacter(const FGameplayAbilityActorInfo* ActorInfo) const
{
	return ActorInfo ? Cast<APlatformerCombatCharacterBase>(ActorInfo->AvatarActor.Get()) : nullptr;
}

APlatformerCharacterBase* UGA_PlatformerCombatAbilityBase::GetPlatformerCharacter(const FGameplayAbilityActorInfo* ActorInfo) const
{
	return ActorInfo ? Cast<APlatformerCharacterBase>(ActorInfo->AvatarActor.Get()) : nullptr;
}

const UPlatformerCharacterAttributeSet* UGA_PlatformerCombatAbilityBase::GetPlatformerAttributeSet(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (const APlatformerCombatCharacterBase* Character = GetPlatformerCombatCharacter(ActorInfo))
	{
		return Character->GetPlatformerAttributeSet();
	}

	return nullptr;
}

UAbilitySystemComponent* UGA_PlatformerCombatAbilityBase::GetPlatformerAbilitySystem(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (const APlatformerCombatCharacterBase* Character = GetPlatformerCombatCharacter(ActorInfo))
	{
		return Character->GetAbilitySystemComponent();
	}

	return nullptr;
}

float UGA_PlatformerCombatAbilityBase::GetAbilityWorldTime(const FGameplayAbilityActorInfo* ActorInfo) const
{
	const AActor* AvatarActor = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
	const UWorld* World = AvatarActor ? AvatarActor->GetWorld() : nullptr;
	return World ? World->GetTimeSeconds() : 0.0f;
}

bool UGA_PlatformerCombatAbilityBase::PassesActivationDelay(const FGameplayAbilityActorInfo* ActorInfo, float DelaySeconds, float LastActivationTime) const
{
	if (DelaySeconds <= 0.0f)
	{
		return true;
	}

	return (GetAbilityWorldTime(ActorInfo) - LastActivationTime) >= DelaySeconds;
}

void UGA_PlatformerCombatAbilityBase::ExecuteGameplayCue(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTag& CueTag) const
{
	if (!CueTag.IsValid())
	{
		return;
	}

	if (UAbilitySystemComponent* AbilitySystemComponent = GetPlatformerAbilitySystem(ActorInfo))
	{
		FGameplayCueParameters CueParameters;
		if (const AActor* AvatarActor = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr)
		{
			CueParameters.Location = AvatarActor->GetActorLocation();
		}

		AbilitySystemComponent->ExecuteGameplayCue(CueTag, CueParameters);
	}
}

ACombatProjectile* UGA_PlatformerCombatAbilityBase::SpawnConfiguredCombatProjectile(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FPlatformerProjectileShotData& ShotData) const
{
	APlatformerCombatCharacterBase* Character = GetPlatformerCombatCharacter(ActorInfo);
	UWorld* World = Character ? Character->GetWorld() : nullptr;
	if (!World || !Character || !ShotData.IsValid())
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Character;
	SpawnParams.Instigator = Character;

	ACombatProjectile* Projectile = World->SpawnActor<ACombatProjectile>(
		ShotData.ProjectileClass,
		ShotData.SpawnLocation,
		ShotData.SpawnRotation,
		SpawnParams);
	if (!Projectile)
	{
		return nullptr;
	}

	Projectile->DamageEffectSpec = ShotData.DamageEffectSpec;
	Projectile->StatusEffectSpec = ShotData.StatusEffectSpec;

	if (ShotData.ProjectileMaxDistance > 0.0f)
	{
		Projectile->ApplyProjectileMaxDistance(ShotData.ProjectileMaxDistance);
	}

	if (ShotData.ProjectileSpeedOverride > 0.0f)
	{
		Projectile->ApplyDeveloperProjectileSpeed(ShotData.ProjectileSpeedOverride);
	}

	return Projectile;
}

bool UGA_PlatformerCombatAbilityBase::PerformMeleeHit(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FPlatformerMeleeHitSettings& HitSettings,
	float DamageAmount,
	TSubclassOf<UGameplayEffect> DamageEffectClass,
	float EffectLevel) const
{
	APlatformerCombatCharacterBase* Character = GetPlatformerCombatCharacter(ActorInfo);
	UWorld* World = Character ? Character->GetWorld() : nullptr;
	if (!World || !Character || !HitSettings.bEnabled || DamageAmount <= 0.0f)
	{
		return false;
	}

	const FVector ForwardVector = Character->GetActorForwardVector().GetSafeNormal2D().IsNearlyZero()
		? FVector::ForwardVector
		: Character->GetActorForwardVector().GetSafeNormal2D();
	const FVector Start = Character->GetActorLocation()
		+ FVector(0.0f, 0.0f, HitSettings.TraceVerticalOffset)
		+ ForwardVector * HitSettings.TraceForwardOffset;
	const FVector End = Start + ForwardVector * HitSettings.TraceDistance;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PlatformerMeleeHit), false, Character);

	TArray<FHitResult> HitResults;
	const bool bHit = World->SweepMultiByObjectType(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(HitSettings.TraceRadius),
		QueryParams);

	if (!bHit)
	{
		return false;
	}

	bool bAppliedAnyDamage = false;
	TSet<const AActor*> HitActors;
	for (const FHitResult& HitResult : HitResults)
	{
		AActor* TargetActor = HitResult.GetActor();
		if (!TargetActor || TargetActor == Character || HitActors.Contains(TargetActor))
		{
			continue;
		}

		HitActors.Add(TargetActor);
		if (Character->ApplyCombatDamageToActor(TargetActor, DamageAmount, HitResult, DamageEffectClass, EffectLevel))
		{
			bAppliedAnyDamage = true;
		}

		if (!HitSettings.bHitMultipleTargets)
		{
			break;
		}
	}

	return bAppliedAnyDamage;
}

UPlatformerAnimInstance* UGA_PlatformerCombatAbilityBase::GetPlatformerAnimInstance(const FGameplayAbilityActorInfo* ActorInfo) const
{
	ACharacter* Character = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	if (!Character)
	{
		return nullptr;
	}

	if (USkeletalMeshComponent* Mesh = Character->GetMesh())
	{
		return Cast<UPlatformerAnimInstance>(Mesh->GetAnimInstance());
	}

	return nullptr;
}

float UGA_PlatformerCombatAbilityBase::PlayAbilityAnimation(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTag& AnimTag,
	UAnimMontage* FallbackMontage,
	float PlayRate) const
{
	// Try data-driven lookup first
	UAnimMontage* Montage = nullptr;
	if (UPlatformerAnimInstance* AnimInstance = GetPlatformerAnimInstance(ActorInfo))
	{
		Montage = AnimInstance->ResolveAbilityMontage(AnimTag);
	}

	// Fallback to direct montage reference
	if (!Montage)
	{
		Montage = FallbackMontage;
	}

	if (!Montage)
	{
		return 0.0f;
	}

	ACharacter* Character = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	if (!Character)
	{
		return 0.0f;
	}

	return Character->PlayAnimMontage(Montage, PlayRate);
}

void UGA_PlatformerCombatAbilityBase::StopAbilityAnimation(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTag& AnimTag,
	UAnimMontage* FallbackMontage,
	float BlendOutTime) const
{
	// Try data-driven lookup first
	UAnimMontage* Montage = nullptr;
	if (UPlatformerAnimInstance* AnimInstance = GetPlatformerAnimInstance(ActorInfo))
	{
		Montage = AnimInstance->ResolveAbilityMontage(AnimTag);
	}

	// Fallback to direct montage reference
	if (!Montage)
	{
		Montage = FallbackMontage;
	}

	if (!Montage)
	{
		return;
	}

	ACharacter* Character = ActorInfo ? Cast<ACharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	if (Character)
	{
		Character->StopAnimMontage(Montage);
	}
}
