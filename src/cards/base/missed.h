#ifndef __BASE_MISSED_H__
#define __BASE_MISSED_H__

#include "cards/card_effect.h"

namespace banggame {

    struct effect_missed {
        bool can_play(card *origin_card, player *origin);
        game_string on_prompt(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin);
    };

    DEFINE_EFFECT(missed, effect_missed)

    struct effect_missedcard : effect_missed {
        void on_play(card *origin_card, player *origin);
    };

    DEFINE_EFFECT(missedcard, effect_missedcard)

    struct handler_play_as_missed {
        bool can_play(card *origin_card, player *origin, card *target_card);
        game_string on_prompt(card *origin_card, player *origin, card *target_card);
        void on_play(card *origin_card, player *origin, card *target_card);
    };

    DEFINE_MTH(play_as_missed, handler_play_as_missed)
}

#endif