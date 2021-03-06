#include "game.h"

#include "holders.h"
#include "game_update.h"

#include "effects/armedanddangerous/requests.h"
#include "effects/base/requests.h"
#include "play_verify.h"

#include <array>

namespace banggame {

    using namespace enums::flag_operators;

    player *game::find_disconnected_player() {
        auto dc_view = std::ranges::filter_view(m_players, [](const player &p) { return p.user_id == 0; });
        if (dc_view.empty()) return nullptr;

        auto first_alive = std::ranges::find_if(dc_view, &player::alive);
        if (first_alive != dc_view.end()) {
            return &*first_alive;
        }
        if (has_expansion(card_expansion_type::ghostcards)) {
            return &dc_view.front();
        } else {
            return nullptr;
        }
    }

    std::vector<game_update> game::get_game_state_updates(player *owner) {
        std::vector<game_update> ret;
#define ADD_TO_RET(name, ...) ret.emplace_back(enums::enum_tag<game_update_type::name> __VA_OPT__(,) __VA_ARGS__)
        
        ADD_TO_RET(add_cards, make_id_vector(m_cards | std::views::transform([](const card &c) { return &c; })), pocket_type::hidden_deck);

        const auto show_never = [](const card &c) { return false; };
        const auto show_always = [](const card &c) { return true; };

        auto move_cards = [&](auto &&range, auto do_show_card) {
            for (card *c : range) {
                ADD_TO_RET(move_card, c->id, c->owner ? c->owner->id : 0, c->pocket, show_card_flags::instant);

                if (do_show_card(*c)) {
                    ADD_TO_RET(show_card, *c, show_card_flags::instant);
                    if (c->num_cubes > 0) {
                        ADD_TO_RET(add_cubes, c->num_cubes, c->id);
                    }

                    if (c->inactive) {
                        ADD_TO_RET(tap_card, c->id, true, true);
                    }
                }
            }
        };

        ADD_TO_RET(game_options, m_options);

        move_cards(m_specials, show_always);
        move_cards(m_deck, show_never);
        move_cards(m_shop_deck, show_never);

        move_cards(m_discards, show_always);
        move_cards(m_selection, [&](const card &c){ return c.owner == owner; });
        move_cards(m_shop_discards, show_always);
        move_cards(m_shop_selection, show_always);
        move_cards(m_hidden_deck, show_always);

        if (!m_scenario_deck.empty()) {
            ADD_TO_RET(move_scenario_deck, m_first_player->id);
        }

        move_cards(m_scenario_deck, [&](const card &c) { return &c == m_scenario_deck.back(); });
        move_cards(m_scenario_cards, [&](const card &c) { return &c == m_scenario_cards.back(); });
        
        if (num_cubes > 0) {
            ADD_TO_RET(add_cubes, num_cubes, 0);
        }

        for (auto &p : m_players) {
            if (p.check_player_flags(player_flags::role_revealed) || &p == owner) {
                ADD_TO_RET(player_show_role, p.id, p.m_role, true);
            }

            if (p.alive() || has_expansion(card_expansion_type::ghostcards)) {
                move_cards(p.m_characters, show_always);
                move_cards(p.m_backup_character, show_never);

                move_cards(p.m_table, show_always);
                move_cards(p.m_hand, [&](const card &c){ return c.owner == owner || c.deck == card_deck_type::character; });

                ADD_TO_RET(player_hp, p.id, p.m_hp, true);
                ADD_TO_RET(player_status, p.id, p.m_player_flags, p.m_range_mod, p.m_weapon_range, p.m_distance_mod);
                
                if (p.m_gold != 0) {
                    ADD_TO_RET(player_gold, p.id, p.m_gold);
                }
            }
        }

        if (m_playing) {
            ADD_TO_RET(switch_turn, m_playing->id);
        }
        if (pending_requests()) {
            ADD_TO_RET(request_status, make_request_update(owner));
        }
        for (const auto &[target, str] : m_saved_log) {
            if (target.matches(owner ? owner->user_id : -1)) {
                ADD_TO_RET(game_log, str);
            }
        }
        if (owner && owner->m_prompt) {
            ADD_TO_RET(game_prompt, owner->m_prompt->second);
        }

#undef ADD_TO_RET
        return ret;
    }

