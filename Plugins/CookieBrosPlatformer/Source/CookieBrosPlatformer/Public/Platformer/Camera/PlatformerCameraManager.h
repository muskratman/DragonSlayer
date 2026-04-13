// Copyright Epic Games, Inc. All Rights Reserved.
//
#pragma once

#include "Camera/PlayerCameraManager.h"
#include "CoreMinimal.h"
#include "Developer/DeveloperPlatformerSettingsTypes.h"
#include "PlatformerCameraManager.generated.h"


class APawn;

/** Camera follow state for the dead zone / follow camera state machine. */
enum class EPlatformerCameraFollowState : uint8 {
  /** Camera does not follow — character moves freely inside the dead zone
     rectangle. */
  DeadZone,
  /** Camera follows the character with directional look-ahead offset. */
  Following
};

/**
 *  Simple platformer camera with smooth scrolling and movement-based
 * look-ahead.
 */
UCLASS()
class COOKIEBROSPLATFORMER_API APlatformerCameraManager
    : public APlayerCameraManager {
  GENERATED_BODY()

public:
  virtual void BeginPlay() override;

  /** Overrides the default camera view target calculation */
  virtual void UpdateViewTarget(FTViewTarget &OutVT, float DeltaTime) override;

  virtual void ApplyDeveloperCameraManagerSettings(
      const FDeveloperPlatformerCameraManagerSettings
          &DeveloperCameraManagerSettings);
  virtual FDeveloperPlatformerCameraManagerSettings
  CaptureDeveloperCameraManagerSettings() const;

protected:
  /** Resolves the baseline camera pose from the pawn's actual camera setup in
   * the Blueprint hierarchy. */
  void ResolveBaseCameraPose(const APawn &TargetPawn, FVector &OutLocation,
                             FRotator &OutRotation, float &OutFOV) const;
  FVector ResolveCameraFocusLocation(const APawn &TargetPawn) const;
  bool IsOrthographicProjectionEnabled() const;
  void SyncProjectionSettings(APawn &TargetPawn);

  /** Converts current movement into a normalized look-ahead direction in the
   * visible platforming plane. */
  FVector2D ResolveMovementDirection2D(const FVector &Velocity) const;
  void LoadAndApplyDeveloperCameraManagerSettings();

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Platformer Camera|Projection")
  EPlatformerCameraProjectionMode CameraProjectionMode =
      EPlatformerCameraProjectionMode::Perspective;

  /** How close we want to stay to the view target */
  UPROPERTY(EditAnywhere, Category = "Platformer Camera",
            meta = (ClampMin = 0, ClampMax = 10000, Units = "cm"))
  float CurrentZoom = 1000.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Platformer Camera|Projection",
            meta = (ClampMin = 1.0, ClampMax = 100000.0, Units = "cm"))
  float OrthographicWidth = 2048.0f;

  /** How far above the target do we want the camera to focus */
  UPROPERTY(EditAnywhere, Category = "Platformer Camera",
            meta = (ClampMin = 0, ClampMax = 10000, Units = "cm"))
  float CameraZOffset = 100.0f;

  /** Default rotation used only if the pawn has no camera-related components.
   */
  UPROPERTY(EditAnywhere, Category = "Platformer Camera",
            meta = (ClampMin = -180.0f, ClampMax = 180.0f))
  FRotator FallbackRotation = FRotator(0.0f, -90.0f, 0.0f);

  /** Movement speeds below this threshold are treated as idle to avoid tiny
   * physics jitter offsets. */
  UPROPERTY(EditAnywhere, Category = "Platformer Camera|Smoothing",
            meta = (ClampMin = 0.0f, ClampMax = 500.0f, Units = "cm/s"))
  float IdleSpeedThreshold = 5.0f;

  /** How much the camera is offset from the character based on movement
   * direction (look-ahead) */
  UPROPERTY(EditAnywhere, Category = "Platformer Camera|Smoothing",
            meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
  float HorizontalOffset = 500.0f;

  /** How fast the camera shifts horizontally when horizontal movement begins
   * (idle -> moving) */
  UPROPERTY(EditAnywhere, Category = "Platformer Camera|Smoothing",
            meta = (ClampMin = 0.0f, ClampMax = 100.0f))
  float HorizontalOffsetInterpSpeedStart = 6.0f;

  /** How fast the camera shifts horizontally back to idle when horizontal
   * movement ends (moving -> idle) */
  UPROPERTY(EditAnywhere, Category = "Platformer Camera|Smoothing",
            meta = (ClampMin = 0.0f, ClampMax = 100.0f))
  float HorizontalOffsetInterpSpeedEnd = 2.0f;

  /** How much the camera is offset vertically in the movement direction (jump /
   * fall look-ahead) */
  UPROPERTY(EditAnywhere, Category = "Platformer Camera|Smoothing",
            meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
  float VerticalOffset = 350.0f;

  /** How fast the camera shifts vertically when the character starts or stops
   * moving up/down quickly */
  UPROPERTY(EditAnywhere, Category = "Platformer Camera|Smoothing",
            meta = (ClampMin = 0.0f, ClampMax = 100.0f))
  float VerticalOffsetInterpSpeed = 2.0f;

  /** How fast the camera adjusts vertically to instant changes in capsule
   * height (like crouching) */
  UPROPERTY(EditAnywhere, Category = "Platformer Camera|Smoothing",
            meta = (ClampMin = 0.0f, ClampMax = 100.0f))
  float CrouchInterpSpeed = 8.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Platformer Camera|Dead Zone",
            meta = (ClampMin = 0.0, ClampMax = 100000.0, Units = "cm"))
  float CameraDeadZoneWidth = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Platformer Camera|Dead Zone",
            meta = (ClampMin = 0.0, ClampMax = 100000.0, Units = "cm"))
  float CameraDeadZoneHeight = 0.0f;

  /** Outer bounding box width. The character cannot visually exceed this
   * distance horizontally on the screen, forcing the camera to rigidly push.
   * Set to 0 to disable. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Platformer Camera|Bound Box",
            meta = (ClampMin = 0.0, ClampMax = 100000.0, Units = "cm"))
  float CameraBoundBoxWidth = 0.0f;

  /** Outer bounding box height. The character cannot visually exceed this
   * distance vertically on the screen, forcing the camera to rigidly push. Set
   * to 0 to disable. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "Platformer Camera|Bound Box",
            meta = (ClampMin = 0.0, ClampMax = 100000.0, Units = "cm"))
  float CameraBoundBoxHeight = 0.0f;

protected:
  /** Current interpolated horizontal offset for look-ahead functionality */
  float CurrentHorizontalOffset = 0.0f;

  /** Current interpolated vertical offset for look-ahead functionality */
  float CurrentVerticalOffset = 0.0f;

  /** Smoothed camera rig offset from the camera focus point. */
  FVector CurrentCameraRigOffset = FVector::ZeroVector;

  UPROPERTY(Transient)
  TWeakObjectPtr<APawn> DeadZoneTrackedPawn;

  /** Current dead zone / follow camera state. */
  EPlatformerCameraFollowState CameraFollowState =
      EPlatformerCameraFollowState::DeadZone;

  /** Center of the dead zone rectangle in world space. Fixed during DeadZone
   * state, tracks character during Following. */
  UPROPERTY(Transient)
  FVector DeadZoneCenter = FVector::ZeroVector;

  /** Character's displacement from camera center due to dead zone.
   * In DeadZone state: tracks character offset from DeadZoneCenter (camera
   * stays at center). In Following state: smoothly interpolates to zero (camera
   * catches up to character). */
  FVector CurrentDeadZoneOffset = FVector::ZeroVector;

  /** Smoothed vertical focus Z — prevents camera snap on capsule height changes
   * (crouch/stand). */
  float SmoothedFocusLocationZ = 0.0f;

  UPROPERTY(Transient)
  bool bIsDeadZoneInitialized = false;

  UPROPERTY(Transient)
  bool bHasCameraRigOffset = false;
};
