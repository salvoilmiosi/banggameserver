#include "requests.h"
#include "effects.h"

#include "../base/effects.h"

#include "../../game.h"

namespace banggame {

    void timer_damaging::on_finished() {
        target->damage(origin_card, origin, damage, is_bang, true);
    }

    game_formatted_string timer_damaging::status_text(player *owner) const {
        return {damage > 1 ? "STATUS_DAMAGING_PLURAL" : "STATUS_DAMAGING", target, origin_card, damage};
    }

    void request_destroy::on_finished() {
        effect_destroy::resolver{origin_card, origin, target_card}.resolve();
    }

    game_formatted_string request_destroy::status_text(player *owner) const {
        if (target == owner) {
            if (target_card->pocket == pocket_type::player_hand) {
                return {"STATUS_DESTROY_FROM_HAND", origin_card};
            } else {
                return {"STATUS_DESTROY", origin_card, target_card};
            }
        } else {
            if (target_card->pocket == pocket_type::player_hand) {
                return {"STATUS_DESTROY_OTHER_FROM_HAND", target, origin_card};
            } else {
                return {"STATUS_DESTROY_OTHER", target, origin_card, target_card};
            }
        }
    }

    void request_steal::on_finished() {
        effect_steal::resolver{origin_card, origin, target_card}.resolve();
    }

    game_formatted_string request_steal::status_text(player *owner) const {
        if (target == owner) {
            if (target_card->pocket == pocket_type::player_hand) {
                return {"STATUS_STEAL_FROM_HAND", origin_card};
            } else {
                return {"STATUS_STEAL", origin_card, target_card};
            }
        } else {
            if (target_card->pocket == pocket_type::player_hand) {
                return {"STATUS_STEAL_OTHER_FROM_HAND", target, origin_card};
            } else {
                return {"STATUS_STEAL_OTHER", target, origin_card, target_card};
            }
        }
    }

    game_formatted_string request_card_as_gatling::status_text(player *owner) const {
        if (target == owner) {
            return {"STATUS_CARD_AS_GATLING", origin_card};
        } else {
            return {"STATUS_CARD_AS_GATLING_OTHER", target, origin_card};
        }
    }

    bool request_bandidos::can_pick(pocket_type pocket, player *target_player, card *target_card) const {
        return pocket == pocket_type::player_hand && target_player == target;
    }

    void request_bandidos::on_pick(pocket_type pocket, player *target_player, card *target_card) {
        target->m_game->add_log("LOG_DISCARDED_CARD_FOR", origin_card, target, target_card);
        target->discard_card(target_card);
        if (--num_cards == 0 || target->m_hand.empty()) {
            target->m_game->pop_request();
        } else {
            using namespace enums::flag_operators;
            flags &= ~effect_flags::escapable;
        }
        target->m_game->update_request();
    }

    void request_bandidos::on_resolve() {
        target->m_game->pop_request();
        target->damage(origin_card, origin, 1);
        target->m_game->update_request();
    }

    game_formatted_string request_bandidos::status_text(player *owner) const {
        if (target == owner) {
            return {"STATUS_BANDIDOS", origin_card};
        } else {
            return {"STATUS_BANDIDOS_OTHER", target, origin_card};
        }
    }

    bool request_tornado::can_pick(pocket_type pocket, player *target_player, card *target_card) const {
        return pocket == pocket_type::player_hand && target_player == target;
    }

    void request_tornado::on_pick(pocket_type pocket, player *target_player, card *target_card) {
        target->m_game->pop_request();
        target->m_game->add_log("LOG_DISCARDED_CARD_FOR", origin_card, target, target_card);
        target->discard_card(target_card);
        target->draw_card(2, origin_card);
        target->m_game->update_request();
    }

    game_formatted_string request_tornado::status_text(player *owner) const {
        if (target == owner) {
            return {"STATUS_TORNADO", origin_card};
        } else {
            return {"STATUS_TORNADO_OTHER", target, origin_card};
        }
    }

    bool request_poker::can_pick(pocket_type pocket, player *target_player, card *target_card) const {
        return pocket == pocket_type::player_hand && target_player == target;
    }

    void request_poker::on_pick(pocket_type pocket, player *target_player, card *target_card) {
        target->m_game->pop_request();
        target->m_game->add_log("LOG_DISCARDED_A_CARD_FOR", origin_card, target);
        target->m_game->move_card(target_card, pocket_type::selection, origin);
        target->m_game->update_request();
    }

    game_formatted_string request_poker::status_text(player *owner) const {
        if (target == owner) {
            return {"STATUS_POKER", origin_card};
        } else {
            return {"STATUS_POKER_OTHER", target, origin_card};
        }
    }

    void request_poker_draw::on_pick(pocket_type pocket, player *target_player, card *target_card) {
        target->m_game->add_log("LOG_DRAWN_CARD", target, target_card);
        target->add_to_hand(target_card);
        if (--num_cards == 0 || target->m_game->m_selection.size() == 0) {
            target->m_game->pop_request();
            for (auto *c : target->m_game->m_selection) {
                target->m_game->move_card(c, pocket_type::discard_pile);
            }
        }
        target->m_game->update_request();
    }

    game_formatted_string request_poker_draw::status_text(player *owner) const {
        if (target == owner) {
            return {"STATUS_POKER_DRAW", origin_card};
        } else {
            return {"STATUS_POKER_DRAW_OTHER", target, origin_card};
        }
    }

    bool request_saved::can_pick(pocket_type pocket, player *target_player, card *target_card) const {
        return pocket == pocket_type::main_deck
            || (pocket == pocket_type::player_hand && target_player == saved);
    }

    void request_saved::on_pick(pocket_type pocket, player *target_player, card *target_card) {
        target->m_game->pop_request();
        if (pocket == pocket_type::player_hand) {
            for (int i=0; i<2 && !saved->m_hand.empty(); ++i) {
                card *stolen_card = saved->random_hand_card();
                target->m_game->add_log(update_target::includes(target, saved), "LOG_STOLEN_CARD", target, saved, stolen_card);
                target->m_game->add_log(update_target::excludes(target, saved), "LOG_STOLEN_CARD_FROM_HAND", target, saved);
                target->steal_card(stolen_card);
            }
        } else {
            target->draw_card(2, origin_card);
        }
        target->m_game->update_request();
    }

    game_formatted_string request_saved::status_text(player *owner) const {
        if (target == owner) {
            return {"STATUS_SAVED", origin_card, saved};
        } else {
            return {"STATUS_SAVED_OTHER", target, origin_card, saved};
        }
    }

    game_formatted_string timer_lemonade_jim::status_text(player *owner) const {
        if (target == owner) {
            return {"STATUS_CAN_PLAY_CARD", origin_card};
        } else {
            return {"STATUS_CAN_PLAY_CARD_OTHER", target, origin_card};
        }
    }
}