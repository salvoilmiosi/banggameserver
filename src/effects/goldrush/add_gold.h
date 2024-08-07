#ifndef __GOLDRUSH_ADD_GOLD_H__
#define __GOLDRUSH_ADD_GOLD_H__

#include "cards/card_effect.h"

namespace banggame {

    struct effect_add_gold {
        int amount;
        effect_add_gold(int value) : amount(std::max(1, value)) {}

        void on_play(card_ptr origin_card, player_ptr origin, player_ptr target);
    };

    DEFINE_EFFECT(add_gold, effect_add_gold)
}

#endif