    void game::start_game(const game_options &options) {
        m_options = options;
    
        add_update<game_update_type::game_options>(options);
        
        auto add_cards = [&](const std::vector<card_data> &cards, pocket_type pocket, std::vector<card *> *out_pocket = nullptr) {
            if (!out_pocket) out_pocket = &get_pocket(pocket);

            size_t count = 0;
            for (const auto &c : cards) {
                if (m_players.size() <= 2 && c.has_tag(tag_type::discard_if_two_players)) continue;;
                if ((c.expansion & m_options.expansions) != c.expansion) continue;

                card copy(c);
                copy.id = m_cards.first_available_id();
                copy.owner = nullptr;
                copy.pocket = pocket;
                auto *new_card = &m_cards.emplace(std::move(copy));

                out_pocket->push_back(new_card);
                ++count;
            }
            return count;
        };

        auto move_testing_cards_back = [](auto &deck) {
#ifdef TESTING_CARDS
            std::ranges::partition(deck, [](card *c) {
                return !c->has_tag(tag_type::testing);
            });
#endif
        };

        if (add_cards(all_cards.specials, pocket_type::specials)) {
            add_update<game_update_type::add_cards>(make_id_vector(m_specials), pocket_type::specials);
            for (card *c : m_specials) {
                send_card_update(c, nullptr, show_card_flags::instant);
            }
        }

        if (add_cards(all_cards.deck, pocket_type::main_deck)) {
            shuffle_cards_and_ids(m_deck);
            move_testing_cards_back(m_deck);
            add_update<game_update_type::add_cards>(make_id_vector(m_deck), pocket_type::main_deck);
        }

        if (add_cards(all_cards.goldrush, pocket_type::shop_deck)) {
            shuffle_cards_and_ids(m_shop_deck);
            move_testing_cards_back(m_shop_deck);
            add_update<game_update_type::add_cards>(make_id_vector(m_shop_deck), pocket_type::shop_deck);
        }

        if (has_expansion(card_expansion_type::armedanddangerous)) {
            num_cubes = 32;
            add_update<game_update_type::add_cubes>(num_cubes, 0);
        }

        if (add_cards(all_cards.highnoon, pocket_type::scenario_deck) || add_cards(all_cards.fistfulofcards, pocket_type::scenario_deck)) {
            shuffle_cards_and_ids(m_scenario_deck);
            auto last_scenario_cards = std::ranges::partition(m_scenario_deck, [](card *c) {
                return c->has_tag(tag_type::last_scenario_card);
            });
            if (last_scenario_cards.begin() != m_scenario_deck.begin()) {
                m_scenario_deck.erase(m_scenario_deck.begin() + 1, last_scenario_cards.begin());
            }
            
            move_testing_cards_back(m_scenario_deck);
            if (m_scenario_deck.size() > m_options.scenario_deck_size) {
                m_scenario_deck.erase(m_scenario_deck.begin() + 1, m_scenario_deck.end() - m_options.scenario_deck_size);
            }

            add_update<game_update_type::add_cards>(make_id_vector(m_scenario_deck), pocket_type::scenario_deck);
        }

        if (add_cards(all_cards.hidden, pocket_type::hidden_deck)) {
            add_update<game_update_type::add_cards>(make_id_vector(m_hidden_deck), pocket_type::hidden_deck);
        }
        
        std::array roles {
            player_role::sheriff,
            player_role::outlaw,
            player_role::outlaw,
            player_role::renegade,
            player_role::deputy,
            player_role::outlaw,
            player_role::deputy,
            player_role::renegade
        };

        std::array roles_3players {
            player_role::deputy_3p,
            player_role::outlaw_3p,
            player_role::renegade_3p
        };

        auto role_it = m_players.size() > 3 ? roles.begin() : roles_3players.begin();

        std::ranges::shuffle(role_it, role_it + m_players.size(), rng);
        for (auto &p : m_players) {
            p.set_role(*role_it++);
        }

        if (m_players.size() > 3) {
            m_first_player = &*std::ranges::find(m_players, player_role::sheriff, &player::m_role);
        } else {
            m_first_player = &*std::ranges::find(m_players, player_role::deputy_3p, &player::m_role);
        }

        std::vector<card *> character_ptrs;
        if (add_cards(all_cards.characters, pocket_type::none, &character_ptrs)) {
            std::ranges::shuffle(character_ptrs, rng);
            move_testing_cards_back(character_ptrs);
        }

        auto add_character_to = [&](card *c, player &p) {
            p.m_characters.push_back(c);
            c->pocket = pocket_type::player_character;
            c->owner = &p;
            add_update<game_update_type::add_cards>(make_id_vector(std::views::single(c)), pocket_type::player_character, p.id);
        };

        auto character_it = character_ptrs.rbegin();

#ifdef TESTING_CARDS
        for (auto &p : m_players) {
            add_character_to(*character_it++, p);
        }
        for (auto &p : m_players) {
            add_character_to(*character_it++, p);
        }
#else
        for (auto &p : m_players) {
            add_character_to(*character_it++, p);
            add_character_to(*character_it++, p);
        }
#endif

        if (has_expansion(card_expansion_type::characterchoice)) {
            for (auto &p : m_players) {
                while (!p.m_characters.empty()) {
                    move_card(p.m_characters.front(), pocket_type::player_hand, &p, show_card_flags::instant | show_card_flags::shown);
                }
            }
            for (player &p : range_all_players(m_first_player)) {
                queue_request<request_characterchoice>(&p);
            }
        } else {
            for (auto &p : m_players) {
                add_log("LOG_CHARACTER_CHOICE", &p, p.m_characters.front());
                send_card_update(p.m_characters.front(), &p, show_card_flags::instant | show_card_flags::shown);
                p.reset_max_hp();
                p.set_hp(p.m_max_hp, true);
                p.m_characters.front()->on_enable(&p);

                move_card(p.m_characters.back(), pocket_type::player_backup, &p, show_card_flags::instant | show_card_flags::hidden);
            }
        }

        queue_action([this] {
            add_log("LOG_GAME_START");

            for (auto &p : m_players) {
                p.m_characters.front()->on_equip(&p);
            }

            for (player &p : range_all_players(m_first_player,
                std::ranges::max(m_players | std::views::transform(&player::get_initial_cards))))
            {
                if (p.m_hand.size() < p.get_initial_cards()) {
                    p.draw_card();
                }
            }

            if (!m_shop_deck.empty()) {
                for (int i=0; i<3; ++i) {
                    draw_shop_card();
                }
            }

            if (!m_scenario_deck.empty()) {
                add_update<game_update_type::move_scenario_deck>(m_first_player->id);
                send_card_update(m_scenario_deck.back(), nullptr, show_card_flags::instant);
            }

            m_playing = m_first_player;
            m_first_player->start_of_turn();
        });
    }

