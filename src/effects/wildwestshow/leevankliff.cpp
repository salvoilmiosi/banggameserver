#include "leevankliff.h"

#include "cards/filter_enums.h"

#include "game/game.h"

namespace banggame {

    card_ptr get_repeat_playing_card(card_ptr origin_card, const effect_context &ctx) {
        if (ctx.card_choice) {
            return ctx.card_choice;
        } else if (ctx.traincost) {
            return ctx.traincost;
        } else {
            return origin_card;
        }
    }

    game_string modifier_leevankliff::get_error(card_ptr origin_card, player_ptr origin, card_ptr playing_card, const effect_context &ctx) {
        if (!ctx.repeat_card) {
            return {"ERROR_CANT_PLAY_CARD", origin_card};
        }

        playing_card = get_repeat_playing_card(playing_card, ctx);
                
        if (ctx.repeat_card != playing_card) {
            return "INVALID_MODIFIER_CARD";
        }

        if (!playing_card->is_brown()) {
            return "ERROR_CARD_IS_NOT_BROWN";
        }

        return {};
    }

    void modifier_leevankliff::add_context(card_ptr origin_card, player_ptr origin, effect_context &ctx) {
        auto range = origin->m_played_cards | rv::reverse;
        if (auto it = rn::find_if(range,
            [](const played_card_history &history) {
                return history.origin_card.pocket == pocket_type::player_hand
                    || history.origin_card.pocket == pocket_type::none;
            }); it != range.end())
        {
            const played_card_history &history = *it;
            card_ptr playing_card = get_repeat_playing_card(history.origin_card.origin_card, history.context);
            if (playing_card->is_brown() && !rn::contains(history.modifiers, origin_card, &card_pocket_pair::origin_card)) {
                ctx.disable_banglimit = true;
                ctx.repeat_card = playing_card;
            }
        }
    }
}