#pragma once
//
#include "Combat/PlatformerCombatCharacterBase.h"
#include "CoreMinimal.h"
#include "Developer/DeveloperPlatformerSettingsTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlatformerCharacterBase.generated.h"


class UPlatformerAbilitySet;
class UPlatformerAnimDataAsset;
class UGA_PlatformerJump;
class USpringArmComponent;
class UCameraComponent;
class ADeveloperJumpTrajectory;
class APlatformerLadder;
class APlatformerLedgeGrab;
class UDamageType;

/**
 * APlatformerCharacterBase
 * Generic playable platformer shell with camera rig, side-view movement, and
 * combat bootstrap.
 */
UCLASS(Abstract)
class COOKIEBROSPLATFORMER_API APlatformerCharacterBase
    : public APlatformerCombatCharacterBase {
  GENERATED_BODY()

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components",
            meta = (AllowPrivateAccess = "true"))
  TObjectPtr<USpringArmComponent> CameraBoom;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",
            meta = (AllowPrivateAccess = "true"))
  TObjectPtr<UCameraComponent> FollowCamera;

  UPROPERTY(EditDefaultsOnly, Category = "Abilities")
  TObjectPtr<UPlatformerAbilitySet> DefaultAbilitySet;

  UPROPERTY(EditDefaultsOnly, Category = "Developer|Jump Preview")
  TSubclassOf<ADeveloperJumpTrajectory> DeveloperJumpTrajectoryClass;

public:
  APlatformerCharacterBase(const FObjectInitializer &ObjectInitializer);

  virtual void InitializeAbilities(const UPlatformerAbilitySet *AbilitySet);
  virtual void NotifyLadderAvailable(APlatformerLadder *Ladder);
  virtual void NotifyLadderUnavailable(APlatformerLadder *Ladder);
  virtual void NotifyLedgeGrabAvailable(APlatformerLedgeGrab *LedgeGrab);
  virtual void NotifyLedgeGrabUnavailable(APlatformerLedgeGrab *LedgeGrab);
  virtual bool EnterLadder(APlatformerLadder *Ladder);
  virtual void ExitLadder(APlatformerLadder *Ladder = nullptr);
  virtual bool TryEnterAvailableLadder(float DesiredClimbInput = 1.0f);
  virtual bool HandleLadderClimbInput(float InClimbInput,
                                      float InputDeadZone = 0.0f);
  virtual bool HandleLadderHorizontalExitInput(float InHorizontalInput,
                                               float InputDeadZone = 0.0f);
  virtual void SetLadderClimbInput(float InClimbInput);
  virtual bool PerformLadderJump();
  virtual bool PerformLadderCrouch();
  UFUNCTION(BlueprintPure, Category = "Ladder|Animation")
  bool ShouldPlayLadderStartAnimation() const;
  UFUNCTION(BlueprintPure, Category = "Ladder|Animation")
  bool ShouldPlayLadderLoopAnimation() const;
  UFUNCTION(BlueprintPure, Category = "Ladder|Animation")
  bool ShouldPlayLadderEndAnimation() const;
  virtual void
  GetAvailableLedgeGrabs(TArray<APlatformerLedgeGrab *> &OutLedgeGrabs) const;
  virtual void ApplyDeveloperCharacterSettings(
      const FDeveloperPlatformerCharacterSettings &DeveloperSettings);
  virtual void ApplyDeveloperSettingsSnapshot(
      const FPlatformerDeveloperSettingsSnapshot &DeveloperSettingsSnapshot);
  virtual FDeveloperPlatformerCharacterSettings
  CaptureDeveloperCharacterSettings() const;
  virtual FPlatformerDeveloperSettingsSnapshot
  CaptureDeveloperSettingsSnapshot() const;
  virtual FVector GetPlatformerCameraFocusLocation() const;
  UFUNCTION(BlueprintCallable, Category = "Developer|Jump Preview")
  ADeveloperJumpTrajectory *SpawnJumpTrajectorySnapshotActor();
  UFUNCTION(BlueprintCallable, Category = "Developer|Debug")
  void SetPrintDeveloperCharacterStateEveryTick(bool bInPrintEveryTick);
  UFUNCTION(BlueprintCallable, Category = "Developer|Debug")
  void PrintDeveloperCharacterState() const;
  UFUNCTION(BlueprintPure, Category = "Developer|Debug")
  FString GetDeveloperCharacterStateDebugString() const;

  FORCEINLINE UCameraComponent *GetFollowCamera() const { return FollowCamera; }
  FORCEINLINE USpringArmComponent *GetCameraBoom() const { return CameraBoom; }
  FORCEINLINE UPlatformerAnimDataAsset *GetAnimDataAsset() const { return AnimDataAsset; }
  FORCEINLINE bool IsOnLadder() const { return bIsOnLadder; }
  FORCEINLINE APlatformerLadder *GetActiveLadder() const {
    return ActiveLadder;
  }
  FORCEINLINE APlatformerLadder *GetAvailableLadder() const {
    return AvailableLadder;
  }
  UFUNCTION(BlueprintPure, Category = "Ladder|Movement")
  float GetLadderClimbInput() const;
  FORCEINLINE bool HasDeveloperCrouchCapsuleScaleOverride() const {
    return bHasDeveloperCrouchCapsuleScaleOverride;
  }
  FORCEINLINE bool HasDeveloperJumpHorizontalSpeedOverride() const {
    return bHasDeveloperJumpHorizontalSpeedOverride;
  }
  float
  ResolveDeveloperCrouchCapsuleScale(float DefaultCrouchCapsuleScale) const;
  float
  ResolveDeveloperJumpHorizontalSpeed(float DefaultJumpHorizontalSpeed) const;
  bool HasActiveDeveloperCombatSettings() const;
  const FDeveloperPlatformerCombatSettings &
  GetActiveDeveloperCombatSettings() const;

