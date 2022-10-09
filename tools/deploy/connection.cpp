//
// Created by hzl on 2019/5/8.
//

#include <iostream>
#include "connection.h"
#include "connection_manager.h"

connection::connection(asio::ip::tcp::socket socket, connection_manager &manager)
    : socket_(std::move(socket)),
    connection_manager_(manager),
    recv_head_(0),
    recv_tail_(0),
    recv_buffer_()
{

}

void connection::start()
{
    do_read();
}

void connection::stop()
{
    socket_.close();
}

void connection::do_read()
{
    auto self(shared_from_this());
    static std::array<char, 4096> buffer;
    socket_.async_read_some(asio::buffer(buffer),
            [this, self](std::error_code ec, std::size_t bytes_transferred)
            {
                if (!ec)
                {
                    memcpy(recv_buffer_ + recv_tail_, buffer.data(), bytes_transferred);
                    recv_tail_ += bytes_transferred;
                    assert(recv_tail_ <= sizeof(recv_buffer_));

                    process();  //循环处理消息

                    memmove(recv_buffer_, recv_buffer_ + recv_head_, recv_tail_ - recv_head_);
                    recv_tail_ -= recv_head_;
                    recv_head_ = 0;

                    do_read();
                }
                else if (ec != asio::error::operation_aborted)
                {
                    connection_manager_.stop(shared_from_this());
                }
            }
            );
}

void connection::process()
{
    while (recv_tail_ - recv_head_ >= 4)
    {
      /*  size_t package_size = ntohl(*((uint32_t*)(&recv_buffer_[recv_head_])));
        if (recv_tail_ - recv_head_ < package_size)
        {
            break;
        }*/

        size_t package_size = 4;

        process_one_code(recv_buffer_ + recv_head_, package_size);

        recv_head_ += package_size;



    }


}

void connection::process_one_code(const char* buffer, size_t length)
{
    std::cout << "process one code :" << std::string(buffer, length) << std::endl;
}




