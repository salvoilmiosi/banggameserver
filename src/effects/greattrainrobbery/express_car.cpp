#include "express_car.h"

#include "game/game.h"

#include "effects/base/requests.h"
#include "effects/base/max_usages.h"

#include "game/possible_to_play.h"

namespace banggame {

    game_string effect_express_car::get_error(card_ptr origin_card, player_ptr origin) {
        int usages = 0;
        origin->m_game->call_event(event_type::count_usages{ origin, origin_card, usages });
        if (usages >= 1) {
            return {"ERROR_MAX_USAGES", origin_card, 1};
        }
        return {};
    }

    game_string effect_express_car::on_prompt(card_ptr origin_card, player_ptr origin) {
        if (origin->is_bot()) {
            if (rn::any_of(get_all_playable_cards(origin), [](card_ptr c) { return c->pocket == pocket_type::player_hand; })) {
                return "BOT_BAD_PLAY";
            }
        } else if (int ncards = int(origin->m_hand.size())) {
            return {"PROMPT_PASS_DISCARD", ncards};
        }
        return {};
    }

    void effect_express_car::on_play(card_ptr origin_card, player_ptr origin) {
        event_card_key key{origin_card, 5};
        origin->m_game->add_listener<event_type::count_usages>(key, [=](player_ptr e_origin, card_ptr e_card, int &usages) {
            if (origin_card == e_card && origin == e_origin) {
                ++usages;
            }
        });
        origin->m_game->add_listener<event_type::pre_turn_start>(key, [=](player_ptr p) {
            if (p != origin) {
                origin->m_game->remove_listeners(key);
            }
        });

        ++origin->m_extra_turns;
        origin->m_game->queue_request<request_discard_hand>(origin_card, origin);
        origin->m_game->queue_action([=]{
            origin->pass_turn();
        });
    }
}