// Copyright CookieBros. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class ATileMapActor;

/**
 * FTileMapActorDetails
 * Custom Details panel for ATileMapActor.
 * Adds buttons for Rebuild, Export JSON, Import JSON, and layer management.
 */
class FTileMapActorDetails : public IDetailCustomization
{
public:
	/** Factory method for registration. */
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	/** Cached weak pointer to the actor being customized. */
	TWeakObjectPtr<ATileMapActor> CachedActor;

	/** Button handlers. */
	FReply OnRebuildClicked();
	FReply OnExportJsonClicked();
	FReply OnImportJsonClicked();
	FReply OnEnsureDefaultLayersClicked();
};
