#include "game/play_verify.h"

namespace banggame {

    using visit_cubes = play_visitor<target_type::select_cubes>;

    template<> bool visit_cubes::possible(const effect_context &ctx) {
        return origin->count_cubes() >= effect.target_value;
    }

    template<> serial::card_list visit_cubes::random_target(const effect_context &ctx) {
        auto cubes = origin->cube_slots()
            | rv::for_each([](card *slot) {
                return rv::repeat_n(slot, slot->num_cubes);
            })
            | rn::to_vector;
        return cubes
            | rv::sample(effect.target_value, origin->m_game->bot_rng)
            | rn::to<serial::card_list>;
    }

    template<> game_string visit_cubes::get_error(const effect_context &ctx, const serial::card_list &target_cards) {
        if (target_cards.size() != effect.target_value) {
            return "ERROR_INVALID_TARGETS";
        }
        for (card *c : target_cards) {
            if (c->owner != origin) {
                return "ERROR_TARGET_NOT_SELF";
            }
        }
        return {};
    }

    template<> game_string visit_cubes::prompt(const effect_context &ctx, const serial::card_list &target_cards) {
        return {};
    }

    template<> void visit_cubes::add_context(effect_context &ctx, const serial::card_list &target_cards) {
        for (card *target : target_cards) {
            effect.type->add_context_card(effect.effect_value, origin_card, origin, target, ctx);
        }
    }

    template<> void visit_cubes::play(const effect_context &ctx, const serial::card_list &target_cards) {
        effect.type->on_play(effect.effect_value, origin_card, origin, {}, ctx);
    }

}