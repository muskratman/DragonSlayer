#include "Traversal/PlatformerTraversalComponent.h"

#include "GameFramework/Character.h"
#include "Traversal/PlatformerTraversalConfigDataAsset.h"
#include "Traversal/PlatformerTraversalMovementComponent.h"

UPlatformerTraversalComponent::UPlatformerTraversalComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlatformerTraversalComponent::OnRegister()
{
	Super::OnRegister();
	ApplyTraversalSettings();
}

void UPlatformerTraversalComponent::BeginPlay()
{
	Super::BeginPlay();
	ApplyTraversalSettings();
}

void UPlatformerTraversalComponent::SetTraversalEnabled(bool bInTraversalEnabled)
{
	bTraversalEnabled = bInTraversalEnabled;
	ApplyTraversalSettings();
}

void UPlatformerTraversalComponent::SetTraversalConfig(UPlatformerTraversalConfigDataAsset* InTraversalConfig)
{
	TraversalConfig = InTraversalConfig;
	ApplyTraversalSettings();
}

void UPlatformerTraversalComponent::SetDeveloperTraversalSettingsOverride(
	const FPlatformerLedgeTraversalSettings& InLedgeSettings,
	const FPlatformerDashSettings& InDashSettings,
	const FPlatformerWallTraversalSettings& InWallSettings)
{
	bHasDeveloperTraversalSettingsOverride = true;
	DeveloperLedgeSettingsOverride = InLedgeSettings;
	DeveloperDashSettingsOverride = InDashSettings;
	DeveloperWallSettingsOverride = InWallSettings;
	ApplyTraversalSettings();
}

void UPlatformerTraversalComponent::ClearDeveloperTraversalSettingsOverride()
{
	bHasDeveloperTraversalSettingsOverride = false;
	ApplyTraversalSettings();
}

UPlatformerTraversalMovementComponent* UPlatformerTraversalComponent::GetTraversalMovementComponent() const
{
	const ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	return CharacterOwner ? Cast<UPlatformerTraversalMovementComponent>(CharacterOwner->GetCharacterMovement()) : nullptr;
}

void UPlatformerTraversalComponent::ApplyTraversalSettings()
{
	if (UPlatformerTraversalMovementComponent* TraversalMovementComponent = GetTraversalMovementComponent())
	{
		TraversalMovementComponent->SetTraversalConfig(TraversalConfig);
		TraversalMovementComponent->SetDefaultLedgeSettings(DefaultLedgeSettings);
		TraversalMovementComponent->SetDefaultDashSettings(DefaultDashSettings);
		TraversalMovementComponent->SetDefaultWallSettings(DefaultWallSettings);

		if (bHasDeveloperTraversalSettingsOverride)
		{
			TraversalMovementComponent->SetDeveloperTraversalSettingsOverride(
				DeveloperLedgeSettingsOverride,
				DeveloperDashSettingsOverride,
				DeveloperWallSettingsOverride);
		}
		else
		{
			TraversalMovementComponent->ClearDeveloperTraversalSettingsOverride();
		}

		TraversalMovementComponent->SetTraversalEnabled(bTraversalEnabled);
	}
}

FPlatformerLedgeTraversalSettings UPlatformerTraversalComponent::GetResolvedLedgeSettings() const
{
	if (bHasDeveloperTraversalSettingsOverride)
	{
		return DeveloperLedgeSettingsOverride;
	}

	return TraversalConfig ? TraversalConfig->LedgeSettings : DefaultLedgeSettings;
}

FPlatformerDashSettings UPlatformerTraversalComponent::GetResolvedDashSettings() const
{
	if (bHasDeveloperTraversalSettingsOverride)
	{
		return DeveloperDashSettingsOverride;
	}

	return TraversalConfig ? TraversalConfig->DashSettings : DefaultDashSettings;
}

FPlatformerWallTraversalSettings UPlatformerTraversalComponent::GetResolvedWallSettings() const
{
	if (bHasDeveloperTraversalSettingsOverride)
	{
		return DeveloperWallSettingsOverride;
	}

	return TraversalConfig ? TraversalConfig->WallSettings : DefaultWallSettings;
}
