#include "weapon.h"

#include "game/game.h"
#include "game/bot_suggestion.h"
#include "prompts.h"
#include "cards/filter_enums.h"

namespace banggame {

    struct is_weapon {
        const_card_ptr target_card;
        bool operator ()(const_card_ptr c) const {
            return c != target_card && c->has_tag(tag_type::weapon);
        }
    };

    game_string equip_weapon::on_prompt(card_ptr origin_card, player_ptr origin, player_ptr target) {
        if (range == 0) {
            if (origin->is_bot() && !bot_suggestion::target_enemy{}.on_check_target(origin_card, origin, target)) {
                return "BOT_BAD_PLAY";
            }
            return prompt_target_self{}.on_prompt(origin_card, origin, target);
        } else if (!origin->is_bot() && target == origin && origin->get_weapon_range() != 0) {
            if (auto it = rn::find_if(target->m_table, is_weapon{origin_card}); it != target->m_table.end()) {
                return {"PROMPT_REPLACE", origin_card, *it};
            }
        }
        return {};
    }

    void equip_weapon::on_enable(card_ptr target_card, player_ptr target) {
        if (auto it = rn::find_if(target->m_table, is_weapon{target_card}); it != target->m_table.end()) {
            target->m_game->add_log("LOG_DISCARDED_SELF_CARD", target, *it);
            target->discard_card(*it);
        }

        target->m_game->add_listener<event_type::count_range_mod>(target_card, [=, range=range](const_player_ptr origin, range_mod_type type, int &value) {
            if (origin == target && type == range_mod_type::weapon_range && !origin->m_game->is_disabled(target_card)) {
                value = range;
            }
        });
    }
}