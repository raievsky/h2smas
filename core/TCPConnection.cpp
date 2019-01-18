//
// Created by raievskc on 11/17/16.
//

#include "TCPConnection.h"

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

using boost::asio::ip::tcp;

TCPConnection::TCPConnection(boost::asio::io_service& io_service, const int id, AgentRequestHandler& requestHandler)
        : m_socket(io_service), m_id(id), m_requestHandler(requestHandler)
{

}

tcp::socket& TCPConnection::socket()
{
    return m_socket;
}

void TCPConnection::startReading()
{
    // Register "handleReadLine" as the callback associated with a read on our socket
    // with a '\n' in the received data.
    boost::asio::async_read_until(m_socket, m_readBuffer, '\n',
                                  boost::bind(&TCPConnection::handleReadLine, this, _1, _2));
    std::cout << "Async read started." << std::endl;
}

void TCPConnection::handle_write(const boost::system::error_code& ec, size_t /*bytes_transferred*/)
{
    if (ec)
    {
        std::cerr << "Error while trying to send message." << std::endl;
    }
}

void TCPConnection::handleReadLine(boost::system::error_code ec, std::size_t bytes_transferred)
{

    // This callback is called when a async_read_until complete,
    // that is to say when a '\n' char has been found in the read data.
    // It is possible that extra chars are in the input buffer after the '\n' char.
    // We should be carefull not to extract them.

    // std::cout << "Number of bytes read : [" << bytes_transferred << "]" << std::endl;

    if (!ec)
    {
        std::string request;
        std::istream inputStream(&m_readBuffer);
        std::getline(inputStream, request); // Discard the '\n' from inputStream
        while (*request.begin() == '\0')
        {

            std::cout << "removing trailing '0' from request" << std::endl;
            request = request.substr(1);
        }
        boost::trim_left_if(request, boost::is_any_of(" \0"));
//        std::cout << "Read line : {" << request << "}" << std::endl;
        if (request == "id?")
        {
            std::cout << "Agent id request received." << std::endl;

            std::string answer;
            // TODO check if this call succeeded.
            m_requestHandler.handleRequest(m_id, AgentRequestHandler::ADD_AGT_ID_RANGE_REQUEST_STRING, answer);

            std::ostringstream os;
            os << m_id;
            m_message = os.str();

            boost::asio::async_write(m_socket, boost::asio::buffer(m_message),
                                     boost::bind(&TCPConnection::handle_write, this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred));
            std::cout << "Sending agent id scheduled." << std::endl;
        }
        //else if (request == "agents?")
        //{
        //    std::cout << "Agent list request received." << std::endl;
        //    m_message = "[1,2,3,4,5]\n";
        //    boost::asio::async_write(m_socket, boost::asio::buffer(m_message),
        //                             boost::bind(&TCPConnection::handle_write, this,
        //                                         boost::asio::placeholders::error,
        //                                         boost::asio::placeholders::bytes_transferred));
        //    std::cout << "Sending of current agent list scheduled." << std::endl;
        //}
        else
        {

            // TODO : put in place the request handler
            std::cout << "Received : {" << request << "}" << std::endl;
            std::cout << "forwarding it to the request handler." << std::endl;
            std::string answer;
            if (m_requestHandler.handleRequest(m_id, request, answer))
            {
                std::cout << "Request handler indicated that there is an answer to forward." << std::endl;
                std::cout << "Forwarding: [" << answer << "]" << std::endl;
                boost::asio::async_write(m_socket, boost::asio::buffer(answer),
                                         boost::bind(&TCPConnection::handle_write, this,
                                                     boost::asio::placeholders::error,
                                                     boost::asio::placeholders::bytes_transferred));
            }
            else
            {
                std::cout << "Request handler indicated that there is NO answer to forward." << std::endl;
            }

//            startReading();

        }
    }
    else
    {
        std::cerr << "Error while trying to handle a received message." << std::endl;
    }

    if (bytes_transferred > 0)
    {
        startReading();
    }
}

TCPConnection::~TCPConnection()
{
    std::cout << "Destroying TCPConnection" << std::endl;
}

void TCPConnection::stop()
{
    m_socket.close();
}

