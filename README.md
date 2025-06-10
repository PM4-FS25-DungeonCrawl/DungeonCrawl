# DungeonCrawl

## API Documentation

A full API reference generated from the code comments with doxygen can be found [here](https://pm4-fs25-dungeoncrawl.github.io/DungeonCrawl/)

## Repo statistics
The number of lines of source code written.
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                               60           1209           1223           5770
C/C++ Header                    55            385            845           1251
Meson                            1             40              8            142
-------------------------------------------------------------------------------
SUM:                           116           1634           2076           7163
-------------------------------------------------------------------------------
```

we will use then C2X standard with Meson as our build tool and for unit testing

indentation will be 4 spaces

regular C naming conventions


# Scrum people
Jil & Nino

# Manual 
Lucien

# Decision Record
## Why write the game in C?
We decided to use C because not being able to rely on objects and built-in features of modern programming languages was an interesting challenge.

Having to use pointers and structs instead of objects led to a lot of challenges, but it was an interesting learning opportunity nonetheless. That being said, in the future we would rather use a modern object-oriented language like Go for a project like this. 

## Why use SQL for saving gamestates?
Primarily just because we wanted to use SQL, but also because saving the gamestate in a .txt or something similar would make it very easy for the player to manipulate their game.

Using SQL was definitely more time consuming and required more effort than a simpler solution, but we think it was worth it.

## Why use property files for localization?
Initially, we used SQL for localization as well but later decided to switch to property files for a number of reasons:
- Localization took way too long to load.
- It was difficult to quickly add new text.
Property files provided a much simpler solution that was both faster to expand and easier to intergrate into our program.

## Why use notcurses?
Because notcurses is one of the best and most-used terminal I/O libraries for C. We started with the library termbox but later switched to notcurses because termbox has no Windows support and is very limited in functionality.

# Definition of Done
- code runs and does what it is supposed to do
- code is reviewed
- has positive and negative unit tests
- test coverage of at least 80% for the modules where it makes sense
- can build on all platforms
- fulfills clean code standards
- code is documented

# TestCoverage
The testcoverage can be checked <a href="https://raw.githack.com/PM4-FS25-DungeonCrawl/DungeonCrawl/refs/heads/develop/documents/coverage/coverage-report.html" target="_blank">here</a>.

# Development Progress
![Sprint 1](documents/burndownchart/sprint_1.svg)
![Sprint 2](documents/burndownchart/sprint_2.svg)
![Sprint 3](documents/burndownchart/sprint_3.svg)
![Sprint 4](documents/burndownchart/sprint_4.svg)

# Project Documentation

## Architecture Diagram
This architecture diagram gives an overview of the different modules that our codebase consists of.
```mermaid
graph TD
    A[User Input] --> B((Game Loop))
    B --> C{Game State}
    C -->|Main Menu| D[Main Menu Module]
    C -->|Map Mode| E[Map Module]
    C -->|Combat| F[Combat Module]
    C -->|Inventory| G[Inventory Module]
    C -->|Stats| H[Stats Module]

    E --> I[Map Generation]
    E --> J[Map Populator]
    E --> K[Lighting Engine]

    F --> L[Abilities]
    F --> M[Damage Calculation]
    F --> N[Characters]

    G --> O[Inventory UI]
    G --> P[Item Handling]

    H --> Q[Player Stats Display]

    D --> R[Save/Load Menu]
    R --> S[Gamestate Database]

    B --> T[Logger Module]
    T --> U[Ring Buffer]
    T --> V[Log File Writer Thread]

    B --> W[Memory Manager]

    B --> X[Notcurses Renderer]
    X --> Y[Terminal Output]

    S --> Z[SQLite Backend]
```
## Data Flow Diagram
This diagram shows the general flow of data within our application.
```mermaid
graph TD
    subgraph External
        UserInput[(User Input)]
        TerminalOutput[(Terminal Output)]
    end

    subgraph System
        A[Game Loop] -->|Switch State| B{Game State}
        B -->|Map Mode| C[Map Generator]
        B -->|Combat Mode| D[Combat Engine]
        B -->|Inventory Mode| E[Inventory System]
        B -->|Stats Mode| F[Stats Display]
        B -->|Main Menu| G[Save/Load Manager]

        C --> H[In-Memory Game State]
        D --> H
        E --> H
        F --> H
        G --> H

        H -->|Render Data| I[Notcurses Renderer]
        I --> TerminalOutput

        UserInput --> A
        A --> J[Logger Module]
        J --> K[Ring Buffer / Log Queue]
        K --> L[Log Writer Thread]
        L --> M[Log File Storage]

        G --> N[SQLite Database]
        N --> G

        D --> O[Character Data]
        O --> D

        E --> P[Potion & Gear Data]
        P --> E

        C --> Q[Randomized Map Data]
        Q --> C
    end
```
## State Diagram
High level overview of the game flow. The game flow is mostly implemented via multiple nested 
```mermaid
stateDiagram-v2
    [*] --> MainMenu

    MainMenu --> NewGame : Start New Game
    MainMenu --> LoadGame : Load Game

    NewGame --> MapMode
    LoadGame --> MapMode

    MapMode --> InventoryMode : Open Inventory
    MapMode --> StatsMode : View Stats
    MapMode --> CombatMode : Encounter Monster
    MapMode --> SaveMenu : Save Game
    MapMode --> MainMenu : Exit to Main Menu

    InventoryMode --> MapMode : Back to Map
    StatsMode --> MapMode : Back to Map

    CombatMode --> CombatMenu
    CombatMenu --> AbilityMenu : Use Ability
    CombatMenu --> PotionMenu : Use Potion
    CombatMenu --> EvaluateCombat

    AbilityMenu --> CombatMenu
    PotionMenu --> CombatMenu
    EvaluateCombat --> PlayerWon : Player Wins
    EvaluateCombat --> PlayerLost : Player Defeated
    EvaluateCombat --> CombatMenu : Continue Combat

    PlayerWon --> MapMode : Return to Map
    PlayerLost --> GameOver
    GameOver --> MainMenu

    SaveMenu --> MapMode : Back to Map

```
# Entity Relation Diagrams
## Gamestate Diagram
```mermaid
erDiagram
    Game_State ||--o{ GS_ID : has
    Game_State ||--o{ GS_SAVEDTIME : has
    Game_State ||--o{ Map_State : "IS A"
    Game_State ||--o{ Player_State : "IS A"

    Map_State ||--o{ MS_ID : has
    Map_State ||--o{ MS_MAP : has
    Map_State ||--o{ MS_REVEALED : has
    Map_State ||--o{ MS_HEIGHT : has
    Map_State ||--o{ MS_WIDTH : has
    Map_State ||--o{ MS_GS_ID : has

    Player_State ||--o{ PS_ID : has
    Player_State ||--o{ PS_X : has
    Player_State ||--o{ PS_Y : has
    Player_State ||--o{ PS_GS_ID : has
```
## Items and Abilities Diagram
```mermaid
erDiagram
    Game_State ||--o{ GS_ID : has
    Game_State ||--o{ GS_SAVEDTIME : has
    Game_State ||--o{ Player : "IS A"
    Game_State ||--o{ Character : "IS A"
    Game_State ||--o{ Monster : "IS A"
    Game_State ||--o{ Boss : "IS A"

    Player ||--o{ Character : "IS A"
    Character ||--o{ Inventory : "1..m"
    Inventory ||--o{ Gear : "m"
    Inventory ||--o{ Potion : "m"
    Gear ||--o{ Ability : "m"
    Ability ||--o{ Damage : "1"

    Character ||--o{ Stats : "2"
    Gear ||--o{ Stats : "m"
    Gear ||--o{ Slot : "1"
    Slot ||--o{ Gear : "m"

    gamestate_has_character }|--|| Game_State : "1"
    gamestate_has_character }|--|| Character : "m"

    character_has_inventory }|--|| Character : "m"
    character_has_inventory }|--|| Inventory : "2"

    inventory_stores_gear }|--|| Inventory : "m"
    inventory_stores_gear }|--|| Gear : "m"

    inventory_stores_potion }|--|| Inventory : "m"
    inventory_stores_potion }|--|| Potion : "m"

    gear_involves_ability }|--|| Gear : "m"
    gear_involves_ability }|--|| Ability : "1"

    ability_has_damage }|--|| Ability : "1"
    ability_has_damage }|--|| Damage : "1"

    gear_located_slot }|--|| Gear : "m"
    gear_located_slot }|--|| Slot : "1"

    gear_has_stats }|--|| Gear : "m"
    gear_has_stats }|--|| Stats : "1"

    character_has_stats }|--|| Character : "m"
    character_has_stats }|--|| Stats : "2"
```
