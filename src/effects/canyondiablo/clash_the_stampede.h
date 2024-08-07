#ifndef __CANYONDIABLO_CLASH_THE_STAMPEDE__
#define __CANYONDIABLO_CLASH_THE_STAMPEDE__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_clash_the_stampede : event_equip {
        void on_enable(card_ptr target_card, player_ptr target);
    };

    DEFINE_EQUIP(clash_the_stampede, equip_clash_the_stampede)
}

#endif