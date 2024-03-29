#include "beer.h"

#include "game/game.h"

namespace banggame {
    
    game_string effect_beer::on_prompt(card *origin_card, player *target) {
        if (target->m_game->num_alive() <= 2 || (target->m_hp == target->m_max_hp)) {
            return {"PROMPT_CARD_NO_EFFECT", origin_card};
        }
        return {};
    }

    void effect_beer::on_play(card *origin_card, player *target) {
        if (target->m_game->num_alive() > 2) {
            int amount = 1;
            target->m_game->call_event(event_type::apply_beer_modifier{ target, amount });
            target->heal(amount);
        }
        target->m_game->call_event(event_type::on_play_beer{ target });
    }

    bool effect_beer::can_play(card *origin_card, player *target) {
        return !target->m_game->pending_requests() || target->m_game->num_alive() > 2;
    }
}