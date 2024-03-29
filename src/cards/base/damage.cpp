#include "damage.h"

#include "cards/game_enums.h"
#include "game/game.h"
#include "deathsave.h"

namespace banggame {
    
    game_string effect_damage::get_error(card *origin_card, player *origin, effect_flags flags) {
        if (origin->m_hp <= damage) {
            return "ERROR_CANT_SELF_DAMAGE";
        }
        return {};
    }

    void effect_damage::on_play(card *origin_card, player *origin, player *target, effect_flags flags) {
        target->damage(origin_card, origin, damage, flags);
    }

    request_damage::timer_damage::timer_damage(request_damage *request)
        : request_timer(request, request->target->m_game->m_options.damage_timer) {}

    request_damage::request_damage(card *origin_card, player *origin, player *target, int damage, effect_flags flags)
        : request_base(origin_card, origin, target, flags & ~(effect_flags::escapable | effect_flags::single_target), 200)
        , damage(damage) {}
    
    std::vector<card *> request_damage::get_highlights() const {
        return target->m_backup_character;
    }

    void request_damage::on_update() {
        if (target->is_ghost()) {
            target->m_game->pop_request();
        } else {
            bool handled = false;
            target->m_game->call_event(event_type::check_damage_response{ target, handled });
            if (!handled) {
                target->m_game->pop_request();
                on_finished();
            }
        }
    }

    void request_damage::on_finished() {
        if (bool(flags & effect_flags::play_as_bang)) {
            if (bool(flags & effect_flags::multi_target)) {
                target->m_game->add_log("LOG_TAKEN_DAMAGE_AS_GATLING", origin_card, target);
            } else {
                target->m_game->add_log("LOG_TAKEN_DAMAGE_AS_BANG", origin_card, target, damage);
            }
        } else {
            target->m_game->add_log("LOG_TAKEN_DAMAGE", origin_card, target, damage);
        }
        target->set_hp(target->m_hp - damage);
        target->m_game->queue_request<request_death>(origin_card, origin, target);
        target->m_game->call_event(event_type::on_hit{ origin_card, origin, target, damage, flags });
    }

    game_string request_damage::status_text(player *owner) const {
        if (bool(flags & effect_flags::play_as_bang)) {
            if (bool(flags & effect_flags::multi_target)) {
                return {"STATUS_DAMAGING_AS_GATLING", target, origin_card};
            } else {
                return {"STATUS_DAMAGING_AS_BANG", target, origin_card, damage};
            }
        } else {
            return {"STATUS_DAMAGING", target, origin_card, damage};
        }
    }

}