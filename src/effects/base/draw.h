#ifndef __BASE_DRAW_H__
#define __BASE_DRAW_H__

#include "cards/card_effect.h"
#include "pick.h"

namespace banggame {
    
    struct effect_draw {
        int ncards;
        effect_draw(int value) : ncards(std::max(1, value)) {}
        
        void on_play(card *origin_card, player *origin) {
            on_play(origin_card, origin, origin);
        }
        void on_play(card *origin_card, player *origin, player *target);
    };

    DEFINE_EFFECT(draw, effect_draw)

    struct effect_queue_draw {
        int ncards;
        effect_queue_draw(int value) : ncards(std::max(1, value)) {}

        void on_play(card *origin_card, player *origin) {
            on_play(origin_card, origin, origin);
        }
        void on_play(card *origin_card, player *origin, player *target);
    };

    DEFINE_EFFECT(queue_draw, effect_queue_draw)

    struct effect_draw_discard {
        game_string get_error(card *origin_card, player *origin) {
            return get_error(origin_card, origin, origin);
        }
        game_string get_error(card *origin_card, player *origin, player *target);

        void on_play(card *origin_card, player *origin) {
            return on_play(origin_card, origin, origin);
        }
        void on_play(card *origin_card, player *origin, player *target);
    };

    DEFINE_EFFECT(draw_discard, effect_draw_discard)

    struct effect_draw_to_discard {
        int ncards;
        effect_draw_to_discard(int value) : ncards(std::max(1, value)) {}

        void on_play(card *origin_card, player *origin);
    };

    DEFINE_EFFECT(draw_to_discard, effect_draw_to_discard)

    struct effect_while_drawing {
        bool can_play(card *origin_card, player *origin);
    };

    DEFINE_EFFECT(while_drawing, effect_while_drawing)

    struct effect_no_cards_drawn {
        bool can_play(card *origin_card, player *origin);
    };

    DEFINE_EFFECT(no_cards_drawn, effect_no_cards_drawn)

    struct effect_add_draw_card: effect_while_drawing {
        void on_play(card *origin_card, player *origin);
    };

    DEFINE_EFFECT(add_draw_card, effect_add_draw_card)

    struct effect_skip_drawing: effect_while_drawing {
        void on_play(card *origin_card, player *origin);
    };

    DEFINE_EFFECT(skip_drawing, effect_skip_drawing)

    struct request_draw : request_picking, std::enable_shared_from_this<request_draw> {
        request_draw(player *target);

        std::vector<card *> cards_from_selection;
        
        int num_drawn_cards = 0;
        int num_cards_to_draw = 2;
        
        card *phase_one_drawn_card();
        void add_to_hand_phase_one(card *target_card);
        void cleanup_selection();

        void on_update() override;
        bool can_pick(card *target_card) const override;
        void on_pick(card *target_card) override;
        game_string status_text(player *owner) const override;
    };
    
    using shared_request_draw = std::shared_ptr<request_draw>;

    namespace event_type {
        DEFINE_STRUCT(count_cards_to_draw,
            (player *, origin)
            (nullable_ref<int>, cards_to_draw)
        )
        
        DEFINE_STRUCT(on_draw_from_deck,
            (player *, origin)
            (shared_request_draw, req)
            (nullable_ref<bool>, handled)
        )

        DEFINE_STRUCT(on_card_drawn,
            (player *, origin)
            (card *, target_card)
            (shared_request_draw, req)
            (nullable_ref<bool>, reveal)
        )
    }
}

#endif