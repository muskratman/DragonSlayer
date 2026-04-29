# Context: PlatformerSettings Editor Panel

## Purpose

`PlatformerSettings` is an editor-only quick settings panel for selected platformer actors. It gives designers a smaller details surface for common tuning values, then pushes those values back to the selected actor.

This system is not the runtime developer settings save/load panel. Runtime developer settings live in `WBP_DeveloperSettings` and `UPlatformerDeveloperSettingsWidget`. `PlatformerSettings` edits level/editor actors directly.

## Ownership

Reusable editor tooling lives in `Plugins/CookieBrosPlatformer`:

- `Source/CookieBrosLevelEditor/Private/PlatformerSettings/SPlatformerSettingsWidget.h`
- `Source/CookieBrosLevelEditor/Private/PlatformerSettings/SPlatformerSettingsWidget.cpp`
- `Source/CookieBrosLevelEditor/Private/PlatformerSettings/PlatformerSettingsObjects.h`
- `Source/CookieBrosLevelEditor/Private/PlatformerSettings/PlatformerSettingsObjects.cpp`
- `Source/CookieBrosLevelEditor/Private/CookieBrosLevelEditor.cpp` registers the tab.

Runtime actor properties live in `Source/CookieBrosPlatformer/Public` and `Private` actor classes. The editor settings objects should remain a facade over those actor properties.

## Opening Flow

1. `FCookieBrosLevelEditorModule` registers the `CookieBros.PlatformerSettings` nomad tab.
2. `OpenPlatformerSettingsTab()` opens the tab.
3. `SpawnPlatformerSettingsTab()` creates `SPlatformerSettingsWidget`.
4. The widget listens to editor selection changes.
5. When exactly one supported actor is selected, it creates a transient `UPlatformerActorSettingsObject` subclass.
6. The settings object pulls values from the actor and is shown in an `IDetailsView`.
7. When a property changes, `HandleSettingsFinishedChanging()` opens a transaction, pushes settings to the actor, and refreshes from selection.

## Main Widget Class

`SPlatformerSettingsWidget` owns the editor UI shell:

- creates the details view
- listens to `USelection::SelectionChangedEvent`
- validates single-actor selection
- creates the right transient settings object
- shows a selection summary
- pushes settings after details changes

The widget should stay generic. Actor-specific behavior belongs in settings object subclasses.

## Settings Object Model

All quick settings objects inherit from:

```text
UPlatformerActorSettingsObject
```

Required methods:

- `PullFromActor(AActor* Actor)`
- `PushToActor()`

The base class stores the selected actor as a weak pointer. Subclasses expose `UPROPERTY(EditAnywhere, Category="Quick Settings")` fields so the details view can render them.

Most environment settings use reflection helpers in `PlatformerSettingsPrivate`:

- `GetTypedPropertyValue`
- `SetTypedPropertyValue`
- `GetVectorPropertyValue`
- `SetVectorPropertyValue`
- `MoveActorInEditor`
- `MoveSceneComponentInEditor`
- `ResolveSceneComponentProperty`

Use direct typed actor APIs when a setting is not a simple reflected property or when pushing should also apply runtime state.

## Supported Actors

Environment actors:

- `APlatformerConveyor`
- `APlatformerDangerBlock`
- `APlatformerDestructibleBlock`
- `APlatformerFallingPlatform`
- `APlatformerGravityVolume`
- `APlatformerHazardProjectile`
- `APlatformerJumpPad`
- `APlatformerMovingPlatform`
- `APlatformerSlipperyBlock`
- `APlatformerSpikes`
- `APlatformerStream`
- `APlatformerSwitch`
- `APlatformerTeleporter`
- `APlatformerTriggeredLift`
- `APlatformerVanishingBlock`
- `APlatformerWallTurret`
- `APlatformerYokuBlocks`

Enemy actors:

- `APlatformerEnemyBase`
- `APlatformerEnemyRanged`

The creation order matters. More specific subclasses must be checked before their base classes. For example, `APlatformerEnemyRanged` is resolved before `APlatformerEnemyBase`, and `APlatformerTriggeredLift` is resolved before `APlatformerMovingPlatform`.

## Enemy Quick Settings

`UPlatformerEnemySettingsObject` edits reusable enemy actor settings:

