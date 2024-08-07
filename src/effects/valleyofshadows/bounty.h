#ifndef __VALLEYOFSHADOWS_BOUNTY_H__
#define __VALLEYOFSHADOWS_BOUNTY_H__

#include "cards/card_effect.h"
#include "game/bot_suggestion.h"
#include "effects/base/prompts.h"

namespace banggame {

    struct equip_bounty : event_equip, prompt_target_self, bot_suggestion::target_enemy {
        void on_enable(card_ptr target_card, player_ptr target);
    };

    DEFINE_EQUIP(bounty, equip_bounty)
}

#endif