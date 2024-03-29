#ifndef __VALLEYOFSHADOWS_POKER_H__
#define __VALLEYOFSHADOWS_POKER_H__

#include "cards/card_effect.h"

namespace banggame {
    
    struct effect_poker {
        void on_play(card *origin_card, player *origin, player *target, effect_flags flags);
        void on_play(card *origin_card, player *origin);
    };

    DEFINE_EFFECT(poker, effect_poker)
}

#endif