#include "Platformer/Environment/PlatformerEnvironmentHelpers.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/SceneComponent.h"
#include "GAS/Attributes/PlatformerCharacterAttributeSet.h"
#include "GAS/Effects/PlatformerDamageGameplayEffect.h"
#include "GAS/PlatformerGameplayTags.h"
#include "GameplayEffect.h"
#include "Interfaces/Damageable.h"

namespace PlatformerEnvironment
{
	void ApplyRelativeTransform(
		USceneComponent* Component,
		const FVector& BaseLocation,
		const FRotator& BaseRotation,
		const FVector& BaseScale,
		const FPlatformerComponentTransformOffset& TransformOffset)
	{
		if (Component == nullptr)
		{
			return;
		}

		const FVector FinalScale = BaseScale * TransformOffset.RelativeScale3D;
		const FQuat FinalRotation = BaseRotation.Quaternion() * TransformOffset.RelativeRotation.Quaternion();

		Component->SetRelativeLocation(BaseLocation + TransformOffset.RelativeLocation);
		Component->SetRelativeRotation(FinalRotation);
		Component->SetRelativeScale3D(FinalScale);
	}

	bool ApplyConfiguredDamage(AActor* SourceActor, AActor* TargetActor, TSubclassOf<UGameplayEffect> DamageEffectClass, float DirectDamage, const FHitResult& HitResult)
	{
		if (!TargetActor || TargetActor == SourceActor)
		{
			return false;
		}

		IAbilitySystemInterface* AbilityTarget = Cast<IAbilitySystemInterface>(TargetActor);
		if (!AbilityTarget)
		{
			return false;
		}

		UAbilitySystemComponent* TargetASC = AbilityTarget->GetAbilitySystemComponent();
		if (!TargetASC)
		{
			return false;
		}

		UAbilitySystemComponent* SourceASC = nullptr;
		if (IAbilitySystemInterface* AbilitySource = Cast<IAbilitySystemInterface>(SourceActor))
		{
			SourceASC = AbilitySource->GetAbilitySystemComponent();
		}

		TSubclassOf<UGameplayEffect> ResolvedDamageEffectClass = DamageEffectClass;
		if (!ResolvedDamageEffectClass && DirectDamage > 0.0f)
		{
			ResolvedDamageEffectClass = UPlatformerDamageGameplayEffect::StaticClass();
		}

		if (ResolvedDamageEffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = SourceASC ? SourceASC->MakeEffectContext() : TargetASC->MakeEffectContext();
			ContextHandle.AddInstigator(SourceActor, SourceActor);
			ContextHandle.AddHitResult(HitResult);

			FGameplayEffectSpecHandle DamageSpec =
				(SourceASC ? SourceASC : TargetASC)->MakeOutgoingSpec(ResolvedDamageEffectClass, 1.0f, ContextHandle);
			if (DamageSpec.IsValid())
			{
				if (DirectDamage > 0.0f)
				{
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
						DamageSpec,
						PlatformerGameplayTags::Data_Combat_Damage,
						DirectDamage);
				}

				if (IDamageable* DamageableTarget = Cast<IDamageable>(TargetActor))
				{
					DamageableTarget->ApplyDamage(DamageSpec, HitResult);
				}
				else
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*DamageSpec.Data.Get());
				}

				return true;
			}
		}

		if (DirectDamage <= 0.0f)
		{
			return false;
		}

		const FGameplayAttribute HealthAttribute = UPlatformerCharacterAttributeSet::GetHealthAttribute();
		if (!TargetASC->HasAttributeSetForAttribute(HealthAttribute))
		{
			return false;
		}

		TargetASC->ApplyModToAttribute(HealthAttribute, EGameplayModOp::Additive, -DirectDamage);
		return true;
	}
}
