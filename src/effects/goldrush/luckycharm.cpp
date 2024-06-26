#include "luckycharm.h"

#include "game/game.h"
#include "effects/base/damage.h"

namespace banggame {

    void equip_luckycharm::on_enable(card *target_card, player *p) {
        p->m_game->add_listener<event_type::on_hit>(target_card, [=](card *origin_card, player *origin, player *target, int damage, effect_flags flags) {
            if (p == target) {
                target->m_game->queue_action([=]{
                    if (target->alive()) {
                        target_card->flash_card();
                        target->add_gold(damage);
                    }
                });
            }
        });
    }
}