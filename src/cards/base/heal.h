#ifndef __EFFECT_HEAL_H__
#define __EFFECT_HEAL_H__

#include "cards/card_effect.h"

namespace banggame {

    struct effect_heal {
        int amount;
        effect_heal(int value) : amount(std::max(1, value)) {}

        game_string on_prompt(card *origin_card, player *origin) {
            return on_prompt(origin_card, origin, origin);
        }
        game_string on_prompt(card *origin_card, player *origin, player *target);

        void on_play(card *origin_card, player *origin) {
            on_play(origin_card, origin, origin);
        }
        void on_play(card *origin_card, player *origin, player *target);
    };

    struct effect_heal_notfull : effect_heal {
        game_string verify(card *origin_card, player *origin) {
            return verify(origin_card, origin, origin);
        }
        game_string verify(card *origin_card, player *origin, player *target);
    };
    
    struct handler_heal_multi {
        game_string on_prompt(card *origin_card, player *origin, int amount);
        void on_play(card *origin_card, player *origin, int amount);
    };
}

#endif