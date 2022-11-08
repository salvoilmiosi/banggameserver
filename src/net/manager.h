#ifndef __MANAGER_H__
#define __MANAGER_H__

#include "lobby.h"
#include "chat_commands.h"

namespace banggame {

template<server_message_type E>
server_message make_message(auto && ... args) {
    return server_message{enums::enum_tag<E>, FWD(args) ...};
}

using send_message_function = std::function<void(client_handle, server_message)>;
using print_error_function = std::function<void(const std::string &message)>;
using kick_client_function = std::function<void(client_handle, const std::string &message)>;

class game_manager {
public:
    void set_send_message_function(send_message_function &&fun) {
        m_send_message = std::move(fun);
    }

    void set_print_error_function(print_error_function &&fun) {
        m_print_error = std::move(fun);
    }

    void set_kick_client_function(kick_client_function &&fun) {
        m_kick_client = std::move(fun);
    }

    void on_receive_message(client_handle client, const client_message &msg);

    void client_disconnected(client_handle client);
    
    bool client_validated(client_handle client) const;

    void tick();

private:
    lobby_data make_lobby_data(lobby_ptr it);
    void send_lobby_update(lobby_ptr it);

    template<server_message_type E>
    void send_message(client_handle client, auto && ... args) {
        m_send_message(client, make_message<E>(FWD(args) ... ));
    }

    template<server_message_type E>
    void broadcast_message(auto && ... args) {
        auto msg = make_message<E>(FWD(args) ... );
        for (client_handle client : users | std::views::keys) {
            m_send_message(client, msg);
        }
    }

    template<server_message_type E>
    void broadcast_message_lobby(const lobby &lobby, auto && ... args) {
        auto msg = make_message<E>(FWD(args) ... );
        for (user_ptr it : lobby.users) {
            m_send_message(it->first, msg);
        }
    }

    void print_error(const std::string &message) {
        if (m_print_error) {
            m_print_error(message);
        }
    }

    void kick_client(client_handle client, const std::string &message) {
        if (m_kick_client) {
            m_kick_client(client, message);
        }
    }

    void kick_user_from_lobby(user_ptr user);

    std::string handle_message(MSG_TAG(connect),        client_handle client, const connect_args &value);
    std::string handle_message(MSG_TAG(lobby_list),     user_ptr user);
    std::string handle_message(MSG_TAG(lobby_make),     user_ptr user, const lobby_info &value);
    std::string handle_message(MSG_TAG(lobby_edit),     user_ptr user, const lobby_info &args);
    std::string handle_message(MSG_TAG(lobby_join),     user_ptr user, const lobby_id_args &value);
    std::string handle_message(MSG_TAG(lobby_rejoin),   user_ptr user, const lobby_rejoin_args &value);
    std::string handle_message(MSG_TAG(lobby_leave),    user_ptr user);
    std::string handle_message(MSG_TAG(lobby_chat),     user_ptr user, const lobby_chat_client_args &value);
    std::string handle_message(MSG_TAG(lobby_return),   user_ptr user);
    std::string handle_message(MSG_TAG(game_start),     user_ptr user);
    std::string handle_message(MSG_TAG(game_action),    user_ptr user, const Json::Value &value);

    std::string handle_chat_command(user_ptr user, const std::string &command);

    std::string command_print_help(user_ptr user);
    std::string command_print_users(user_ptr user);
    std::string command_kick_user(user_ptr user, std::string_view userid);

private:
    user_map users;
    lobby_map m_lobbies;

    int m_lobby_counter = 0;
    int m_user_counter = 0;
    
    send_message_function m_send_message;
    print_error_function m_print_error;
    kick_client_function m_kick_client;

    friend struct lobby;
    friend class chat_command;
};

}

#endif