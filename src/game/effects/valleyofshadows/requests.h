#ifndef __VALLEYOFSHADOWS_REQUESTS_H__
#define __VALLEYOFSHADOWS_REQUESTS_H__

#include "../card_effect.h"

#include "../base/requests.h"

namespace banggame {

    struct request_targeting : request_base {
        request_targeting(card *origin_card, player *origin, player *target, card *target_card, effect_flags flags = {})
            : request_base(origin_card, origin, target, flags)
            , target_card(target_card) {}
        
        card *target_card;
        virtual std::vector<card *> get_highlights() const override;
    };

    struct request_destroy : request_targeting, resolvable_request {
        using request_targeting::request_targeting;

        void on_resolve() override;
        game_string status_text(player *owner) const override;
    };

    struct request_steal : request_targeting, resolvable_request {
        using request_targeting::request_targeting;

        void on_resolve() override;
        game_string status_text(player *owner) const override;
    };
    
    struct request_card_as_gatling : request_bang {
        using request_bang::request_bang;
        game_string status_text(player *owner) const override;
    };

    struct request_bandidos : request_base, resolvable_request {
        using request_base::request_base;

        int num_cards = 2;

        bool can_pick(pocket_type pocket, player *target, card *target_card) const override;
        void on_pick(pocket_type pocket, player *target, card *target_card) override;
        void on_resolve() override;
        game_string status_text(player *owner) const override;
    };

    struct request_tornado : request_base {
        using request_base::request_base;
        
        bool can_pick(pocket_type pocket, player *target, card *target_card) const override;
        void on_pick(pocket_type pocket, player *target, card *target_card) override;
        game_string status_text(player *owner) const override;
    };

    struct request_poker : request_base {
        using request_base::request_base;

        bool can_pick(pocket_type pocket, player *target, card *target_card) const override;
        void on_pick(pocket_type pocket, player *target, card *target_card) override;
        game_string status_text(player *owner) const override;
    };

    struct request_poker_draw : selection_picker {
        request_poker_draw(card *origin_card, player *target)
            : selection_picker(origin_card, nullptr, target) {}

        int num_cards = 2;

        void on_pick(pocket_type pocket, player *target, card *target_card) override;
        game_string status_text(player *owner) const override;
    };

    struct timer_damaging : timer_request, cleanup_request {
        timer_damaging(card *origin_card, player *origin, player *target, int damage, bool is_bang)
            : timer_request(origin_card, origin, target)
            , damage(damage)
            , is_bang(is_bang) {}
        
        int damage;
        bool is_bang;

        void on_finished() override;
        game_string status_text(player *owner) const override;
    };

    struct request_saved : request_base {
        request_saved(card *origin_card, player *target, player *saved)
            : request_base(origin_card, nullptr, target, effect_flags::auto_pick)
            , saved(saved) {}

        player *saved = nullptr;

        bool can_pick(pocket_type pocket, player *target, card *target_card) const override;
        void on_pick(pocket_type pocket, player *target, card *target_card) override;
        game_string status_text(player *owner) const override;
    };

    struct request_lemonade_jim : request_base {
        request_lemonade_jim(card *origin_card, player *origin, player *target)
            : request_base(origin_card, origin, target, effect_flags::auto_respond) {}
        
        game_string status_text(player *owner) const override;
    };

}

#endif