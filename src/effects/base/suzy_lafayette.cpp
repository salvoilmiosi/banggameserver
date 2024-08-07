#include "suzy_lafayette.h"

#include "game/game.h"

namespace banggame {

    static void check_empty_hand(card_ptr origin_card, player_ptr origin) {
        origin->m_game->queue_action([=]{
            if (origin->alive() && origin->empty_hand()) {
                origin_card->flash_card();
                origin->draw_card(1, origin_card);
            }
        });
    }

    void equip_suzy_lafayette::on_enable(card_ptr origin_card, player_ptr origin) {
        if (origin->m_game->m_playing) {
            check_empty_hand(origin_card, origin);
        }

        origin->m_game->add_listener<event_type::on_discard_hand_card>(origin_card, [=](player_ptr target, card_ptr target_card, bool used) {
            if (origin == target) {
                check_empty_hand(origin_card, origin);
            }
        });
    }
}