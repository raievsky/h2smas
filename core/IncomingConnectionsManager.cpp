//
// Created by raievskc on 11/17/16.
//
// Most of the code here has been borrowed from :
// http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/tutorial/tutdaytime3/src.html


#include <ctime>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "IncomingConnectionsManager.h"

using boost::asio::ip::tcp;

IncomingConnectionsManager::IncomingConnectionsManager(boost::asio::io_service &io_service,
                                                       AgentRequestHandler& requestHandler)
        : m_acceptor(io_service, tcp::endpoint(tcp::v4(), 7777)), m_signals(io_service),
          m_agentRequestHandler(requestHandler)
{
//    asyncWaitStop();

    m_connections.reserve(100);

    StartAccept();
}

void IncomingConnectionsManager::StartAccept()
{

    const int nextId = m_connections.size();
    TCPConnection::TCPConnectionPtr newConnection = std::make_shared<TCPConnection>(m_acceptor.get_io_service(),
                                                                                    nextId, m_agentRequestHandler);

    m_connections.push_back(std::move(newConnection));
    m_acceptor.async_accept(m_connections[nextId]->socket(),
                            boost::bind(&IncomingConnectionsManager::handleAccept, this, nextId,
                                        boost::asio::placeholders::error));
}

void IncomingConnectionsManager::handleAccept(int connectionId,
                                              const boost::system::error_code &error)
{

    std::cout << "Handling an asynchronous accept. Id : " << connectionId << std::endl;

    if (!error)
    {
        m_connections[connectionId]->startReading();
    }

    // Communication with the agent will be handled at the connection level, start waiting for new connections
    StartAccept();
}

void IncomingConnectionsManager::asyncWaitStop()
{
    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
    // signals_.add(SIGQUIT);

    m_signals.async_wait(
            [this](boost::system::error_code /*ec*/, int /*signo*/)
            {
                // The server is stopped by cancelling all outstanding asynchronous
                // operations. Once all operations have finished the io_service::run()
                // call will exit.
                m_acceptor.close();
                closeAllConnections();
            });
}

IncomingConnectionsManager::~IncomingConnectionsManager()
{
    std::cout << "Destroying IncomingConnectionManager." << std::endl;
}

void IncomingConnectionsManager::closeAllConnections()
{
    for (auto c : m_connections)
    {
        c->stop();
    }
}

