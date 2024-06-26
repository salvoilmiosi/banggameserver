#include "greg_digger.h"

#include "game/game.h"

#include "effects/base/deathsave.h"

namespace banggame {

    void equip_greg_digger::on_enable(card *target_card, player *p) {
        p->m_game->add_listener<event_type::on_player_death>({target_card, 1}, [p](player *origin, player *target) {
            if (p != target) {
                p->m_game->queue_action([=]{
                    if (p->alive()) {
                        p->heal(2);
                    }
                });
            }
        });
    }
}