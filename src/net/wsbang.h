#ifndef __WSBANG_H__
#define __WSBANG_H__

#include "wsserver.h"
#include "wsconnection.h"

#include "game/manager.h"
#include "game/net_options.h"

#include <iostream>

namespace banggame {

    struct bang_server : net::wsserver<bang_server, client_message, server_message> {
        using base = net::wsserver<bang_server, client_message, server_message>;
        using client_handle = typename base::client_handle;

        game_manager m_mgr;
        std::jthread m_game_thread;

        bang_server(asio::io_context &ctx) : base(ctx) {
            m_mgr.set_send_message_function([&](client_handle con, banggame::server_message msg) {
                this->push_message(con, std::move(msg));
            });

            m_mgr.set_print_error_function([&](const std::string &msg) {
                std::cerr << msg << '\n';
            });
        }

        void tick() {
            m_mgr.tick();
        }

        void on_disconnect(client_handle con) {
            m_mgr.client_disconnected(con);
        }

        void on_message(client_handle con, const client_message &msg) {
            m_mgr.on_receive_message(con, msg);
        }
    };

}

#endif