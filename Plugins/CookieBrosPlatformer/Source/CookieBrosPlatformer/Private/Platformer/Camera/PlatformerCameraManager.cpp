// Copyright Epic Games, Inc. All Rights Reserved.

#include "Platformer/Camera/PlatformerCameraManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

void APlatformerCameraManager::ResolveBaseCameraPose(const APawn& TargetPawn, FVector& OutLocation, FRotator& OutRotation, float& OutFOV) const
{
	OutLocation = TargetPawn.GetActorLocation() + FVector(0.0f, CurrentZoom, CameraZOffset);
	OutRotation = FallbackRotation;
	OutFOV = 65.0f;

	if (const UCameraComponent* CameraComp = TargetPawn.FindComponentByClass<UCameraComponent>())
	{
		OutLocation = CameraComp->GetComponentLocation();
		OutRotation = CameraComp->GetComponentRotation();
		OutFOV = CameraComp->FieldOfView;
		return;
	}

	if (const USpringArmComponent* SpringArm = TargetPawn.FindComponentByClass<USpringArmComponent>())
	{
		OutLocation = SpringArm->GetSocketLocation(USpringArmComponent::SocketName);
		OutRotation = SpringArm->GetSocketRotation(USpringArmComponent::SocketName);
	}
}

FVector2D APlatformerCameraManager::ResolveMovementDirection2D(const FVector& Velocity) const
{
	const FVector2D MovementInPlane(Velocity.X, Velocity.Z);
	return MovementInPlane.GetSafeNormal();
}

void APlatformerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	APawn* TargetPawn = Cast<APawn>(OutVT.Target);
	if (!IsValid(TargetPawn))
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		return;
	}

	FVector BaseCameraLocation = FVector::ZeroVector;
	FRotator BaseCameraRotation = FRotator::ZeroRotator;
	float TargetFOV = 65.0f;
	ResolveBaseCameraPose(*TargetPawn, BaseCameraLocation, BaseCameraRotation, TargetFOV);

	OutVT.POV.Rotation = BaseCameraRotation;
	OutVT.POV.FOV = TargetFOV;

	const FVector Velocity = TargetPawn->GetVelocity();
	float TargetHorizontalLookAhead = 0.0f;
	float TargetVerticalLookAhead = 0.0f;

	if (!Velocity.IsNearlyZero(IdleSpeedThreshold))
	{
		const FVector2D MovementDirection = ResolveMovementDirection2D(Velocity);
		TargetHorizontalLookAhead = MovementDirection.X * HorizontalOffset;
		TargetVerticalLookAhead = MovementDirection.Y * VerticalOffset;
	}

	const float HorizontalInterpSpeed = FMath::IsNearlyZero(TargetHorizontalLookAhead, KINDA_SMALL_NUMBER)
		? HorizontalOffsetInterpSpeedEnd
		: HorizontalOffsetInterpSpeedStart;
	CurrentHorizontalOffset = FMath::FInterpTo(CurrentHorizontalOffset, TargetHorizontalLookAhead, DeltaTime, HorizontalInterpSpeed);
	CurrentVerticalOffset = FMath::FInterpTo(CurrentVerticalOffset, TargetVerticalLookAhead, DeltaTime, VerticalOffsetInterpSpeed);

	FVector FinalCameraLocation = BaseCameraLocation + FVector(CurrentHorizontalOffset, 0.0f, CurrentVerticalOffset);
	FinalCameraLocation.X = FMath::Clamp(FinalCameraLocation.X, CameraXMinBounds, CameraXMaxBounds);

	OutVT.POV.Location = FinalCameraLocation;
}
