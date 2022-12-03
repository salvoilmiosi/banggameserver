#include "flint_westwood.h"

#include "game/game.h"

namespace banggame {

    void handler_flint_westwood::on_play(card *origin_card, player *origin, card *chosen_card, card *target_card) {
        auto target = target_card->owner;

        for (int i=2; i && !target->m_hand.empty(); --i) {
            card *stolen_card = target->random_hand_card();
            if (stolen_card->visibility != card_visibility::shown) {
                target->m_game->add_log(update_target::includes(origin, target), "LOG_STOLEN_CARD", origin, target, stolen_card);
                target->m_game->add_log(update_target::excludes(origin, target), "LOG_STOLEN_CARD_FROM_HAND", origin, target);
            } else {
                target->m_game->add_log("LOG_STOLEN_CARD", origin, target, stolen_card);
            }
            origin->steal_card(stolen_card);
        }
        if (chosen_card->visibility != card_visibility::shown) {
            target->m_game->add_log(update_target::includes(origin, target), "LOG_GIFTED_CARD", origin, target, chosen_card);
            target->m_game->add_log(update_target::excludes(origin, target), "LOG_GIFTED_A_CARD", origin, target);
        } else {
            target->m_game->add_log("LOG_GIFTED_CARD", origin, target, chosen_card);
        }
        target->steal_card(chosen_card);
    }
}