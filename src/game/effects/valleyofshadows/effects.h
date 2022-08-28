#ifndef __VALLEYOFSHADOWS_EFFECTS_H__
#define __VALLEYOFSHADOWS_EFFECTS_H__

#include "../card_effect.h"

namespace banggame {

    struct effect_aim {
        game_string verify(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin);
    };
    
    struct effect_backfire {
        game_string verify(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin);
    };

    struct effect_bandidos {
        void on_play(card *origin_card, player *origin, player *target, effect_flags flags);
    };

    struct effect_tornado {
        void on_play(card *origin_card, player *origin, player *target, effect_flags flags);
    };

    struct effect_poker {
        game_string on_prompt(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin);
    };

    struct effect_saved {
        bool can_respond(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin);
    };

    struct effect_escape {
        bool can_respond(card *origin_card, player *origin);
        void on_play(card *origin_card, player *origin);
    };

    struct effect_fanning : effect_empty {
        game_string verify(card *origin_card, player *origin);
    };

    struct handler_fanning {
        game_string verify(card *origin_card, player *origin, player *player1, player *player2);
        void on_play(card *origin_card, player *origin, player *player1, player *player2);
    };

    struct handler_play_as_gatling {
        void on_play(card *origin_card, player *origin, card *chosen_card);
    };
}

#endif