#ifndef __BASE_WEAPON_H__
#define __BASE_WEAPON_H__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_weapon : event_equip {
        struct nodisable {};
        
        int range;
        equip_weapon(int range): range(range) {}

        game_string on_prompt(card *origin_card, player *origin, player *target);
        void on_enable(card *target_card, player *target);
    };

    DEFINE_EQUIP(weapon, equip_weapon)
}

#endif