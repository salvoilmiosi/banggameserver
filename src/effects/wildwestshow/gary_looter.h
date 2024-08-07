#ifndef __WILDWESTSHOW_GARY_LOOTER__
#define __WILDWESTSHOW_GARY_LOOTER__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_gary_looter : event_equip {
        void on_enable(card_ptr target_card, player_ptr target);
    };

    DEFINE_EQUIP(gary_looter, equip_gary_looter)
}

#endif