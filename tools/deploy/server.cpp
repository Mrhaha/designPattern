//
// Created by hzl on 2019/5/8.
//

#include <iostream>
#include "server.h"

server::server(std::string_view address, std::string_view port)
    : io_context_(),
    signals_(io_context_),
    acceptor_(io_context_),
    connection_manager_()
{
    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
#if defined(SIGQUIT)
    signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

    do_await_stop();

    asio::ip::tcp::resolver resolver(io_context_);
    asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    do_accept();
}

void server::run()
{
    io_context_.run();
}

void server::do_accept()
{
    acceptor_.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket){
        if (acceptor_.is_open())
        {
            if (!ec)
            {
                connection_manager_.start(std::make_shared<connection>(std::move(socket), connection_manager_));
            }

            do_accept();
        }
    });
}

void server::do_await_stop()
{
    signals_.async_wait(
            [this](std::error_code ec, int32_t signo)
            {
                acceptor_.close();
                connection_manager_.stop_all();
            }
            );
}
