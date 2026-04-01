#include "AI/BossBase.h"
#include "GAS/Attributes/EnemyAttributeSet.h"

ABossBase::ABossBase()
{
	AttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));
}
