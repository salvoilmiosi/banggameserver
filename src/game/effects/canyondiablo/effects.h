#ifndef __CANYONDIABLO_EFFECTS_H__
#define __CANYONDIABLO_EFFECTS_H__

#include "../card_effect.h"

namespace banggame {

    struct effect_graverobber {
        void on_play(card *origin_card, player *origin);
    };

    struct effect_mirage {
        opt_error verify(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin);
    };

    struct effect_disarm {
        opt_error verify(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin);
    };

    struct handler_card_sharper {
        opt_error verify(card *origin_card, player *origin, card *chosen_card, card *target_card);
        void on_play(card *origin_card, player *origin, card *chosen_card, card *target_card);
        void on_resolve(card *origin_card, player *origin, card *chosen_card, card *target_card);
    };

    struct effect_sacrifice {
        bool can_respond(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin);
    };

    struct effect_lastwill : event_based_effect {
        void on_enable(card *origin_card, player *origin);
        
        bool can_respond(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin) {}
    };

    struct handler_lastwill {
        void on_play(card *origin_card, player *origin, const target_list &targets);
    };

}

#endif