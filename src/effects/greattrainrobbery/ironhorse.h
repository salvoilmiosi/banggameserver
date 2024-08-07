#ifndef __GREATTRAINROBBERY_IRONHORSE_H__
#define __GREATTRAINROBBERY_IRONHORSE_H__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_ironhorse : event_equip {
        void on_enable(card_ptr origin_card, player_ptr origin);
    };

    DEFINE_EQUIP(ironhorse, equip_ironhorse)
}

#endif