    request_status_args game::make_request_update(player *p) {
        const auto &req = top_request();
        request_status_args ret{
            req.origin() ? req.origin()->id : 0,
            req.target() ? req.target()->id : 0,
            req.status_text(p),
            req.flags()
        };

        if (!p) return ret;

        auto add_ids_for = [&](auto &&cards) {
            for (card *c : cards) {
                if (req.can_respond(p, c)) {
                    ret.respond_ids.push_back(c->id);
                }
            }
        };

        add_ids_for(p->m_hand | std::views::filter([](card *c) { return c->color == card_color_type::brown; }));
        add_ids_for(p->m_table | std::views::filter(std::not_fn(&card::inactive)));
        add_ids_for(p->m_characters);
        add_ids_for(m_scenario_cards | std::views::reverse | std::views::take(1));
        add_ids_for(m_specials);
        
        if (bool(req.flags() & effect_flags::force_play)) {
            add_ids_for(m_shop_selection);
        }

        if (req.target() != p) return ret;

        auto maybe_add_pick_id = [&](pocket_type pocket, player *target_player, card *target_card) {
            if (req.can_pick(pocket, target_player, target_card)) {
                ret.pick_ids.emplace_back(pocket,
                    target_player ? target_player->id : 0,
                    target_card ? target_card->id : 0);
            }
        };

        for (player &target : m_players) {
            std::ranges::for_each(target.m_hand, std::bind_front(maybe_add_pick_id, pocket_type::player_hand, &target));
            std::ranges::for_each(target.m_table, std::bind_front(maybe_add_pick_id, pocket_type::player_table, &target));
            std::ranges::for_each(target.m_characters, std::bind_front(maybe_add_pick_id, pocket_type::player_character, &target));
        }
        maybe_add_pick_id(pocket_type::main_deck, nullptr, nullptr);
        maybe_add_pick_id(pocket_type::discard_pile, nullptr, nullptr);
        std::ranges::for_each(m_selection, std::bind_front(maybe_add_pick_id, pocket_type::selection, nullptr));

        return ret;
    }

