#ifndef __ARMEDANDDANGEROUS_EQUIPS_H__
#define __ARMEDANDDANGEROUS_EQUIPS_H__

#include "../card_effect.h"

namespace banggame {
    
    struct effect_bomb {
        void on_equip(card *target_card, player *target);
        void on_unequip(card *target_card, player *target);
    };

    struct effect_tumbleweed : event_based_effect {
        void on_enable(card *target_card, player *target);

        bool can_respond(card *origin_card, player *origin);
        void on_play(card *origin_card, player *target);
    };
}

#endif