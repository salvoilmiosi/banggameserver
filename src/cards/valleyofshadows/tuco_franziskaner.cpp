#include "tuco_franziskaner.h"

#include "game/game.h"

#include "cards/base/draw.h"

namespace banggame {

    void equip_tuco_franziskaner::on_enable(card *target_card, player *p) {
        p->m_game->add_listener<event_type::count_cards_to_draw>({target_card, 1}, [p](player *origin, int &value) {
            if (p == origin && std::ranges::none_of(p->m_table, &card::is_blue)) {
                value += 2;
            }
        });
    }
}