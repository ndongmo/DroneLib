/*!
* \file UDPSocket.h
* \brief UDP Socket data structure.
* \author Ndongmo Silatsa Fabrice
* \date 26-02-2019
* \version 1.0
*/

#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

class UDPSocket
{
public:
	UDPSocket(void);
	~UDPSocket(void);
	/*!
	* \brief Opens the listening socket.
	* \param addr : client address
	* \param port : client port number
	* \return -1 when an error occured, otherwise 1.
	*/
	int open(char *addr, int port);
	/*!
	* \brief Sends data to the client address.
	* \param buf : data buffer
	* \param size : buffer size
	* \return -1 when an error occured, otherwise the lenght of sent data
	*/
	int send(const char *buf, int size);
	/*!
	* \brief Receives data from the client address.
	* \param buf : data buffer
	* \param size : buffer size
	* \return -1 when an error occured, otherwise the lenght of received data
	*/
	int receive(char *buf, int size);

private:
	WSADATA m_wsaData;	/* Data receiver of the Windows Sockets */
	SOCKET m_sock;		/* UDP Sockets */
	struct sockaddr_in m_server_addr;	/* Server address */
	struct sockaddr_in m_client_addr;	/* Client address */
};

