#ifndef __BASE_LUCKY_DUKE_H__
#define __BASE_LUCKY_DUKE_H__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_lucky_duke : event_equip {
        void on_enable(card_ptr target_card, player_ptr target);
    };

    DEFINE_EQUIP(lucky_duke, equip_lucky_duke)

}

#endif