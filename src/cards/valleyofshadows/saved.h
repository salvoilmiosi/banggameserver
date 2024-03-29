#ifndef __VALLEYOFSHADOWS_SAVED_H__
#define __VALLEYOFSHADOWS_SAVED_H__

#include "cards/card_effect.h"

namespace banggame {

    struct effect_saved {
        bool can_play(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin);
    };

    DEFINE_EFFECT(saved, effect_saved)
}

#endif