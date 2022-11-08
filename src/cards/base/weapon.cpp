#include "weapon.h"

#include "game/game.h"

namespace banggame {

    static bool is_weapon(card *c) {
        return c->has_tag(tag_type::weapon);
    }

    game_string effect_weapon::on_prompt(player *origin, card *target_card, player *target) {
        if (target == origin) {
            if (auto it = std::ranges::find_if(target->m_table, is_weapon); it != target->m_table.end()) {
                return {"PROMPT_REPLACE", target_card, *it};
            }
        }
        return {};
    }

    void effect_weapon_base::on_equip(card *target_card, player *target) {
        if (auto it = std::ranges::find_if(target->m_table, is_weapon); it != target->m_table.end()) {
            target->m_game->add_log("LOG_DISCARDED_SELF_CARD", target, *it);
            target->discard_card(*it);
        }
    }

    void effect_weapon_base::on_enable(card *target_card, player *target) {
        target->m_weapon_range = range;
        target->send_player_status();
    }

    void effect_weapon_base::on_disable(card *target_card, player *target) {
        target->m_weapon_range = 1;
        target->send_player_status();
    }
}