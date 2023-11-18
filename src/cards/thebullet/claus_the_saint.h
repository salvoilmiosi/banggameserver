#ifndef __THEBULLET_CLAUS_THE_SAINT_H__
#define __THEBULLET_CLAUS_THE_SAINT_H__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_claus_the_saint : event_equip {
        void on_enable(card *target_card, player *target);
    };

    struct handler_claus_the_saint {
        game_string get_error(card *origin_card, player *origin, card *target_card, player *target_player);
        void on_play(card *origin_card, player *origin, card *target_card, player *target_player);
    };
}

#endif