#include "curse.h"

#include "game/game.h"

namespace banggame {

    void effect_curse::on_enable(card *target_card, player *target) {
        target->m_game->add_listener<event_type::apply_sign_modifier>(target_card, [](player *, card_sign &value) {
            value.suit = card_suit::spades;
        });
    }
}