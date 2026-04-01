# Architecture: BurningCORE

## Project Shape

3D side-scrolling action platformer з production-first архітектурою.
Єдиний C++ модуль `BurningCORE` з core/gameplay шарами та legacy variant-слідами.
Production path проходить через `Core + Character + GAS + AI + Systems + Platformer`.

## Production Structure

```
Source/BurningCORE/
├── BurningCOREGameMode     ← abstract base GameMode
├── BurningCOREPlayerController ← abstract base Controller
├── Platformer/
│   ├── Character/          ← production pawn + platformer interaction contracts
│   └── Environment/        ← moving platforms, pickups, jump pads, soft platforms
├── UI/                     ← runtime gameplay UI + pause menu
├── Platformer/Base/        ← platformer shell: mode + controller
└── Platformer/Camera/      ← platformer camera shell
```

## Layers

| Шар | Відповідальність |
|---|---|
| **Core** | GameMode, shared controller, game state, save/load |
| **Character** | ADragonCharacter, form/overdrive components, side-view movement |
| **Platformer** | PlayableDragonCharacter, platformer interaction contracts, environment actors |
| **AI** | StateTree, GameplayStateTree, EQS contexts, AIController |
| **Gameplay** | checkpoints, interactables, environment actors |
| **Interfaces** | UE5 interfaces (UINTERFACE): IDamageable, IInteractable, platformer interaction contracts |
| **Animation** | AnimNotify (не AnimBP — BP-only) |
| **UI** | UMG widgets (C++ base + BP layout) |

## Key Decisions

- **Production-first layering** — gameplay будується навколо `ADragonCharacter`, `AEnemyBase` і `Platformer/*`. ЧОМУ: менше дублювання та один активний production path.
- **Interfaces замість hard dependencies** — слабка зв'язність між character/environment/UI шарами. ЧОМУ: простіше підтримувати shell та gameplay окремо.
- **EnhancedInput** з Do*() паттерном (Move→DoMove, Look→DoLook). ЧОМУ: дозволяє UI та AI викликати ту ж логіку без InputAction.
- **StateTree для AI** замість BehaviorTree. ЧОМУ: вибір розробника для UE 5.7+.
- **C++ base + BP derived** для Characters та UI. ЧОМУ: логіка в C++ для агента, візуали/ассети в BP.
- **PublicIncludePaths** перераховані в Build.cs. ЧОМУ: flat includes між варіантами.
