#ifndef __ARMEDANDDANGEROUS_BUNTLINESPECIAL_H__
#define __ARMEDANDDANGEROUS_BUNTLINESPECIAL_H__

#include "cards/card_effect.h"

namespace banggame {

    struct effect_buntlinespecial {
        void on_play(card *origin_card, player *origin);
    };

    DEFINE_EFFECT(buntlinespecial, effect_buntlinespecial)
}

#endif