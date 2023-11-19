#include "fanning.h"

#include "cards/base/bang.h"

#include "cards/game_enums.h"

#include "game/game.h"
#include "game/bot_suggestion.h"


namespace banggame {

    game_string handler_fanning::get_error(card *origin_card, player *origin, player *target1, player *target2) {
        if (target1 == target2 || origin->m_game->calc_distance(target1, target2) + target2->get_distance_mod() > 1) {
            return "ERROR_TARGETS_NOT_ADJACENT";
        }
        return {};
    }

    bool handler_fanning::on_check_target(card *origin_card, player *origin, player *target1, player *target2) {
        return bot_suggestion::target_enemy{}.on_check_target(origin_card, origin, target1)
            && bot_suggestion::target_enemy{}.on_check_target(origin_card, origin, target2);
    }

    void handler_fanning::on_play(card *origin_card, player *origin, player *target1, player *target2) {
        effect_bang{}.on_play(origin_card, origin, target1, effect_flags::escapable);
        effect_bang{}.on_play(origin_card, origin, target2, effect_flags::escapable);
    }
}