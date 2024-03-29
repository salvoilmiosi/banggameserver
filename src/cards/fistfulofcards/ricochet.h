#ifndef __FISTFULOFCARDS_RICOCHET_H__
#define __FISTFULOFCARDS_RICOCHET_H__

#include "cards/card_effect.h"
#include "game/bot_suggestion.h"

namespace banggame {

    struct effect_ricochet : bot_suggestion::target_enemy_card {
        void on_play(card *origin_card, player *origin, card *target);
    };

    DEFINE_EFFECT(ricochet, effect_ricochet)
}

#endif