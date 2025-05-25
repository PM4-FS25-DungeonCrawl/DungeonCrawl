#ifndef CHARACTER_H
#define CHARACTER_H

#include "../combat/ability_fw.h"
#include "../combat/damage.h"
#include "../common.h"
#include "../item/gear.h"
#include "../item/potion.h"
#include "../memory/memory_management.h"
#include "../stats/stats.h"

#define MAX_ABILITY_LIMIT 20
#define MAX_GEAR_LIMIT 20
#define MAX_POTION_LIMIT 20

typedef enum {
    PLAYER,
    MONSTER,
    BOSS
} character_type_t;

typedef struct character_t {
    character_type_t type;
    char name[MAX_NAME_LENGTH];

    stats_t base_stats;
    stats_t current_stats;

    resources_t max_resources;
    resources_t current_resources;

    damage_resistance_t resistance[DAMAGE_TYPE_COUNT];
    defenses_t defenses;

    ability_t* abilities[MAX_ABILITY_LIMIT];
    ability_t* base_attack;
    int ability_count;

    potion_t* potion_inventory[MAX_POTION_LIMIT];
    int potion_count;

    gear_t* gear_inventory[MAX_GEAR_LIMIT];
    int gear_count;

    gear_t* equipment[MAX_SLOT];

    int level;
    int xp;
    int xp_reward;
    int skill_points;
} character_t;

/**
 * @brief Initializes a new character
 * @param memory_pool Pointer to the memory pool for dynamic allocation
 * @param type The type of the character (e.g., player, enemy)
 * @param name The name of the character
 * @return Pointer to the initialized character, or NULL on failure
 */
character_t* init_character(memory_pool_t* memory_pool, character_type_t type, const char* name);

/**
 * @brief Frees the memory allocated for a character
 * @param memory_pool Pointer to the memory pool used for allocation
 * @param character Pointer to the character to be freed
 */
void free_character(memory_pool_t* memory_pool, character_t* character);

/**
 * @brief Sets the stats for a character
 * @param c Pointer to the character to set stats for
 * @param strength Strength value
 * @param intelligence Intelligence value
 * @param dexterity Dexterity value
 * @param constitution Constitution value
 */
void set_character_stats(character_t* character, int strength, int intelligence, int dexterity, int constitution);

/**
 * @brief Sets the stats for a character
 * @param stats Pointer to the stats structure to set
 * @param strength Strength value
 * @param intelligence Intelligence value
 * @param dexterity Dexterity value
 * @param constitution Constitution value
 */
void set_stats(stats_t* stats, int strength, int intelligence, int dexterity, int constitution);

/**
 * @brief Updates the character's resources based on their stats
 * @param current_resources Pointer to the current resources structure to update
 * @param max_resources Pointer to the max resources structure to update
 * @param base_stats Pointer to the base stats structure
 */
void update_character_resources(resources_t* current_resources, resources_t* max_resources, stats_t* base_stats);

/**
 * @brief Sets the damage modifier for a character
 * @param character Pointer to the character to set the damage modifier for
 * @param type The type of damage to modify
 * @param value The value of the damage modifier
 */
void set_character_dmg_modifier(character_t* character, damage_type_t type, int value);

/**
 * @brief Adds an ability to a character
 * @param character Pointer to the character
 * @param ability Pointer to the ability to add
 */
void add_ability(character_t* character, ability_t* ability);

/**
 * @brief Removes an ability from a character
 * @param character Pointer to the character
 * @param ability Pointer to the ability to remove
 */
void remove_ability(character_t* character, const ability_t* ability);

/**
 * @brief Adds a potion to a character's inventory
 * @param character Pointer to the character
 * @param potion Pointer to the potion to add
 */
void add_potion(character_t* character, potion_t* potion);

/**
 * @brief Removes a potion from a character's inventory
 * @param character Pointer to the character
 * @param potion Pointer to the potion to remove
 */
void remove_potion(character_t* character, potion_t* potion);

/**
 * @brief Adds gear to a character's inventory
 * @param character Pointer to the character
 * @param gear Pointer to the gear to add
 */
void add_gear(character_t* character, gear_t* gear);

/**
 * @brief Removes a gear item from a character's inventory
 * @param character Pointer to the character
 * @param gear Pointer to the gear item to remove
 */
void remove_gear(character_t* character, gear_t* gear);

/**
 * @brief Removes the gear equipped in a specific slot of a character.
 *
 * @param character Pointer to the character whose gear is to be removed.
 * @param slot The slot from which the gear should be removed.
 */
void remove_equipped_gear(character_t* character, gear_slot_t slot);

bool add_equipped_gear(character_t* character, gear_t* gear);

/**
 * @brief Equips a gear item to a character
 * @param character Pointer to the character
 * @param gear Pointer to the gear item to equip
 */
void equip_gear(character_t* character, gear_t* gear);

/**
 * @brief Unequips a gear item from a character and adds it to the character's inventory.
 * @param character Pointer to the character
 * @param slot The slot to unequip from
 */
void unequip_gear(character_t* character, gear_slot_t slot);

/**
 * @brief Resets the current stats of a character to their base values
 * @param character Pointer to the character
 */
void reset_current_stats(character_t* character);

/**
 * @brief Sets the level for a character
 * @param character Pointer to the character
 * @param level The level value
 */
void set_level(character_t* character, int level);

/**
 * @brief Sets the XP reward for a character
 * @param character Pointer to the character
 * @param xp_reward The XP reward value
 */
void set_xp_reward(character_t* character, int xp_reward);

/**
 * @brief sets initial xp for a character
 * @param character Pointer to the character
 * @param xp The initial xp value
 */
void set_initial_xp(character_t* character, int xp);

/**
 * @brief Sets the skill points for a character
 * @param character Pointer to the character
 * @param skill_points How many skill points the character has
 */
void set_skill_points(character_t* character, int skill_points);

/**
 * @brief Resets the player stats to their base values
 * @param player Pointer to the player character
 */
void reset_player_stats(character_t* player);

#endif//CHARACTER_H