    void game::send_request_status_clear() {
        add_update<game_update_type::status_clear>();
    }

    void game::send_request_update() {
        auto &req = top_request();
        if (req.target() && bool(req.flags() & (effect_flags::auto_pick | effect_flags::auto_respond))) {
            auto target_request_update = make_request_update(req.target());
            if (bool(req.flags() & effect_flags::auto_pick) && target_request_update.pick_ids.size() == 1 && target_request_update.respond_ids.empty()) {
                const auto &args = target_request_update.pick_ids.front();
                req.on_pick(args.pocket,
                    args.player_id ? find_player(args.player_id) : nullptr,
                    args.card_id ? find_card(args.card_id) : nullptr);
                return;
            }
            if (bool(req.flags() & effect_flags::auto_respond) && target_request_update.pick_ids.empty() && target_request_update.respond_ids.size() == 1) {
                player *target = req.target();
                card *card_ptr = find_card(target_request_update.respond_ids.front());
                bool is_response = !bool(req.flags() & effect_flags::force_play);
                auto &effects = is_response ? card_ptr->responses : card_ptr->effects;
                if (card_ptr->modifier == card_modifier_type::none && std::ranges::all_of(effects, [](const effect_holder &holder) {
                    return holder.target == target_type::none;
                })) {
                    if (!is_response) {
                        pop_request();
                    }
                    play_card_verify{target, card_ptr, is_response,
                        target_list{effects.size(), play_card_target{enums::enum_tag<target_type::none>}}}.do_play_card();
                    return;
                }
            }
        }

        auto spectator_target = update_target::excludes_public();
        for (auto &p : m_players) {
            spectator_target.add(&p);
            add_update<game_update_type::request_status>(update_target::includes_private(&p), make_request_update(&p));
        }
        add_update<game_update_type::request_status>(std::move(spectator_target), make_request_update(nullptr));
    }
    
    void game::draw_check_then(player *origin, card *origin_card, draw_check_function fun) {
        m_current_check.set(origin, origin_card, std::move(fun));
        m_current_check.start();
    }

