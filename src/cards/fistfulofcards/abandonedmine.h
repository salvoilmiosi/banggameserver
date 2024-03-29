#ifndef __FISTFULOFCARDS_ABANDONEDMINE_H__
#define __FISTFULOFCARDS_ABANDONEDMINE_H__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_abandonedmine {
        void on_enable(card *target_card, player *target);
        void on_disable(card *target_card, player *target);
    };

    DEFINE_EQUIP(abandonedmine, equip_abandonedmine)
}

#endif