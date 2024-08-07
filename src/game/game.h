#ifndef __GAME_H__
#define __GAME_H__

#include "game_table.h"
#include "request_queue.h"
#include "player_iterator.h"
#include "utils/generator.h"

namespace banggame {

    struct game : game_table, request_queue {
        game(unsigned int seed = 0);
        
        utils::generator<json::json> get_spectator_join_updates();
        utils::generator<json::json> get_game_log_updates(player_ptr target);
        utils::generator<json::json> get_rejoin_updates(player_ptr target);

        card_sign get_card_sign(card_ptr c);

        card_ptr add_card(const card_data &data);
        void add_players(std::span<int> user_ids);
        void start_game(const game_options &options);

        player_distances make_player_distances(player_ptr p);
        request_status_args make_request_update(player_ptr p);
        status_ready_args make_status_ready_update(player_ptr p);
        player_order_update make_player_order_update(bool instant = false);

        ticks get_total_update_time() const;

        void send_request_status_clear();
        bool send_request_status_ready();
        void send_request_update();

        void start_next_turn();

        bool request_bot_play();

        void handle_player_death(player_ptr killer, player_ptr target, discard_all_reason reason);
    };

}

#endif