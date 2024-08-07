#ifndef __LOBBY_H__
#define __LOBBY_H__

#include "options.h"
#include "messages.h"

#include "game/game.h"

namespace banggame {

class game_manager;
struct client_state;
struct game_user;
struct lobby;

using client_handle = std::weak_ptr<void>;

struct lobby_error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct critical_error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

enum class lobby_team {
    game_player,
    game_spectator,
};

static constexpr ticks lobby_lifetime = 5min;
static constexpr ticks user_lifetime = 10s;

static constexpr ticks client_accept_timer = 5s;
static constexpr ticks ping_interval = 10s;
static constexpr auto pings_until_disconnect = 2min / ping_interval;

struct client_state {
    client_state(client_handle client) : client{client} {}
    
    client_handle client;
    game_user *user = nullptr;
    ticks ping_timer = ticks{0};
    int ping_count = 0;
};

struct game_user: user_info {
    game_user(const user_info &info, id_type session_id)
        : user_info{info}, session_id{session_id} {}
    
    id_type session_id = 0;
    lobby *in_lobby = nullptr;

    client_handle client;
    ticks lifetime = user_lifetime;

    std::chrono::milliseconds get_disconnect_lifetime() const {
        if (client.expired()) {
            return std::chrono::duration_cast<std::chrono::milliseconds>(lifetime);
        }
        return {};
    }
};

struct lobby_user {
    lobby_team team;
    int user_id;
    game_user *user;
};

struct lobby_bot: user_info {
    lobby_bot(const user_info &info, int user_id)
        : user_info{info}, user_id{user_id} {}

    int user_id;
};

struct lobby : lobby_info {
    lobby(const lobby_info &info, id_type lobby_id)
        : lobby_info{info}, lobby_id{lobby_id} {}

    id_type lobby_id;
    int user_id_count = 0;

    std::vector<lobby_user> users;
    std::vector<lobby_bot> bots;
    std::vector<lobby_chat_args> chat_messages;
    
    lobby_state state;
    ticks lifetime = lobby_lifetime;

    std::unique_ptr<banggame::game> m_game;

    lobby_user &add_user(game_user &user) {
        if (auto it = rn::find(users, &user, &lobby_user::user); it != users.end()) {
            return *it;
        } else {
            user.in_lobby = this;
            return users.emplace_back(lobby_team::game_player, ++user_id_count, &user);
        }
    }

    int get_user_id(const game_user &user) const {
        if (auto it = rn::find(users, &user, &lobby_user::user); it != users.end()) {
            return it->user_id;
        }
        return 0;
    }

    explicit operator lobby_data() const {
        return {
            .lobby_id = lobby_id,
            .name = name,
            .num_players = int(rn::count(users, lobby_team::game_player, &lobby_user::team)),
            .num_spectators = int(rn::count(users, lobby_team::game_spectator, &lobby_user::team)),
            .max_players = lobby_max_players,
            .state = state
        };
    }
};

using user_map = std::unordered_map<id_type, game_user>;
using client_map = std::map<client_handle, client_state, std::owner_less<>>;
using lobby_map = std::unordered_map<id_type, lobby>;
using lobby_list = std::vector<lobby_map::iterator>;

}

#endif