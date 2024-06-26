#include "caravan.h"

#include "effects/base/draw.h"

#include "game/game.h"

namespace banggame {
    
    inline effect_draw build_effect_draw(card *origin_card, const effect_context &ctx, int num_cubes) {
        return effect_draw{ctx.selected_cubes.count(origin_card) / num_cubes + 2};
    }

    void effect_caravan::on_play(card *origin_card, player *origin, const effect_context &ctx) {
        build_effect_draw(origin_card, ctx, num_cubes).on_play(origin_card, origin);
    }

}