#include "effects.h"
#include "requests.h"

#include "../valleyofshadows/requests.h"

#include "../../game.h"

namespace banggame {
    using namespace enums::flag_operators;

    void effect_play_card_action::on_play(card *origin_card, player *origin) {
        origin->play_card_action(origin_card);
    }

    opt_error effect_max_usages::verify(card *origin_card, player *origin) {
        if (origin_card->usages >= max_usages) {
            return game_error("ERROR_MAX_USAGES", origin_card, max_usages);
        }
        return std::nullopt;
    }
    void effect_max_usages::on_play(card *origin_card, player *origin) {
        ++origin_card->usages;
    }

    opt_error effect_pass_turn::verify(card *origin_card, player *origin) {
        card *mandatory_card = nullptr;
        origin->m_game->call_event<event_type::verify_mandatory_card>(origin, mandatory_card);
        if (mandatory_card) {
            return game_error("ERROR_MANDATORY_CARD", mandatory_card);
        }
        return std::nullopt;
    }

    opt_fmt_str effect_pass_turn::on_prompt(card *origin_card, player *origin) {
        int diff = origin->m_hand.size() - origin->max_cards_end_of_turn();
        if (diff == 1) {
            return game_formatted_string{"PROMPT_PASS_DISCARD"};
        } else if (diff > 1) {
            return game_formatted_string{"PROMPT_PASS_DISCARD_PLURAL", diff};
        }
        return std::nullopt;
    }

    void effect_pass_turn::on_play(card *origin_card, player *origin) {
        origin->pass_turn();
    }

    bool effect_resolve::can_respond(card *origin_card, player *origin) {
        return origin->m_game->top_request_is<resolvable_request>(origin);
    }
    
    void effect_resolve::on_play(card *origin_card, player *origin) {
        auto copy = origin->m_game->top_request();
        copy.get<resolvable_request>().on_resolve();
    }

    opt_error effect_self_damage::verify(card *origin_card, player *origin) {
        if (origin->m_hp <= 1) {
            return game_error("ERROR_CANT_SELF_DAMAGE");
        }
        return std::nullopt;
    }

    void effect_self_damage::on_play(card *origin_card, player *origin) {
        origin->damage(origin_card, origin, 1);
    }

    void effect_bang::on_play(card *origin_card, player *origin, player *target, effect_flags flags) {
        target->m_game->add_log("LOG_PLAYED_CARD_ON", origin_card, origin, target);
        target->m_game->queue_request<request_bang>(origin_card, origin, target, flags);
    }

    static void queue_request_bang(std::shared_ptr<request_bang> &&req) {
        req->is_bang_card = true;
        req->origin->m_game->call_event<event_type::apply_bang_modifier>(req->origin, req.get());
        req->origin->m_game->queue_action([req = std::move(req)]() mutable {
            if (!req->target->immune_to(req->origin_card)) {
                req->origin->m_game->queue_request(std::move(req));
            }
        });
    }

    void handler_bangcard::on_play(card *origin_card, player *origin, player *target) {
        origin->m_game->add_log("LOG_PLAYED_CARD_ON", origin_card, origin, target);
        queue_request_bang(std::make_shared<request_bang>(origin_card, origin, target, effect_flags::single_target));
    }

    void handler_play_as_bang::on_play(card *origin_card, player *origin, card *chosen_card, player *target) {
        origin->m_game->add_log("LOG_PLAYED_CARD_AS_BANG_ON", chosen_card, origin, target);
        origin->discard_card(chosen_card);
        queue_request_bang(std::make_shared<request_card_as_bang>(chosen_card, origin, target, effect_flags::single_target));
    }

    bool effect_missedlike::can_respond(card *origin_card, player *origin) {
        if (auto *req = origin->m_game->top_request_if<missable_request>(origin)) {
            return req->can_respond(origin_card);
        }
        return false;
    }

    void effect_missed::on_play(card *origin_card, player *origin) {
        origin->m_game->top_request().get<missable_request>().on_miss();
    }

    void effect_barrel::on_play(card *origin_card, player *target) {
        target->m_game->top_request().get<missable_request>().add_card(origin_card);
        target->m_game->update_request();
        target->m_game->draw_check_then(target, origin_card, [=](card *drawn_card) {
            if (target->get_card_sign(drawn_card).suit == card_suit::hearts) {
                target->m_game->add_log("LOG_CARD_HAS_EFFECT", origin_card);
                effect_missed().on_play(origin_card, target);
            }
        });
    }

