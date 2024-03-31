#include "ricochet.h"

#include "game/game.h"
#include "effects/base/bang.h"
#include "effects/base/steal_destroy.h"

namespace banggame {
    
    struct request_ricochet : request_targeting, missable_request {
        using request_targeting::request_targeting;

        request_timer *timer() override { return nullptr; }

        void on_update() override {
            if (target->empty_hand()) {
                auto_resolve();
            }
        }

        void on_resolve_target() override {
            effect_destroy{}.on_resolve(origin_card, origin, target_card);
        }

        void on_miss(card *c, effect_flags missed_flags = {}) override {
            target->m_game->pop_request();
        }

        game_string status_text(player *owner) const override {
            if (target == owner) {
                return {"STATUS_RICOCHET", origin_card, target_card};
            } else {
                return {"STATUS_RICOCHET_OTHER", target, origin_card, target_card};
            }
        }
    };

    void effect_ricochet::on_play(card *origin_card, player *origin, card *target_card) {
        origin->m_game->queue_request<request_ricochet>(origin_card, origin, target_card->owner, target_card);
    }
}