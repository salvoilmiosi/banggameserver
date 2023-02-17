#ifndef __BASE_DAMAGE_H__
#define __BASE_DAMAGE_H__

#include "cards/card_effect.h"

namespace banggame {
    
    struct effect_damage {
        int damage;
        effect_damage(int damage) : damage(std::max(1, damage)) {}

        game_string get_error(card *origin_card, player *origin, effect_flags flags = {});
        
        void on_play(card *origin_card, player *origin, effect_flags flags = {}) {
            on_play(origin_card, origin, origin, flags);
        }

        void on_play(card *origin_card, player *origin, player *target, effect_flags flags = {});
    };

    struct request_damage : request_base {
        request_damage(card *origin_card, player *origin, player *target, int damage, effect_flags flags = {});

        ~request_damage();

        int damage;

        player *savior = nullptr;

        std::function<void()> cleanup_function;

        struct timer_damage : request_timer {
            explicit timer_damage(request_damage *request);
            
            void on_finished() override {
                static_cast<request_damage *>(request)->on_finished();
            }
        };

        timer_damage m_timer{this};
        request_timer *timer() override { return &m_timer; }

        std::vector<card *> get_highlights() const override;
        void on_update() override;
        void on_finished();
        game_string status_text(player *owner) const override;
    };
}

#endif