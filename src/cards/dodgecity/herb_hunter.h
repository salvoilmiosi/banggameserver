#ifndef __DODGECITY_HERB_HUNTER_H__
#define __DODGECITY_HERB_HUNTER_H__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_herb_hunter : event_equip {
        void on_enable(card *target_card, player *target);
    };
}

#endif