protected:
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  virtual void
  SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;
  virtual void FellOutOfWorld(const UDamageType &DamageType) override;
  virtual void OnCombatDeath(AActor *DamageInstigatorActor) override;
  virtual void OnCombatRevived() override;
  virtual void OnEnteredLadder(APlatformerLadder *Ladder);
  virtual void OnExitedLadder(APlatformerLadder *Ladder);
  virtual void ApplyDeveloperCameraSettings(
      const FDeveloperPlatformerCameraSettings &DeveloperCameraSettings);
  virtual FDeveloperPlatformerCameraSettings
  CaptureDeveloperCameraSettings() const;
  virtual void ApplyDeveloperCharacterMovementSettings(
      const FDeveloperPlatformerCharacterMovementSettings
          &DeveloperCharacterMovementSettings);
  virtual FDeveloperPlatformerCharacterMovementSettings
  CaptureDeveloperCharacterMovementSettings() const;
  virtual void ApplyDeveloperCombatSettings(
      const FDeveloperPlatformerCombatSettings &DeveloperCombatSettings);
  virtual FDeveloperPlatformerCombatSettings
  CaptureDeveloperCombatSettings() const;
  virtual float GetHealthWidgetVerticalPadding() const override;
  void SetShowJumpTrajectoryPreview(bool bInShowJumpTrajectoryPreview);
  ADeveloperJumpTrajectory *EnsureDeveloperJumpTrajectoryActor();
  void DestroyDeveloperJumpTrajectoryActor();
  void SetDeveloperCrouchCapsuleScaleOverride(float InCrouchCapsuleScale);
  void ClearDeveloperCrouchCapsuleScaleOverride();
  void SetDeveloperJumpHorizontalSpeedOverride(float InJumpHorizontalSpeed);
  void ClearDeveloperJumpHorizontalSpeedOverride();
  float CaptureDeveloperCrouchCapsuleScale() const;
  float CaptureDeveloperJumpHorizontalSpeed() const;
  float ResolveDefaultCrouchCapsuleScale() const;
  float ResolveStandingCapsuleHalfHeight() const;
  void ApplyResolvedCrouchCapsuleScale();
  void UpdateActiveLadderMovement(float DeltaTime);
  void UpdateLadderAnimationState(float DeltaTime);
  void SnapCharacterToActiveLadder();
  void ExitActiveLadderToFalling(APlatformerLadder *Ladder = nullptr);
  bool IsCharacterAtOrBelowLadderBottom(const APlatformerLadder *Ladder) const;
  float GetCharacterFeetWorldZ() const;
  float GetWorldTimeSafe() const;
  void BeginLadderStartAnimationState();
  void BeginLadderLoopAnimationState();
  void BeginLadderEndAnimationState();
  void ClearLadderAnimationState();
  const UGA_PlatformerJump *FindGrantedJumpAbility() const;
  void RefreshJumpTrajectoryPreview();
  FDeveloperPlatformerCombatSettings
  ResolveDeveloperCombatSettingsForApplication(
      const FDeveloperPlatformerCombatSettings &DeveloperCombatSettings) const;
  void LoadAndApplyDeveloperSettings();
  void
  SetHasActiveDeveloperCombatSettings(bool bInHasActiveDeveloperCombatSettings);

  UPROPERTY(Transient)
  TObjectPtr<APlatformerLadder> ActiveLadder;

  UPROPERTY(Transient)
  TObjectPtr<APlatformerLadder> AvailableLadder;

  UPROPERTY(Transient)
  TArray<TWeakObjectPtr<APlatformerLedgeGrab>> AvailableLedgeGrabs;

  UPROPERTY(Transient)
  bool bIsOnLadder = false;

  UPROPERTY(Transient)
  float SavedPreLadderGravityScale = 1.0f;

  UPROPERTY(Transient)
  TEnumAsByte<EMovementMode> SavedPreLadderMovementMode = MOVE_Walking;

  UPROPERTY(Transient)
  bool bHadPreLadderGravityOverride = false;

  UPROPERTY(Transient)
  float PreLadderGravityOverride = 1.0f;

  UPROPERTY(Transient)
  float LadderClimbInput = 0.0f;

  UPROPERTY(Transient)
  float LadderJumpEndTime = -1.0f;

  UPROPERTY(Transient)
  float LadderCrouchEndTime = -1.0f;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ladder",
            meta = (ClampMin = 0.0, Units = "cm"))
  float LadderBottomExitTolerance = 4.0f;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ladder|Animation",
            meta = (ClampMin = 0.0, Units = "s"))
  float LadderStartStateDuration = 0.16f;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ladder|Animation",
            meta = (ClampMin = 0.0, Units = "s"))
  float LadderEndStateDuration = 0.12f;

  UPROPERTY(BlueprintReadOnly, Transient, Category = "Ladder|Animation")
  bool bShouldLadderStart = false;

  UPROPERTY(BlueprintReadOnly, Transient, Category = "Ladder|Animation")
  bool bShouldLadderLoop = false;

  UPROPERTY(BlueprintReadOnly, Transient, Category = "Ladder|Animation")
  bool bShouldLadderEnd = false;

  UPROPERTY(Transient)
  float LadderStartStateTimeRemaining = 0.0f;

  UPROPERTY(Transient)
  float LadderEndStateTimeRemaining = 0.0f;

  UPROPERTY(Transient)
  bool bHasActiveDeveloperCombatSettings = false;

  UPROPERTY(Transient)
  FDeveloperPlatformerCombatSettings ActiveDeveloperCombatSettings;

  UPROPERTY(Transient)
  bool bHasDeveloperCrouchCapsuleScaleOverride = false;

  UPROPERTY(Transient)
  float DeveloperCrouchCapsuleScaleOverride = 0.0f;

  UPROPERTY(Transient)
  bool bHasDeveloperJumpHorizontalSpeedOverride = false;

  UPROPERTY(Transient)
  float DeveloperJumpHorizontalSpeedOverride = 0.0f;

  UPROPERTY(Transient)
  bool bShowJumpTrajectoryPreview = false;

  UPROPERTY(Transient)
  TObjectPtr<ADeveloperJumpTrajectory> DeveloperJumpTrajectoryActor;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Developer|Debug")
  bool bPrintDeveloperCharacterStateEveryTick = true;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI",
            meta = (ClampMin = 0.0, Units = "cm"))
  float PlatformerHealthWidgetVerticalPadding = 20.0f;

  /** Data-driven animation mapping for ability montages. */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
  TObjectPtr<UPlatformerAnimDataAsset> AnimDataAsset;

  /** Subclass of AnimBP to link as anim layers at BeginPlay (optional). */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
  TSubclassOf<UAnimInstance> LinkedAnimLayerClass;
};