    void game::start_next_turn() {
        auto it = m_players.find(m_playing->id);
        do {
            if (check_flags(game_flags::invert_rotation)) {
                if (it == m_players.begin()) it = m_players.end();
                --it;
            } else {
                ++it;
                if (it == m_players.end()) it = m_players.begin();
            }
            call_event<event_type::verify_revivers>(&*it);
        } while (!it->alive());

        player *next_player = &*it;
        
        if (next_player == m_first_player) {
            queue_action_front([this]{
                draw_scenario_card();
            });
        }

        queue_action_front([next_player]{
            next_player->start_of_turn();
        });
    }

    void game::handle_player_death(player *killer, player *target, bool no_handle_game_over) {
        if (killer != m_playing) killer = nullptr;
        
        for (card *c : target->m_characters) {
            target->disable_equip(c);
        }

        if (target->m_characters.size() > 1) {
            add_update<game_update_type::remove_cards>(make_id_vector(target->m_characters | std::views::drop(1)));
            target->m_characters.resize(1);
        }

        if (!m_first_dead) m_first_dead = target;

        if (killer && killer != target) {
            add_log("LOG_PLAYER_KILLED", killer, target);
        } else {
            add_log("LOG_PLAYER_DIED", target);
        }
        
        add_update<game_update_type::player_show_role>(target->id, target->m_role);
        target->add_player_flags(player_flags::role_revealed);

        call_event<event_type::on_player_death>(killer, target);
        
        queue_action([target]{
            target->discard_all(true);
        });

        if (no_handle_game_over) {
            return;
        }

        queue_action([this, killer, target] {
            if (check_flags(game_flags::game_over)) return;

            target->add_player_flags(player_flags::dead);
            target->set_hp(0, true);

            player_role winner_role = player_role::unknown;

            auto alive_players_view = m_players | std::views::filter(&player::alive);
            int num_alive = std::ranges::distance(alive_players_view);

            if (num_alive == 0) {
                winner_role = player_role::outlaw;
            } else if (num_alive == 1 || std::ranges::all_of(alive_players_view, [](player_role role) {
                return role == player_role::sheriff || role == player_role::deputy;
            }, &player::m_role)) {
                winner_role = alive_players_view.front().m_role;
            } else if (m_players.size() > 3) {
                if (target->m_role == player_role::sheriff) {
                    winner_role = player_role::outlaw;
                }
            } else if (killer) {
                if (target->m_role == player_role::outlaw_3p && killer->m_role == player_role::renegade_3p) {
                    winner_role = player_role::renegade_3p;
                } else if (target->m_role == player_role::renegade_3p && killer->m_role == player_role::deputy_3p) {
                    winner_role = player_role::deputy_3p;
                } else if (target->m_role == player_role::deputy_3p && killer->m_role == player_role::outlaw_3p) {
                    winner_role = player_role::outlaw_3p;
                }
            }

            if (winner_role != player_role::unknown) {
                for (const auto &p : m_players) {
                    if (!p.check_player_flags(player_flags::role_revealed)) {
                        add_update<game_update_type::player_show_role>(p.id, p.m_role);
                    }
                }
                add_log("LOG_GAME_OVER");
                set_game_flags(game_flags::game_over);
                add_update<game_update_type::game_over>(winner_role);
            } else if (m_playing == target) {
                start_next_turn();
            } else if (killer) {
                if (m_players.size() > 3) {
                    switch (target->m_role) {
                    case player_role::outlaw:
                        killer->draw_card(3);
                        break;
                    case player_role::deputy:
                        if (killer->m_role == player_role::sheriff) {
                            queue_action([this, killer]{
                                add_log("LOG_SHERIFF_KILLED_DEPUTY", killer);
                                killer->discard_all(false);
                            });
                        }
                        break;
                    }
                } else {
                    killer->draw_card(3);
                }
            }
            
            if (target == m_first_player && num_alive > 1) {
                m_first_player = std::next(player_iterator(m_first_player));
                add_update<game_update_type::move_scenario_deck>(m_first_player->id);
            }

            if (!has_expansion(card_expansion_type::ghostcards)) {
                add_update<game_update_type::player_remove>(target->id);
            }
        });
    }

}