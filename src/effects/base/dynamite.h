#ifndef __BASE_DYNAMITE_H__
#define __BASE_DYNAMITE_H__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_dynamite : event_equip {
        void on_enable(card_ptr target_card, player_ptr target);
    };

    DEFINE_EQUIP(dynamite, equip_dynamite)
}

#endif