- `Health` default `100`
- `MovementSpeed` default `300 cm/s`
- `Damage` default `10`
- `HitDelay` default `1s`
- `PatrolDelayTime` endpoint delay, default `0s`
- `EnablePlayerChase` default `false`
- `ChaseAgroRadius` default `300 cm`
- `PatrolPoints` as relative offsets from the enemy's start location

`UPlatformerRangedEnemySettingsObject` adds:

- `ProjectileSpeed` default `500 cm/s`
- `ProjectileDistance` default `600 cm`

The runtime source of truth is `APlatformerEnemyBase` and `APlatformerEnemyRanged`. `PushToActor()` uses their public setters so editor edits also refresh the enemy's runtime attributes, movement component speed, projectile data, and patrol data.

`APlatformerEnemyBase` still supports `UPlatformerEnemyArchetypeAsset`. Archetype data is treated as a template: it fills enemy values that are still at class defaults, while instance-edited quick settings can stay local to a placed actor.

## Enemy Runtime Details

Enemy settings are applied to:

- `UPlatformerCharacterAttributeSet::MaxHealth`
- `Health`
- `BaseDamage`
- `MeleeAttackDamage`
- `RangeBaseAttackDamage`
- `AttackSpeed`
- `MeleeAttackDelay`
- `RangeAttackDelay`
- `MoveSpeed`
- `UCharacterMovementComponent::MaxWalkSpeed`
- `MaxFlySpeed`

`HitDelay` is designer-facing seconds between hits. `AttackSpeed` remains an attribute and is derived as:

```text
AttackSpeed = HitDelay > 0 ? 1 / HitDelay : 0
```

`PatrolPoints` are stored as relative offsets from the enemy's start location. Runtime patrol resolves each point as `PatrolOriginLocation + PatrolPoints[Index]`, where `PatrolOriginLocation` is captured from the actor location on `BeginPlay`. Native patrol movement only runs when patrol is enabled, the array is not empty, the enemy is alive, and the enemy has no active combat target.

Patrol follows a ping-pong route through the array, not a closed loop. For example, `A -> B -> C -> B -> A`. `PatrolDelayTime` is applied only at endpoint indices `0` and `Num - 1`.

Walking enemies patrol along X only. Flying enemies can patrol along X/Z. The Y axis is ignored to keep enemies locked to the side-view gameplay plane.

Enemy patrol preview uses editor-only visualization on the actor: a spline through the relative patrol points plus sphere markers for each point.

Enemy capsules ignore movement collision with other `APlatformerEnemyBase` actors, so enemies can cross each other even on the same Y plane. This does not change player-vs-enemy collision responses.

## Common Pitfalls

- Do not add gameplay logic to `SPlatformerSettingsWidget`; add typed APIs to actors or settings object subclasses.
- Reflection helpers silently fail when a property is renamed. Prefer direct actor APIs for new systems that need runtime side effects.
- Always check subclasses before base classes in `CreateSettingsObjectForActor()`.
- `PullFromActor()` should be cheap and should not mutate the actor.
- `PushToActor()` should call `Modify()` before changing editor actor data.
- If a setting moves an actor or component, use the editor helpers so transforms and editor state update correctly.
- Do not confuse this panel with the runtime developer settings Save/Load flow.

## Adding A New Supported Actor

1. Add a `UPlatformer...SettingsObject` subclass in `PlatformerSettingsObjects.h`.
2. Implement `PullFromActor()` and `PushToActor()` in `PlatformerSettingsObjects.cpp`.
3. Include the actor header in `SPlatformerSettingsWidget.cpp`.
4. Add a branch in `CreateSettingsObjectForActor()`.
5. Put subclass checks before base-class checks.
6. Use reflection helpers for simple editor properties and direct actor APIs for settings with runtime side effects.
7. Rebuild the editor module and test single-selection, multi-selection, unsupported actor selection, undo transaction, and actor refresh.

## Quick Read Map For Agents

Start here:

- `SPlatformerSettingsWidget::RefreshFromSelection`
- `SPlatformerSettingsWidget::CreateSettingsObjectForActor`
- `SPlatformerSettingsWidget::HandleSettingsFinishedChanging`
- `UPlatformerActorSettingsObject`
- `PlatformerSettingsPrivate::SetTypedPropertyValue`
- `UPlatformerEnemySettingsObject::PushToActor`
- `UPlatformerRangedEnemySettingsObject::PushToActor`

The safest extension path is to mirror a nearby settings object, then move any non-trivial runtime application into the actor class itself.
