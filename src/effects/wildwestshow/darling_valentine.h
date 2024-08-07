#ifndef __WILDWESTSHOW_DARLING_VALENTINE_H___
#define __WILDWESTSHOW_DARLING_VALENTINE_H___

#include "cards/card_effect.h"

namespace banggame {

    struct equip_darling_valentine : event_equip {
        void on_enable(card_ptr target_card, player_ptr target);
    };

    DEFINE_EQUIP(darling_valentine, equip_darling_valentine)
}

#endif