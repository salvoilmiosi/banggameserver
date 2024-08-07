#include "ruleset.h"

#include "game/game.h"

#include "effects/base/damage.h"

namespace banggame {
    void ruleset_goldrush::on_apply(game *game) {
        game->add_listener<event_type::on_game_setup>({nullptr, 2}, [](player_ptr origin){
            for (int i=0; i<3; ++i) {
                origin->m_game->draw_shop_card();
            }
        });
        
        game->add_listener<event_type::on_hit>({nullptr, 6}, [=](card_ptr origin_card, player_ptr origin, player_ptr target, int damage, effect_flags flags) {
            if (origin && game->m_playing == origin && origin != target && origin->alive()) {
                origin->add_gold(damage);
            }
        });

        game->add_listener<event_type::on_equip_card>({nullptr, 5}, [](player_ptr origin, player_ptr target, card_ptr origin_card, const effect_context &ctx) {
            if (origin_card->is_black()) {
                origin->m_game->draw_shop_card();
            }
        });
    }
}