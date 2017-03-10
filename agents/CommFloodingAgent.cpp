//
// Created by raievskc on 11/13/16.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <cerrno>
#include <iosfwd>
#include <sstream>
#include <sys/wait.h>

#include "CommFloodingAgent.h"

CommFloodingAgent::CommFloodingAgent() :
        m_id(-1), m_socketfd(0)
{

}

bool CommFloodingAgent::init(const char *host, const char *port)
{

    bool connectionSucceeded = connectToCore(host, port);

    if (!connectionSucceeded)
    {
        return false;
    }
    const char *idRequest = "id?\n";
    write(m_socketfd, idRequest, strlen(idRequest));

    int nRead = read(m_socketfd, m_inputBuf, BUF_SIZE);

    if (nRead <= 0)
    {  // -1 : error, 0 : just read EOF.
        fprintf(stderr, "Read on core socket failed. Errno : %s\n", strerror(errno));
        return false;
    }

    m_inputBuf[nRead] = '\0';

    int receivedId = atoi(
            m_inputBuf); // Ignores conversion errors to simplify code since the input is sent by the core.

    m_id = receivedId;
    printf("A CommFloodingAgent just received id [%d] from core.\n", m_id);

    m_initializationSucceeded = true;
    return true;
}


void CommFloodingAgent::run()
{

    if (!m_initializationSucceeded)
    { return; }

    std::ostringstream request;
    request << "setPos(" << m_id << "," << m_id << ")\n";

    write(m_socketfd, request.str().c_str(), request.str().size());
    printf("A CommFloodingAgent just sent setpos request with x: [%d].\n", m_id);

    retrieveAgentsInRange(5);

    return;

    // Request ids of all registered agents
    retrieveAgentsList();

//    pid_t childPID = fork();

    // Flood all agents with useless messages
    for (auto a : m_agentList)
    {
        std::ostringstream message;
        message << "Flood from [" << m_id << "] to [" << a << "]" << std::endl;

        for (int i = 0; i < 1e4; i++)
        {
            sendMessage(a, message.str().c_str());
        }
    }


//    if (childPID > 0)
//    {
//        wait(NULL);
//    }

}

bool CommFloodingAgent::retrieveAgentsList()
{
    const char *otherAgentsRequest = "agents?\n";
    write(m_socketfd, otherAgentsRequest, strlen(otherAgentsRequest));

    // Retrieving agent list from core which is of the form : "[1,2,3,4,5]\n"

    int totRead = 0; // Total number of read bytes
    int nRead = -1;
    int maxToRead = 1e6;

    std::stringstream inputSS;

    while (true)
    {
        nRead = read(m_socketfd, m_inputBuf, BUF_SIZE);

        if (nRead == -1)
        {
            if (errno == EINTR)
            {         /* Interrupted --> restart read() */
                continue;
            }
            else
            {
                fprintf(stderr, "Read on core socket failed. Errno : %s\n", strerror(errno));
            }
            return false;              /* Some other error */

        }
        else if (nRead == 0)
        {      /* EOF */
            if (totRead == 0)
            {           /* No bytes read; return 0 */
                return false;
            }
            else
            {                        /* Some bytes read; add '\0' */
                break;
            }

        }

        /* 'nRead' must be >0 if we get here */
        if (totRead + nRead < maxToRead - 1)
        {
            totRead += nRead;
            m_inputBuf[nRead] = '\0';
            inputSS << m_inputBuf;
        }
        else
        {
            fprintf(stderr, "Error while retrieving agents list. Too much to read.\n");
        }

        if (m_inputBuf[nRead - 1] == '\n')
        { // If the last read char is \n, the list of agents is finished
            break;
        }
    }


    // All read input is in inputSS now. Parse it.

    printf("input: [%s]\n", inputSS.str().c_str());

    char discard;
    inputSS >> discard; // discard "["

    int id = -1;
    while (!inputSS.eof())
    {
        inputSS >> id;
        if (inputSS.good())
        {
            m_agentList.push_back(id);
            printf("adding agent [%d]\n", id);
        }
        else
        {
            inputSS.clear(); // clear inputSS's error bits
            inputSS >> discard; // discard ",", "]", and "\n"
//            printf("discarding char [%c]\n", discard);
        }
    }

    printf("Constructed ids : \n");
    for (auto constructedId : m_agentList)
    {
        printf("%d, ", constructedId);
        fflush(stdout);
    }

    return true;
}

