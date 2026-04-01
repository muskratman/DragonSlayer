#include "Character/DragonCharacter.h"
#include "Character/DragonFormComponent.h"
#include "Character/DragonOverdriveComponent.h"
#include "GAS/Attributes/DragonAttributeSet.h"

ADragonCharacter::ADragonCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AttributeSet = CreateDefaultSubobject<UDragonAttributeSet>(TEXT("AttributeSet"));

	FormComponent = CreateDefaultSubobject<UDragonFormComponent>(TEXT("FormComponent"));
	OverdriveComponent = CreateDefaultSubobject<UDragonOverdriveComponent>(TEXT("OverdriveComponent"));
}