    opt_error effect_banglimit::verify(card *origin_card, player *origin) {
        bool value = origin->m_bangs_played < origin->m_bangs_per_turn;
        origin->m_game->call_event<event_type::apply_volcanic_modifier>(origin, value);
        if (!value) {
            return game_error("ERROR_ONE_BANG_PER_TURN");
        }
        return std::nullopt;
    }

    void effect_banglimit::on_play(card *origin_card, player *origin) {
        ++origin->m_bangs_played;
    }

    void effect_indians::on_play(card *origin_card, player *origin, player *target, effect_flags flags) {
        target->m_game->add_log("LOG_PLAYED_CARD_ON", origin_card, origin, target);
        target->m_game->queue_request<request_indians>(origin_card, origin, target, flags);
    }

    void effect_duel::on_play(card *origin_card, player *origin, player *target, effect_flags flags) {
        target->m_game->add_log("LOG_PLAYED_CARD_ON", origin_card, origin, target);
        target->m_game->queue_request<request_duel>(origin_card, origin, target, origin, flags);
    }

    void effect_generalstore::on_play(card *origin_card, player *origin) {
        for (int i=0; i<origin->m_game->num_alive(); ++i) {
            origin->m_game->draw_card_to(pocket_type::selection);
        }
        origin->m_game->queue_request<request_generalstore>(origin_card, origin, origin);
    }

    opt_fmt_str effect_heal::on_prompt(card *origin_card, player *origin, player *target) {
        if (target->m_hp == target->m_max_hp) {
            return game_formatted_string{"PROMPT_CARD_NO_EFFECT", origin_card};
        }
        return std::nullopt;
    }

    void effect_heal::on_play(card *origin_card, player *origin, player *target) {
        target->heal(amount);
    }

    opt_error effect_heal_notfull::verify(card *origin_card, player *origin, player *target) {
        if (target->m_hp == target->m_max_hp) {
            return game_error("ERROR_CANT_HEAL_PAST_FULL_HP");
        }
        return std::nullopt;
    }

    opt_fmt_str effect_beer::on_prompt(card *origin_card, player *origin, player *target) {
        if ((target->m_game->m_players.size() > 2 && target->m_game->num_alive() == 2)
            || (target->m_hp == target->m_max_hp)) {
            return game_formatted_string{"PROMPT_CARD_NO_EFFECT", origin_card};
        }
        return std::nullopt;
    }

    void effect_beer::on_play(card *origin_card, player *origin, player *target) {
        target->m_game->call_event<event_type::on_play_beer>(target);
        if (target->m_game->m_players.size() <= 2 || target->m_game->num_alive() > 2) {
            int amt = 1;
            target->m_game->call_event<event_type::apply_beer_modifier>(target, amt);
            target->heal(amt);
        }
    }

    bool effect_deathsave::can_respond(card *origin_card, player *origin) {
        return origin->m_game->top_request_if<request_death>(origin);
    }

    void effect_deathsave::on_play(card *origin_card, player *origin) {
        if (origin->m_hp > 0) {
            origin->m_game->pop_request();
            origin->m_game->update_request();
        } else {
            origin->m_game->top_request().get<request_death>().tried_save = true;
        }
    }

    opt_fmt_str prompt_target_self_hand::on_prompt(card *origin_card, player *origin, card *target_card) {
        if (origin == target_card->owner && target_card->pocket == pocket_type::player_hand) {
            return game_formatted_string{"PROMPT_TARGET_OWN_HAND", origin_card};
        }
        return std::nullopt;
    }

    void effect_steal::on_play(card *origin_card, player *origin, card *target_card, effect_flags flags) {
        if (origin != target_card->owner && target_card->owner->can_escape(origin, origin_card, flags)) {
            origin->m_game->queue_request<request_steal>(origin_card, origin, target_card->owner, target_card, flags);
        } else {
            resolver{origin_card, origin, target_card}.resolve();
        }
    }

    void effect_steal::resolver::resolve() const {
        if (origin->m_game->num_queued_requests([&]{
            origin->m_game->call_event<event_type::on_discard_card>(origin, target_card->owner, target_card);
        })) {
            origin->m_game->queue_action_front([*this]{ finalize(); });
        } else {
            finalize();
        }
    }