bool CommFloodingAgent::connectToCore(const char *node, const char *service)
{

    // Open socket connection with core
    m_socketfd = socket(AF_INET, SOCK_STREAM, 0);

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int retrieveAddressResult, j;

    /* Obtain address(es) matching host/port */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    retrieveAddressResult = getaddrinfo(node, service, &hints, &result);
    if (retrieveAddressResult != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retrieveAddressResult));
        return false;
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket
       and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        m_socketfd = socket(rp->ai_family, rp->ai_socktype,
                            rp->ai_protocol);
        if (m_socketfd == -1)
        {
            continue;
        }

        int connectResult = connect(m_socketfd, rp->ai_addr, rp->ai_addrlen);
        if (connectResult != -1)
        {
            break;                  /* Success */
        }
        else
        {
            fprintf(stderr, "A connection attempt failed. Errno : %s\n", strerror(errno));
        }

        close(m_socketfd);
    }

    if (rp == NULL)
    {               /* No address succeeded */
        fprintf(stderr, "Could not connect\n");
        return false;
    }

    return true;
}

CommFloodingAgent::~CommFloodingAgent()
{

    shutdown(m_socketfd, SHUT_RDWR);
}

void CommFloodingAgent::sendMessage(int receiverId, const char *message)
{

    int nbWrittent = write(m_socketfd, message, strlen(message) + 1);
    if (nbWrittent < strlen(message))
    {
        fprintf(stderr, "CommFloodAgent with id [%d], failed to send message [%s] to agent with id [%d]\n", m_id,
                message,
                receiverId);
    }
    printf(".");
}

void CommFloodingAgent::retrieveAgentsInRange(int range)
{
    std::ostringstream request;
    request << "agentsInRange(" << range << ")\n";
    write(m_socketfd, request.str().c_str(), request.str().size() + 1);
    printf("A CommFloodingAgent just sent agents in range request with id: [%d] and range : [%d].\n", m_id, range);

    printAllRead();
}

void CommFloodingAgent::printAllRead()
{

    printf("A CommFloodingAgent just started waiting for agents in range\n");

    int nRead = 0;
    while (true)
    {
        nRead = read(m_socketfd, m_inputBuf, BUF_SIZE);

        if (nRead == -1)
        {
            if (errno == EINTR)
            {         /* Interrupted --> restart read() */
                continue;
            }
            else
            {
                fprintf(stderr, "Read on core socket failed. Errno : %s\n", strerror(errno));
            }
            break;              /* Some other error */

        }
        else if (nRead == 0)
        {      /* EOF */
            break;
        }

        m_inputBuf[nRead] = '\0';
        printf("%s", m_inputBuf);
    }

    printf("\n");
}


/* Read characters from 'fd' until a newline is encountered. If a newline
  character is not encountered in the first (n - 1) bytes, then the excess
  characters are discarded. The returned string placed in 'buf' is
  null-terminated and includes the newline character if it was read in the
  first (n - 1) bytes. The function return value is the number of bytes
  placed in buffer (which includes the newline character if encountered,
  but excludes the terminating null byte). */

//  ssize_t
//  readLine(int fd, void *buffer, size_t n) {
//      ssize_t numRead;                    /* # of bytes fetched by last read() */
//      size_t totRead;                     /* Total bytes read so far */
//      char *buf;
//      char ch;
//
//      if (n <= 0 || buffer == NULL) {
//          errno = EINVAL;
//          return -1;
//      }
//
//      buf = buffer;                       /* No pointer arithmetic on "void *" */
//
//      totRead = 0;
//      for (;;) {
//          numRead = read(fd, &ch, 1);
//
//          if (numRead == -1) {
//              if (errno == EINTR)         /* Interrupted --> restart read() */
//                  continue;
//              else
//                  return -1;              /* Some other error */
//
//          } else if (numRead == 0) {      /* EOF */
//              if (totRead == 0)           /* No bytes read; return 0 */
//                  return 0;
//              else                        /* Some bytes read; add '\0' */
//                  break;
//
//          } else {                        /* 'nRead' must be 1 if we get here */
//              if (totRead < n - 1) {      /* Discard > (n - 1) bytes */
//                  totRead++;
//                  *buf++ = ch;
//              }
//
//             if (ch == '\n')
//                 break;
//         }
//     }
//
//     *buf = '\0';
//     return totRead;
// }
