# DungeonCrawl

we will use then C2X standard with Meson as our build tool and for unit testing

indentation will be 4 spaces
regular C naming conventions


# Scrum people
Jil & Nino

# Manual 
Lucien

# Definition of Done
- code runs and does what it is supposed to do
- code is reviewed
- has positive and negative unit tests
- test coverage of at least 90%
- can build on all platforms
- fulfills clean code standards
- code is documented

# TestCoverage
The testcoverage can be checked <a href="https://raw.githack.com/PM4-FS25-DungeonCrawl/DungeonCrawl/refs/heads/develop/documents/coverage/coverage-report.html" target="_blank">here</a>.

# Project Documentation
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
## Localization Diagram
```mermaid
erDiagram
    Attribute ||--o{ AT_ID : has
    Attribute ||--o{ AT_NAME : has

    Localization ||--o{ LC_ID : has
    Localization ||--o{ LC_AT_1 : has
    Localization ||--o{ LC_AT_2 : has
    Localization ||--o{ LC_AT_3 : has
    Localization ||--o{ LC_AT_4 : has
    Localization ||--o{ LC_TEXT_DE : has
    Localization ||--o{ LC_TEXT_EN : has

    Attribute ||--o{ Localization : "ID"
```
