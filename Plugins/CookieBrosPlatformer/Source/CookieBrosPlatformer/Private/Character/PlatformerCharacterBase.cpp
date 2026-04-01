#include "Character/PlatformerCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/SideViewMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GAS/Attributes/PlatformerCharacterAttributeSet.h"
#include "GAS/PlatformerAbilitySet.h"
#include "GameFramework/SpringArmComponent.h"

APlatformerCharacterBase::APlatformerCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USideViewMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.0f;
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

UAbilitySystemComponent* APlatformerCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void APlatformerCharacterBase::ApplyDamage(const FGameplayEffectSpecHandle& DamageSpec, const FHitResult& HitResult)
{
	if (!AbilitySystemComponent || !DamageSpec.IsValid())
	{
		return;
	}

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*DamageSpec.Data.Get());
}

bool APlatformerCharacterBase::IsAlive() const
{
	return AttributeSet && AttributeSet->GetHealth() > 0.0f;
}

void APlatformerCharacterBase::InitializeAbilities(const UPlatformerAbilitySet* AbilitySet)
{
	if (!AbilitySystemComponent || !AbilitySet || !HasAuthority())
	{
		return;
	}

	for (const FPlatformerAbilitySet_Ability& AbilityData : AbilitySet->Abilities)
	{
		if (AbilityData.AbilityClass)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityData.AbilityClass, AbilityData.AbilityLevel, INDEX_NONE, this));
		}
	}

	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddInstigator(this, this);

	for (const FPlatformerAbilitySet_Effect& EffectData : AbilitySet->Effects)
	{
		if (!EffectData.EffectClass)
		{
			continue;
		}

		FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(EffectData.EffectClass, EffectData.EffectLevel, ContextHandle);
		if (EffectSpec.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
		}
	}
}

void APlatformerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAbilities(DefaultAbilitySet);
	}
}

void APlatformerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Input binding lives in higher-level player/controller shells.
}
