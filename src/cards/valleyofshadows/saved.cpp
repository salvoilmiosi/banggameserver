#include "saved.h"

#include "game/game.h"
#include "cards/base/damage.h"

namespace banggame {
    
    struct request_saved : request_base {
        request_saved(card *origin_card, player *target, player *saved)
            : request_base(origin_card, nullptr, target, effect_flags::auto_pick)
            , saved(saved) {}

        player *saved = nullptr;

        bool can_pick(card *target_card) const override {
            return target_card->pocket == pocket_type::main_deck
                || target_card->pocket == pocket_type::player_hand && target_card->owner == saved;
        }

        void on_pick(card *target_card) override {
            target->m_game->pop_request();
            if (target_card->pocket == pocket_type::main_deck) {
                target->draw_card(2, origin_card);
            } else {
                for (int i=0; i<2 && !saved->m_hand.empty(); ++i) {
                    card *stolen_card = saved->random_hand_card();
                    target->m_game->add_log(update_target::includes(target, saved), "LOG_STOLEN_CARD", target, saved, stolen_card);
                    target->m_game->add_log(update_target::excludes(target, saved), "LOG_STOLEN_CARD_FROM_HAND", target, saved);
                    target->steal_card(stolen_card);
                }
            }
            target->m_game->update_request();
        }

        game_string status_text(player *owner) const override {
            if (target == owner) {
                return {"STATUS_SAVED", origin_card, saved};
            } else {
                return {"STATUS_SAVED_OTHER", target, origin_card, saved};
            }
        }
    };

    bool effect_saved::can_respond(card *origin_card, player *origin) {
        if (auto *req = origin->m_game->top_request_if<timer_damaging>()) {
            return req->target != origin;
        }
        return false;
    }

    void effect_saved::on_play(card *origin_card, player *origin) {
        auto &req = origin->m_game->top_request().get<timer_damaging>();
        player *saved = req.target;
        origin->m_game->queue_action_front([=]{
            if (saved->alive()) {
                origin->m_game->queue_request<request_saved>(origin_card, origin, saved);
            }
        });
        if (0 == --req.damage) {
            origin->m_game->pop_request();
        }
        origin->m_game->update_request();
    }
}