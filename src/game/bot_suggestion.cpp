#include "bot_suggestion.h"
#include "game.h"

namespace banggame::bot_suggestion {

    bool target_enemy::on_check_target(card *origin_card, player *origin, player *target) {
        switch (origin->m_role) {
        case player_role::outlaw:
            return target->m_role == player_role::sheriff
                || target->m_role == player_role::deputy;
        case player_role::sheriff:
        case player_role::deputy:
            return target->m_role == player_role::outlaw
                || target->m_role == player_role::renegade;
        case player_role::renegade: {
            auto targets = std::ranges::filter_view(origin->m_game->m_players, [origin](player *p) {
                return p != origin && p->alive();
            });
            auto num_outlaws = std::ranges::count_if(targets, [](player_role role) {
                return role == player_role::outlaw
                    || role == player_role::renegade;
            }, &player::m_role);
            auto num_sheriff_or_deputy = std::ranges::count_if(targets, [](player_role role) {
                return role == player_role::sheriff
                    || role == player_role::deputy;
            }, &player::m_role);
            if (num_outlaws > num_sheriff_or_deputy) {
                return target->m_role == player_role::outlaw
                    || target->m_role == player_role::renegade
                    && origin != target;
            } else if (num_outlaws < num_sheriff_or_deputy) {
                if (num_outlaws == 0) {
                    return target->m_role == player_role::sheriff;
                } else {
                    return target->m_role == player_role::deputy;
                }
            } else {
                return true;
            }
        }
        case player_role::outlaw_3p:
            return origin->m_game->num_alive() <= 2 || target->m_role == player_role::deputy_3p;
        case player_role::deputy_3p:
            return origin->m_game->num_alive() <= 2 || target->m_role == player_role::renegade_3p;
        case player_role::renegade_3p:
            return origin->m_game->num_alive() <= 2 || target->m_role == player_role::outlaw_3p;
        default:
            return true;
        }
    }
    
    bool target_enemy::on_check_target(card *origin_card, player *origin, card *target) {
        return on_check_target(origin_card, origin, target->owner);
    }

    bool target_friend::on_check_target(card *origin_card, player *origin, player *target) {
        switch (origin->m_role) {
        case player_role::outlaw:
            return target->m_role == player_role::outlaw;
        case player_role::sheriff:
        case player_role::deputy:
            return target->m_role == player_role::sheriff
                || target->m_role == player_role::deputy;
        case player_role::renegade:
        default:
            return origin == target;
        }
    }
    
    bool target_friend::on_check_target(card *origin_card, player *origin, card *target) {
        return on_check_target(origin_card, origin, target->owner);
    }
}