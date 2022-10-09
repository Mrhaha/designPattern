//
// Created by hzl on 2019/5/8.
//
#pragma once

#include <memory>
#include "asio/ip/tcp.hpp"

class connection_manager;

/// Represents a single connection from a client.
class connection : public std::enable_shared_from_this<connection>
{
public:
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    /// Construct a connection with the given socket.
    explicit connection(asio::ip::tcp::socket socket, connection_manager& manager);

    /// Start the first asynchronous operation for the connection.
    void start();

    /// Stop all asynchronous operations associated with the connection.
    void stop();

private:
    /// Perform an asynchronous read operation.
    void do_read();

    /// process an asynchronous message.
    void process();

private:
    void process_one_code(const char* buffer, size_t length);

private:
    /// Socket for the connection.
    asio::ip::tcp::socket socket_;

    /// The manager for this connection.
    connection_manager& connection_manager_;

    /// Buffer for incoming data.
    std::size_t recv_head_;
    std::size_t recv_tail_;
    char recv_buffer_[8096]; //特别注意：不是循环缓冲区
};

using connection_ptr = std::shared_ptr<connection>;

