#ifndef __BASE_SLAB_THE_KILLER_H__
#define __BASE_SLAB_THE_KILLER_H__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_slab_the_killer : event_equip {
        void on_enable(card_ptr target_card, player_ptr target);
    };

    DEFINE_EQUIP(slab_the_killer, equip_slab_the_killer)
}

#endif