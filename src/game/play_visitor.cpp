#include "play_visitor.h"

namespace banggame {

    using namespace enums::flag_operators;

    opt_error play_visitor<target_type::none>::verify(const play_card_verify *verifier, const effect_holder &effect) {
        if (effect.target != target_type::none) {
            return game_error("ERROR_INVALID_ACTION");
        } else {
            return effect.verify(verifier->card_ptr, verifier->origin);
        }
    }

    opt_fmt_str play_visitor<target_type::none>::prompt(const play_card_verify *verifier, const effect_holder &effect) {
        return effect.on_prompt(verifier->card_ptr, verifier->origin);
    }

    void play_visitor<target_type::none>::play(const play_card_verify *verifier, const effect_holder &effect) {
        effect.on_play(verifier->card_ptr, verifier->origin, effect_flags{});
    }

    opt_error play_visitor<target_type::player>::verify(const play_card_verify *verifier, const effect_holder &effect, player *target) {
        if (!target || (effect.target != target_type::player && effect.target != target_type::conditional_player)) {
            return game_error("ERROR_INVALID_ACTION");
        } else if (auto error = check_player_filter(verifier->card_ptr, verifier->origin, effect.player_filter, target)) {
            return error;
        } else {
            return effect.verify(verifier->card_ptr, verifier->origin, target);
        }
    }

    opt_fmt_str play_visitor<target_type::player>::prompt(const play_card_verify *verifier, const effect_holder &effect, player *target) {
        if (target) {
            return effect.on_prompt(verifier->card_ptr, verifier->origin, target);
        } else {
            return std::nullopt;
        }
    }

    void play_visitor<target_type::player>::play(const play_card_verify *verifier, const effect_holder &effect, player *target) {
        if (target == verifier->origin || !target->immune_to(verifier->card_ptr)) {
            auto flags = effect_flags::single_target;
            if (verifier->card_ptr->sign && verifier->card_ptr->color == card_color_type::brown) {
                flags |= effect_flags::escapable;
            }
            effect.on_play(verifier->card_ptr, verifier->origin, target, flags);
        }
    }

    opt_error play_visitor<target_type::conditional_player>::verify(const play_card_verify *verifier, const effect_holder &effect) {
        if (effect.target != target_type::conditional_player) {
            return game_error("ERROR_INVALID_ACTION");
        } else {
            // TODO check set target validi
            return std::nullopt;
        }
    }

    opt_fmt_str play_visitor<target_type::conditional_player>::prompt(const play_card_verify *verifier, const effect_holder &effect) {
        return std::nullopt;
    }

    void play_visitor<target_type::conditional_player>::play(const play_card_verify *verifier, const effect_holder &effect) {
        // do nothing
    }

    opt_error play_visitor<target_type::other_players>::verify(const play_card_verify *verifier, const effect_holder &effect) {
        if (effect.target != target_type::other_players) {
            return game_error("ERROR_INVALID_ACTION");
        } else {
            for (player &p : range_other_players(verifier->origin)) {
                if (auto error = effect.verify(verifier->card_ptr, verifier->origin, &p)) {
                    return error;
                }
            }
            return std::nullopt;
        }
    }

    opt_fmt_str play_visitor<target_type::other_players>::prompt(const play_card_verify *verifier, const effect_holder &effect) {
        opt_fmt_str msg = std::nullopt;
        for (player &p : range_other_players(verifier->origin)) {
            msg = effect.on_prompt(verifier->card_ptr, verifier->origin, &p);
            if (!msg) break;
        }
        return msg;
    }

    void play_visitor<target_type::other_players>::play(const play_card_verify *verifier, const effect_holder &effect) {
        auto targets = range_other_players(verifier->origin);
        
        effect_flags flags{};
        if (verifier->card_ptr->sign && verifier->card_ptr->color == card_color_type::brown) {
            flags |= effect_flags::escapable;
        }
        if (std::ranges::distance(targets) == 1) {
            flags |= effect_flags::single_target;
        }
        for (player &p : targets) {
            if (!p.immune_to(verifier->card_ptr)) {
                effect.on_play(verifier->card_ptr, verifier->origin, &p, flags);
            }
        }
    }

    opt_error play_visitor<target_type::all_players>::verify(const play_card_verify *verifier, const effect_holder &effect) {
        if (effect.target != target_type::all_players) {
            return game_error("ERROR_INVALID_ACTION");
        } else {
            for (player &p : range_all_players(verifier->origin)) {
                if (auto error = effect.verify(verifier->card_ptr, verifier->origin, &p)) {
                    return error;
                }
            }
            return std::nullopt;
        }
    }

    opt_fmt_str play_visitor<target_type::all_players>::prompt(const play_card_verify *verifier, const effect_holder &effect) {
        opt_fmt_str msg = std::nullopt;
        for (player &p : range_all_players(verifier->origin)) {
            msg = effect.on_prompt(verifier->card_ptr, verifier->origin, &p);
            if (!msg) break;
        }
        return msg;
    }

