#include "combat_mode.h"

void combat(Player *player, Monster *monster){

    combat_state current_state = MENU_COMBAT;

    while(player->health > 0 && monster->health > 0){
        switch (current_state)
        {
        case MENU_COMBAT:
            combat_menu(player, monster);
            break;
        case MENU_ABILITY:
            ability_menu(player, monster);
            break;
        case MENU_ITEM:
            item_menu(player, monster);
        default:
            break;
        }
    }
    if (player->health <= 0) {
        player_died();
    } else if (monster->health <= 0) {
        player_won();
    }
}

void combat_menu(Player *player, Monster *monster){
    /* TODO */
}

void ability_menu(Player *player, Monster *monster){
    /* TODO */
}

void item_menu(Player *player, Monster *monster){
    /* TODO */
}

void use_ability(Player *player, Monster *monster, Ability *ability){
    /* TODO */
}

bool roll_hit(Player *player, Ability *ability, int dice_size){
    /* TODO */
}

int roll_damage(Player *player, Ability *ability, int dice_size){
    /* TODO */
}

void deal_damage(int damage, Monster *monster){
    /* TODO */
}

void take_damage(Monster *monster, Player *player){
    /* TODO */
}

void use_item(){
    /* TODO */
}

void player_won(){
    /* TODO */
}

void player_died(){
    /* TODO */
}