#ifndef __BOT_SUGGESTION_H__
#define __BOT_SUGGESTION_H__

#include "cards/card_fwd.h"

namespace banggame::bot_suggestion {

    struct target_enemy {
        bool on_check_target(card *origin_card, player *origin, player *target);
        bool on_check_target(card *origin_card, player *origin, card *target);
    };

    struct target_friend {
        bool on_check_target(card *origin_card, player *origin, player *target);
        bool on_check_target(card *origin_card, player *origin, card *target);
    };

    struct target_enemy_card {
        bool on_check_target(card *origin_card, player *origin, card *target);
    };
    
}

#endif