    void play_visitor<target_type::all_players>::play(const play_card_verify *verifier, const effect_holder &effect) {
        effect_flags flags{};
        if (verifier->card_ptr->sign && verifier->card_ptr->color == card_color_type::brown) {
            flags |= effect_flags::escapable;
        }
        for (player &p : range_all_players(verifier->origin)) {
            if (!p.immune_to(verifier->card_ptr)) {
                effect.on_play(verifier->card_ptr, verifier->origin, &p, flags);
            }
        }
    }

    opt_error play_visitor<target_type::card>::verify(const play_card_verify *verifier, const effect_holder &effect, card *target) {
        if (effect.target != target_type::card) {
            return game_error("ERROR_INVALID_ACTION");
        } else if (!target->owner) {
            return game_error("ERROR_INVALID_ACTION");
        } else if (auto error = check_player_filter(verifier->card_ptr, verifier->origin, effect.player_filter, target->owner)) {
            return error;
        } else if (auto error = check_card_filter(verifier->card_ptr, verifier->origin, effect.card_filter, target)) {
            return error;
        } else {
            return effect.verify(verifier->card_ptr, verifier->origin, target);
        }
    }

    opt_fmt_str play_visitor<target_type::card>::prompt(const play_card_verify *verifier, const effect_holder &effect, card *target) {
        return effect.on_prompt(verifier->card_ptr, verifier->origin, target);
    }

    void play_visitor<target_type::card>::play(const play_card_verify *verifier, const effect_holder &effect, card *target) {
        auto flags = effect_flags::single_target;
        if (verifier->card_ptr->sign && verifier->card_ptr->color == card_color_type::brown) {
            flags |= effect_flags::escapable;
        }
        if (target->owner == verifier->origin) {
            effect.on_play(verifier->card_ptr, verifier->origin, target, flags);
        } else if (!target->owner->immune_to(verifier->card_ptr)) {
            if (target->pocket == pocket_type::player_hand) {
                effect.on_play(verifier->card_ptr, verifier->origin, target->owner->random_hand_card(), flags);
            } else {
                effect.on_play(verifier->card_ptr, verifier->origin, target, flags);
            }
        }
    }

    opt_error play_visitor<target_type::cards_other_players>::verify(const play_card_verify *verifier, const effect_holder &effect, const std::vector<card *> &target_cards) {
        if (effect.target != target_type::cards_other_players) {
            return game_error("ERROR_INVALID_ACTION");
        } else if (!std::ranges::all_of(verifier->origin->m_game->m_players | std::views::filter(&player::alive), [&](const player &p) {
            int found = std::ranges::count(target_cards, &p, &card::owner);
            if (p.m_hand.empty() && p.m_table.empty()) return found == 0;
            if (&p == verifier->origin) return found == 0;
            else return found == 1;
        })) {
            return game_error("ERROR_INVALID_TARGETS");
        } else {
            for (card *c : target_cards) {
                if (auto error = effect.verify(verifier->card_ptr, verifier->origin, c)) {
                    return error;
                }
            }
            return std::nullopt;
        }
    }

    opt_fmt_str play_visitor<target_type::cards_other_players>::prompt(const play_card_verify *verifier, const effect_holder &effect, const std::vector<card *> &target_cards) {
        opt_fmt_str msg = std::nullopt;
        for (card *target_card : target_cards) {
            msg = effect.on_prompt(verifier->card_ptr, verifier->origin, target_card);
            if (!msg) break;
        }
        return msg;
    }

    void play_visitor<target_type::cards_other_players>::play(const play_card_verify *verifier, const effect_holder &effect, const std::vector<card *> &target_cards) {
        effect_flags flags{};
        if (verifier->card_ptr->sign && verifier->card_ptr->color == card_color_type::brown) {
            flags |= effect_flags::escapable;
        }
        if (target_cards.size() == 1) {
            flags |= effect_flags::single_target;
        }
        for (card *target_card : target_cards) {
            if (target_card->pocket == pocket_type::player_hand) {
                effect.on_play(verifier->card_ptr, verifier->origin, target_card->owner->random_hand_card(), flags);
            } else {
                effect.on_play(verifier->card_ptr, verifier->origin, target_card, flags);
            }
        }
    }

    opt_error play_visitor<target_type::cube>::verify(const play_card_verify *verifier, const effect_holder &effect, const std::vector<card *> &target_cards) {
        if (effect.target != target_type::cube) {
            return game_error("ERROR_INVALID_ACTION");
        } else {
            for (card *c : target_cards) {
                if (!c || c->owner != verifier->origin) {
                    return game_error("ERROR_INVALID_ACTION");
                }
                if (auto error = effect.verify(verifier->card_ptr, verifier->origin, c)) {
                    return error;
                }
            }
            return std::nullopt;
        }
    }

    opt_fmt_str play_visitor<target_type::cube>::prompt(const play_card_verify *verifier, const effect_holder &effect, const std::vector<card *> &target_cards) {
        opt_fmt_str msg = std::nullopt;
        for (card *target_card : target_cards) {
            msg = effect.on_prompt(verifier->card_ptr, verifier->origin, target_card);
            if (!msg) break;
        }
        return msg;
    }

    void play_visitor<target_type::cube>::play(const play_card_verify *verifier, const effect_holder &effect, const std::vector<card *> &target_cards) {
        for (card *target_card : target_cards) {
            effect.on_play(verifier->card_ptr, verifier->origin, target_card, effect_flags{});
        }
    }
}