#include "AI/PlatformerEnemyBase.h"

#include "AbilitySystemComponent.h"
#include "Components/StateTreeComponent.h"
#include "Data/PlatformerEnemyArchetypeAsset.h"
#include "GAS/Attributes/PlatformerCharacterAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"

APlatformerEnemyBase::APlatformerEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTree"));

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));

	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = 2000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	PerceptionComponent->ConfigureSense(*SightConfig);

	UAISenseConfig_Damage* DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	DamageConfig->SetMaxAge(5.0f);
	PerceptionComponent->ConfigureSense(*DamageConfig);
	PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

UAbilitySystemComponent* APlatformerEnemyBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void APlatformerEnemyBase::ApplyDamage(const FGameplayEffectSpecHandle& DamageSpec, const FHitResult& HitResult)
{
	if (!AbilitySystemComponent || !DamageSpec.IsValid() || !IsAlive() || !AttributeSet)
	{
		return;
	}

	const float PreHealth = AttributeSet->GetHealth();
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*DamageSpec.Data.Get());

	const float PostHealth = AttributeSet->GetHealth();
	const float ActualDamage = PreHealth - PostHealth;

	if (!IsAlive())
	{
		HandleDeath();
	}
	else if (ActualDamage >= StaggerThreshold)
	{
		FGameplayEventData EventData;
		EventData.EventMagnitude = ActualDamage;
		AbilitySystemComponent->HandleGameplayEvent(FGameplayTag::RequestGameplayTag(TEXT("Event.Hit.Received")), &EventData);
	}
}

bool APlatformerEnemyBase::IsAlive() const
{
	return AttributeSet && AttributeSet->GetHealth() > 0.0f;
}

void APlatformerEnemyBase::InitializeFromArchetype(const UPlatformerEnemyArchetypeAsset* Archetype)
{
	if (!Archetype || !AbilitySystemComponent || !AttributeSet)
	{
		return;
	}

	AttributeSet->SetMaxHealth(Archetype->BaseHealth);
	AttributeSet->SetHealth(Archetype->BaseHealth);
	AttributeSet->SetBaseDamage(Archetype->BaseDamage);
	AttributeSet->SetAttackSpeed(Archetype->AttackSpeed);
	AttributeSet->SetMoveSpeed(Archetype->MoveSpeed);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = Archetype->MoveSpeed;
	}

	if (StateTreeComponent && Archetype->BehaviorTree)
	{
		StateTreeComponent->SetStateTree(Archetype->BehaviorTree);
		StateTreeComponent->StartLogic();
	}

	if (HasAuthority())
	{
		for (const TSubclassOf<UGameplayAbility>& AbilityClass : Archetype->Abilities)
		{
			if (AbilityClass)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, this));
			}
		}
	}
}

void APlatformerEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeFromArchetype(DefaultArchetype);
	}
}

void APlatformerEnemyBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f)
	{
		HandleDeath();
	}
}

void APlatformerEnemyBase::HandleDeath()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")));
	}
}