    void effect_steal::resolver::finalize() const {
        if (origin->alive()) {
            auto priv_target = update_target::includes(origin, target_card->owner);
            auto inv_target = update_target::excludes(origin, target_card->owner);
            if (origin != target_card->owner) {
                origin->m_game->add_log(priv_target, "LOG_STOLEN_CARD", origin, target_card->owner, target_card);
                if (target_card->pocket == pocket_type::player_hand) {
                    origin->m_game->add_log(inv_target, "LOG_STOLEN_CARD_FROM_HAND", origin, target_card->owner);
                } else {
                    origin->m_game->add_log(inv_target, "LOG_STOLEN_CARD", origin, target_card->owner, target_card);
                }
            } else {
                origin->m_game->add_log(priv_target, "LOG_STOLEN_SELF_CARD", origin, target_card);
                if (target_card->pocket == pocket_type::player_hand) {
                    origin->m_game->add_log(inv_target, "LOG_STOLEN_SELF_CARD_FROM_HAND", origin);
                } else {
                    origin->m_game->add_log(inv_target, "LOG_STOLEN_SELF_CARD", origin, target_card);
                }
            }
            origin->steal_card(target_card);
        }
    }

    void effect_discard::on_play(card *origin_card, player *origin, card *target_card, effect_flags flags) {
        if (origin != target_card->owner && target_card->owner->can_escape(origin, origin_card, flags)) {
            origin->m_game->queue_request<request_destroy>(origin_card, origin, target_card->owner, target_card, flags);
        } else {
            resolver{origin_card, origin, target_card}.resolve();
        }
    }

    void effect_discard::resolver::resolve() const {
        if (origin->m_game->num_queued_requests([&]{
            origin->m_game->call_event<event_type::on_discard_card>(origin, target_card->owner, target_card);
        })) {
            origin->m_game->queue_action_front([*this]{ finalize(); });
        } else {
            finalize();
        }
    }

    void effect_discard::resolver::finalize() const {
        if (origin->alive()) {
            if (origin != target_card->owner) {
                origin->m_game->add_log("LOG_DISCARDED_CARD", origin, target_card->owner, target_card);
            } else {
                origin->m_game->add_log("LOG_DISCARDED_SELF_CARD", target_card->owner, target_card);
            }
            target_card->owner->discard_card(target_card);
        }
    }

    bool effect_while_drawing::can_respond(card *origin_card, player *origin) {
        return origin->m_game->top_request_is<request_draw>(origin);
    }

    void effect_end_drawing::on_play(card *origin_card, player *origin) {
        if (origin->m_game->top_request_is<request_draw>()) {
            origin->m_game->pop_request();
            origin->m_game->add_listener<event_type::on_effect_end>(origin_card, [=](player *p, card *c) {
                if (p == origin && c == origin_card) {
                    origin->m_game->queue_action([=]{
                        origin->m_game->call_event<event_type::post_draw_cards>(origin);
                    });
                    origin->m_game->remove_listeners(origin_card);
                }
            });
            origin->m_game->update_request();
        }
    }

    void effect_draw::on_play(card *origin_card, player *origin, player *target) {
        target->draw_card(ncards, origin_card);
    }

    opt_error effect_draw_discard::verify(card *origin_card, player *origin, player *target) {
        if (target->m_game->m_discards.empty()) {
            return game_error("ERROR_DISCARD_PILE_EMPTY");
        }
        return std::nullopt;
    }

    void effect_draw_discard::on_play(card *origin_card, player *origin, player *target) {
        card *drawn_card = target->m_game->m_discards.back();
        target->m_game->add_log("LOG_DRAWN_FROM_DISCARD", target, drawn_card);
        target->add_to_hand(drawn_card);
    }

    void effect_draw_to_discard::on_play(card *origin_card, player *origin) {
        for (int i=0; i<ncards; ++i) {
            origin->m_game->draw_card_to(pocket_type::discard_pile);
        }
    }

    void effect_draw_one_less::on_play(card *origin_card, player *target) {
        target->m_game->queue_action([=]{
            if (target->alive()) {
                ++target->m_num_drawn_cards;
                while (target->m_num_drawn_cards < target->m_num_cards_to_draw) {
                    target->add_to_hand_phase_one(target->m_game->phase_one_drawn_card());
                }
            }
        });
    }

}