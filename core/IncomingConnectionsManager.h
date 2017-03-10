//
// Created by raievskc on 11/17/16.
//

#include <boost/asio.hpp>
#include "TCPConnection.h"

#ifndef HHSMAS_INCOMINGCONNECTIONSMANAGER_H
#define HHSMAS_INCOMINGCONNECTIONSMANAGER_H

class IncomingConnectionsManager {

public:
    IncomingConnectionsManager(boost::asio::io_service &io_service, AgentRequestHandler requestHandler);

    ~IncomingConnectionsManager();

protected:
    void StartAccept();

    void handleAccept(int connectionId,
                      const boost::system::error_code &error);


    // Ensure the IncomingConnectionManager wait for a signal to disapear
    void asyncWaitStop();

    void closeAllConnections();

    boost::asio::ip::tcp::acceptor m_acceptor;
    std::vector<TCPConnection::TCPConnectionPtr> m_connections;
    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set m_signals;

    AgentRequestHandler &m_agentRequestHandler;
};

#endif //HHSMAS_INCOMINGCONNECTIONSMANAGER_H
