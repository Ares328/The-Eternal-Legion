# The Eternal Legion

The Eternal Legion is a third-person action/strategy prototype built in Unreal Engine 5 using C++.  
You play as a Necromancer who commands a growing legion of minions, converts enemies to your side, and manages combat through simple, data-driven systems.

## Core Features

- **Necromancer & Minions**
  - Player-controlled Necromancer implemented in C++ (`ANecromancer`), with abilities such as summoning minions and converting enemies.
  - Minions (`AMinion`) with basic AI states (Idle, Following, Attacking, Dead), aggro ranges, and simple combat logic.

- **Data-Driven Movement**
  - Movement behavior abstracted into `UMovementStrategy` subclasses (e.g. `UPlayerMovementStrategy`, `UMinionMovementStrategy`).
  - AI minions use a dedicated movement strategy to follow the Necromancer, chase targets, and align with the overall combat flow.

- **Input & Ability System**
  - Uses Unreal’s Enhanced Input system to bind player actions (move, look, jump, summon, convert, minion commands).
  - `UInputProcessorComponent` centralizes input handling and forwards high-level events to the Necromancer and UI.

- **UI Integration**
  - UMG-based command widget (`UMinionCommandsWidget`) that reflects ability usage, minion aggro changes, and player health.
  - C++ updates the widget via clean, event-style calls (e.g. `OnSummonTriggered`, `OnConvertTriggered`, `SetAggroRange`, `SetHealthPercentage`).

- **Animation Hooks**
  - Minion animation driven by a custom AnimInstance (`UMinionAnimInstance`) that reads movement speed and AI state.
  - Designed so animation blueprints can stay focused on visuals while C++ controls state, targeting, and timing.

## Goals

- Keep gameplay logic **authoritative in C++**, while exposing clear hooks for Animation Blueprints and UI.
- Use simple, composable systems (movement strategies, minion states, input processor) to make iteration on gameplay and content easy.
- Provide a clean, readable codebase that demonstrates Unreal C++ patterns: components, strategies, AI, UI, and animation integration.

## Tech Stack

- Unreal Engine 5 (C++)
- Enhanced Input
- UMG (UI widgets)
- Animation Blueprints / AnimInstance

## Diagrams

- [Show class diagram (Google Drive)](https://drive.google.com/file/d/16IwGTBXMcH80LSXr_Y29OR6s-yR3pGuR/view?usp=sharing)
