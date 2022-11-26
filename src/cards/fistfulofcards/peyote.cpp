#include "peyote.h"

#include "game/game.h"

namespace banggame {

    struct request_peyote : selection_picker {
        request_peyote(card *origin_card, player *target)
            : selection_picker(origin_card, nullptr, target) {}

        void on_pick(card *target_card) override {
            target->m_game->flash_card(target_card);
            
            auto *drawn_card = target->m_game->top_of_deck();
            target->m_game->send_card_update(drawn_card, nullptr, show_card_flags::short_pause);

            short choice = *target_card->get_tag_value(tag_type::peyote);

            if (choice == 1) {
                target->m_game->add_log("LOG_DECLARED_RED", target, origin_card);
            } else {
                target->m_game->add_log("LOG_DECLARED_BLACK", target, origin_card);
            }

            if (choice == 1
                ? (drawn_card->sign.suit == card_suit::hearts || drawn_card->sign.suit == card_suit::diamonds)
                : (drawn_card->sign.suit == card_suit::clubs || drawn_card->sign.suit == card_suit::spades))
            {
                target->draw_card();
            } else {
                auto lock = target->m_game->lock_updates(true);
                target->m_game->add_log("LOG_DISCARDED_SELF_CARD", target, drawn_card);
                target->m_game->move_card(drawn_card, pocket_type::discard_pile);

                while (!target->m_game->m_selection.empty()) {
                    target->m_game->move_card(target->m_game->m_selection.front(), pocket_type::hidden_deck, nullptr, show_card_flags::instant);
                }
                target->m_game->call_event<event_type::post_draw_cards>(target);
            }
        }

        game_string status_text(player *owner) const override {
            if (target == owner) {
                return {"STATUS_PEYOTE", origin_card};
            } else {
                return {"STATUS_PEYOTE_OTHER", target, origin_card};
            }
        }
    };
    
    void equip_peyote::on_enable(card *target_card, player *target) {
        target->m_game->add_listener<event_type::phase_one_override>(target_card, [=](player *p) {
            std::vector<card *> target_cards;
            for (card *c : p->m_game->m_hidden_deck) {
                if (c->has_tag(tag_type::peyote)) {
                    target_cards.push_back(c);
                }
            }
            for (card *c : target_cards) {
                p->m_game->move_card(c, pocket_type::selection, nullptr, show_card_flags::instant);
            }
            
            p->m_game->queue_request<request_peyote>(target_card, p);
        });

        target->m_game->set_game_flags(game_flags::phase_one_override);
    }
}