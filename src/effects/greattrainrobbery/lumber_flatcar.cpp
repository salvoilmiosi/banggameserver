#include "lumber_flatcar.h"

#include "game/game.h"

namespace banggame {

    void equip_lumber_flatcar::on_enable(card_ptr target_card, player_ptr target) {
        target->m_game->add_listener<event_type::count_range_mod>({target_card, 0}, [=](const_player_ptr origin, range_mod_type type, int &value) {
            if (origin == target && type == range_mod_type::range_mod) {
                --value;
            }
        });
    }

    void equip_lumber_flatcar::on_disable(card_ptr target_card, player_ptr target) {
        target->m_game->remove_listeners(event_card_key{target_card, 0});
    }
}