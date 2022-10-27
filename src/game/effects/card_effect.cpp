#include "card_effect.h"

#include "../game.h"
#include "../play_verify.h"

namespace banggame {

    void request_base::on_pick(card *target_card) {
        throw std::runtime_error("missing on_pick(card)");
    }

    bool request_base::can_respond(player *target, card *target_card) const {
        const bool is_response = !bool(flags & effect_flags::force_play);
        return !target->m_game->is_disabled(target_card) && target->is_possible_to_play(target_card, is_response);
    }

    bool request_base::auto_resolve() {
        if (!target || !bool(flags & (effect_flags::auto_pick | effect_flags::auto_respond | effect_flags::auto_respond_empty_hand))) {
            return false;
        }

        auto update = target->m_game->make_request_update(target);
        if (bool(flags & effect_flags::auto_pick) && update.pick_cards.size() == 1 && update.respond_cards.empty()) {
            on_pick(update.pick_cards.front());
            return true;
        }

        if ((bool(flags & effect_flags::auto_respond) || bool(flags & effect_flags::auto_respond_empty_hand) && target->m_hand.empty())
            && update.pick_cards.empty() && update.respond_cards.size() == 1)
        {
            card *origin_card = update.respond_cards.front();
            bool is_response = !bool(flags & effect_flags::force_play);
            auto &effects = is_response ? origin_card->responses : origin_card->effects;
            if (origin_card->equips.empty()
                && origin_card->optionals.empty()
                && origin_card->modifier == card_modifier_type::none
                && std::ranges::all_of(effects, [](const effect_holder &holder) { return holder.target == target_type::none; })
            ) {
                if (!is_response) {
                    target->m_game->pop_request();
                }
                play_card_verify{target, origin_card, is_response,
                    target_list{effects.size(), play_card_target{enums::enum_tag<target_type::none>}}}.do_play_card();
                return true;
            }
        }
        return false;
    }

    void timer_request::tick() {
        if (awaiting_confirms.empty() && duration != ticks{0} && --duration == ticks{0}) {
            auto copy = shared_from_this();
            target->m_game->pop_request();
            on_finished();
            target->m_game->update_request();
        } else if (auto_confirm_timer != ticks{0} && --auto_confirm_timer == ticks{0}) {
            awaiting_confirms.clear();
        }
    }

    void timer_request::add_pending_confirm(player *p) {
        awaiting_confirms.push_back(p);
    }

    void timer_request::confirm_player(player *p) {
        auto it = std::ranges::find(awaiting_confirms, p);
        if (it != awaiting_confirms.end()) {
            awaiting_confirms.erase(it);
        }
    }

    bool selection_picker::can_pick(card *target_card) const {
        return target_card->pocket == pocket_type::selection;
    }
}