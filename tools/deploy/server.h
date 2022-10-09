//
// Created by hzl on 2019/5/8.
//

#pragma once

#include <string>
#include <cstdint>
#include "asio/ip/tcp.hpp"
#include "asio/io_context.hpp"
#include "asio/signal_set.hpp"
#include "connection_manager.h"

class server
{
public:
    server(const server&) = delete;
    server& operator=(const server&) = delete;

    explicit server(std::string_view address, std::string_view port);

    void run();

private:
    /// Perform an asynchronous accept operation.
    void do_accept();

    /// Wait for a request to stop the server.
    void do_await_stop();

private:
    /// The io_context used to perform asynchronous operations.
    asio::io_context io_context_;

    /// The signal_set is used to register for process termination notifications.
    asio::signal_set signals_;

    /// Acceptor used to listen for incoming connections.
    asio::ip::tcp::acceptor acceptor_;

    /// The connection manager which owns all live connections.
    connection_manager connection_manager_;
};

