#ifndef __BASE_SLAB_THE_KILLER_H__
#define __BASE_SLAB_THE_KILLER_H__

#include "cards/card_effect.h"

namespace banggame {

    struct effect_slab_the_killer : event_based_effect {
        void on_enable(card *target_card, player *target);
    };
}

#endif