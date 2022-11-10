#include "judge.h"

#include "game/game.h"

namespace banggame {

    void equip_judge::on_enable(card *target_card, player *target) {
        target->m_game->set_game_flags(game_flags::disable_equipping);
    }
}