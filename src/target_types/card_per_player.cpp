#include "game/play_verify.h"

#include "cards/game_enums.h"

#include "game/filters.h"

namespace banggame {

    using visit_cards = play_visitor<target_type::card_per_player>;

    template<> bool visit_cards::possible(const effect_context &ctx) {
        return true;
    }

    static auto cards_target_set(const player *origin, const card *origin_card, target_card_filter filter, player *target, const effect_context &ctx) {
        return rv::concat(target->m_table, target->m_hand)
            | rv::filter([&](const card *target_card) {
                return !filters::check_card_filter(origin_card, origin, filter, target_card, ctx);
            });
    }

    template<> serial::card_list visit_cards::random_target(const effect_context &ctx) {
        serial::card_list ret;
        for (player *target : range_all_players(origin)) {
            if (!filters::check_player_filter(origin, effect.player_filter, target, ctx)) {
                if (auto targets = cards_target_set(origin, origin_card, effect.card_filter, target, ctx)) {
                    ret.push_back(random_element(targets, origin->m_game->bot_rng));
                }
            }
        }
        return ret;
    }

    template<> game_string visit_cards::get_error(const effect_context &ctx, const serial::card_list &target_cards) {
        if (!rn::all_of(origin->m_game->m_players | rv::filter(&player::alive), [&](player *p) {
            size_t found = rn::count(target_cards, p, &card::owner);
            if (rn::empty(cards_target_set(origin, origin_card, effect.card_filter, p, ctx))) return found == 0;
            if (p == ctx.skipped_player || filters::check_player_filter(origin, effect.player_filter, p, ctx)) return found == 0;
            else return found == 1;
        })) {
            return "ERROR_INVALID_TARGETS";
        } else {
            for (card *c : target_cards) {
                MAYBE_RETURN(filters::check_card_filter(origin_card, origin, effect.card_filter, c, ctx));
                MAYBE_RETURN(effect.type->get_error_card(effect.effect_value, origin_card, origin, c, ctx));
            }
            return {};
        }
    }

    template<> game_string visit_cards::prompt(const effect_context &ctx, const serial::card_list &target_cards) {
        if (target_cards.empty()) {
            return {"PROMPT_CARD_NO_EFFECT", origin_card};
        }
        game_string msg;
        for (card *target_card : target_cards) {
            msg = effect.type->on_prompt_card(effect.effect_value, origin_card, origin, target_card, ctx);
            if (!msg) break;
        }
        return msg;
    }

    template<> void visit_cards::add_context(effect_context &ctx, const serial::card_list &target_cards) {
        for (card *target_card : target_cards) {
            effect.type->add_context_card(effect.effect_value, origin_card, origin, target_card, ctx);
        }
    }

    template<> void visit_cards::play(const effect_context &ctx, const serial::card_list &target_cards) {
        auto flags = effect_flags::multi_target;
        if (origin_card->is_brown()) {
            flags |= effect_flags::escapable;
        }
        if (target_cards.size() == 1) {
            flags |= effect_flags::single_target;
        }
        for (card *target_card : target_cards) {
            if (target_card->pocket == pocket_type::player_hand) {
                effect.type->on_play_card(effect.effect_value, origin_card, origin, target_card->owner->random_hand_card(), flags, ctx);
            } else {
                effect.type->on_play_card(effect.effect_value, origin_card, origin, target_card, flags, ctx);
            }
        }
    }

}