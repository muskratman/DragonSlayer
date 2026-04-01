// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlatformerInteractable.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UPlatformerInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Simple interface to allow actors to interact without knowing their internal implementation.
 */
class COOKIEBROSPLATFORMER_API IPlatformerInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Interactable")
	virtual void Interaction(AActor* Interactor) = 0;
};
