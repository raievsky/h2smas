//
// Created by raievskc on 11/17/16.
//

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "AgentRequestHandler.h"

#ifndef HHSMAS_TCPCONNECTION_H
#define HHSMAS_TCPCONNECTION_H

class TCPConnection
        : public boost::enable_shared_from_this<TCPConnection> {
public:
    typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;

    TCPConnection(boost::asio::io_service &service, const int id, AgentRequestHandler &requestHandler);


    boost::asio::ip::tcp::socket &socket();

    void startReading();

    void stop();

    ~TCPConnection();

protected:

    AgentRequestHandler &m_requestHandler;

    void handle_write(const boost::system::error_code &ec,
                      size_t /*bytes_transferred*/);

    boost::asio::ip::tcp::socket m_socket;
    std::string m_message;
    boost::asio::streambuf m_readBuffer;

    int m_id;

    void handleReadLine(boost::system::error_code ec, std::size_t bytes_transferred);

};

#endif //HHSMAS_TCPCONNECTION_H
