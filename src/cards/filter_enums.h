#ifndef __CARDS_FILTER_ENUMS_H__
#define __CARDS_FILTER_ENUMS_H__

#include "utils/enums.h"

namespace banggame {

    DEFINE_ENUM_FLAGS(target_player_filter,
        (any)
        (dead)
        (self)
        (notself)
        (notsheriff)
        (range_1)
        (range_2)
        (reachable)
        (not_empty_hand)
        (not_empty_cubes)
    )

    DEFINE_ENUM_FLAGS(target_card_filter,
        (table)
        (hand)
        (blue)
        (black)
        (train)
        (blue_or_train)
        (hearts)
        (diamonds)
        (clubs)
        (spades)
        (two_to_nine)
        (ten_to_ace)
        (bang)
        (bangcard)
        (missed)
        (missedcard)
        (beer)
        (bronco)
        (cube_slot)
        (can_repeat)
        (can_target_self)
        (catbalou_panic)
    )
    
    DEFINE_ENUM(tag_type,
        (none)
        (temp_card)
        (ghost_card)
        (confirm)
        (skip_logs)
        (no_auto_discard)
        (hidden)
        (discard_if_two_players)
        (bangcard)
        (missed)
        (missedcard)
        (play_as_bang)
        (beer)
        (indians)
        (panic)
        (cat_balou)
        (drawing)
        (weapon)
        (horse)
        (repeatable)
        (auto_confirm)
        (auto_confirm_red_ringo)
        (card_choice)
        (last_scenario_card)
        (peyote)
        (handcuffs)
        (buy_cost)
        (max_hp)
        (bronco)
    )
}

#endif