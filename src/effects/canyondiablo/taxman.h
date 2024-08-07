#ifndef __CANYONDIABLO_TAXMAN_H__
#define __CANYONDIABLO_TAXMAN_H__

#include "cards/card_effect.h"
#include "game/bot_suggestion.h"
#include "effects/base/prompts.h"

namespace banggame {

    struct equip_taxman : event_equip, prompt_target_self, bot_suggestion::target_enemy {
        int predraw_check_priority;
        equip_taxman(int predraw_check_priority) : predraw_check_priority{predraw_check_priority} {}
        
        void on_enable(card_ptr target_card, player_ptr target);
    };

    DEFINE_EQUIP(taxman, equip_taxman)
}

#endif