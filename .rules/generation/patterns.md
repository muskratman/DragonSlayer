# Patterns: BurningCORE UE5 C++

## Pattern 1: Production Character

```cpp
// ✅ Production pawn наслідує gameplay base і додає input glue
UCLASS()
class APlayableDragonCharacter : public ADragonCharacter
{
    GENERATED_BODY()
    // Production input bindings, pawn-specific tuning
};

// ❌ Паралельний legacy pawn з дубльованим movement/input стеком
```

## Pattern 2: UE5 Interface

```cpp
// ✅ Мінімальний інтерфейс — pure virtual
UINTERFACE(MinimalAPI, NotBlueprintable)
class UCombatDamageable : public UInterface
{
    GENERATED_BODY()
};

class ICombatDamageable
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Damageable")
    virtual void ApplyDamage(float Damage, AActor* DamageCauser,
        const FVector& DamageLocation, const FVector& DamageImpulse) = 0;
};

// ❌ Інтерфейс з реалізацією або з полями
```

## Pattern 3: EnhancedInput — Do*() delegation

```cpp
// ✅ Input callback → public Do*() метод
void APlayableDragonCharacter::Input_Move(const FInputActionValue& Value)
{
    const float MoveValue = Value.Get<float>();
    AddMovementInput(FVector(1.0f, 0.0f, 0.0f), MoveValue);
}

// ❌ Розкидати production input logic по кількох legacy pawn-класах
void Move(const FInputActionValue& Value)
{
    // дублювання input glue
}
```

## Pattern 4: UPROPERTY — категорії та meta

```cpp
// ✅ Повна специфікація
UPROPERTY(EditAnywhere, Category="Melee Attack|Damage",
    meta = (ClampMin = 0, ClampMax = 100))
float MeleeDamage = 1.0f;

UPROPERTY(EditAnywhere, Category="Camera",
    meta = (ClampMin = 0, ClampMax = 1000, Units = "cm"))
float DeathCameraDistance = 400.0f;

// ❌ Без категорії, без clamp, без default
UPROPERTY(EditAnywhere)
float MeleeDamage;
```

## Pattern 5: Component getters

```cpp
// ✅ FORCEINLINE getter
FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

// ✅ Private component з AllowPrivateAccess
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components",
    meta = (AllowPrivateAccess = "true"))
USpringArmComponent* CameraBoom;
```

## Pattern 6: AnimNotify для gameplay events

```cpp
// ✅ Окремий AnimNotify клас на кожну подію
class UAnimNotify_DoAttackTrace : public UAnimNotify  // trace
class UAnimNotify_CheckCombo : public UAnimNotify     // combo window
class UAnimNotify_CheckChargedAttack : public UAnimNotify  // charge check

// ❌ Один God-AnimNotify з enum для типу
```

## Pattern 7: Blueprint hooks (C++ → BP)

```cpp
// ✅ BlueprintImplementableEvent для візуальних ефектів
UFUNCTION(BlueprintImplementableEvent, Category="Combat")
void DealtDamage(float Damage, const FVector& ImpactPoint);

UFUNCTION(BlueprintImplementableEvent, Category="Combat")
void ReceivedDamage(float Damage, const FVector& ImpactPoint, const FVector& DamageDirection);

// ❌ Хардкод VFX/SFX в C++
```

## Anti-Patterns

| ❌ Не роби | ✅ Замість | ЧОМУ |
|---|---|---|
| Логіку Variant в base клас | Наслідуй і override | Масштабованість |
| Hard ref на assets в C++ | BP derived + BlueprintImplementableEvent | Гнучкість |
| BehaviorTree (в цьому проекті) | StateTree | Вибір архітектора |
| God-class з усіма механіками | Interface + composition | Розв'язка |
| Raw pointer без forward decl | Forward declaration в .h | Компіляція |
