#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * IInteractable
 * Generic interaction contract for platformer actors.
 */
class COOKIEBROSPLATFORMER_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	bool CanInteract(AActor* Interactor) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	FText GetInteractionPrompt() const;
};
