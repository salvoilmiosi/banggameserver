#ifndef __WILDWESTSHOW_FLINT_WESTWOOD__
#define __WILDWESTSHOW_FLINT_WESTWOOD__

#include "cards/card_effect.h"
#include "game/bot_suggestion.h"

namespace banggame {

    struct handler_flint_westwood {
        bool on_check_target(card *origin_card, player *origin, card *chosen_card, card *target_card) {
            return bot_suggestion::target_enemy{}.on_check_target(origin_card, origin, target_card);
        }
        void on_play(card *origin_card, player *origin, card *chosen_card, card *target_card);
    };

    DEFINE_MTH(flint_westwood, handler_flint_westwood)
}

#endif