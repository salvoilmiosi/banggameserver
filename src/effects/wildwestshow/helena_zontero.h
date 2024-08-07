#ifndef __WILDWESTSHOW_HELENA_ZONTERO_H___
#define __WILDWESTSHOW_HELENA_ZONTERO_H___

#include "cards/card_effect.h"

namespace banggame {

    struct equip_helena_zontero {
        void on_enable(card_ptr target_card, player_ptr target);
    };

    DEFINE_EQUIP(helena_zontero, equip_helena_zontero)
}

#endif