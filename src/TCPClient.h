/*!
* \file TCPClient.h
* \brief TCP Client data structure.
* \author Ndongmo Silatsa Fabrice
* \date 26-02-2019
* \version 1.0
*/
#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

class TCPClient
{
public:
	TCPClient(void);
	~TCPClient(void);
	/*!
	* \brief Opens the given address in TCP mode.
	* \param addr : client address
	* \param port : client port number
	* \return -1 when the connection failed, otherwise 1.
	*/
	int open(char *addr, int port);
	/*!
	* \brief Sends data to the server.
	* \param buf : sending buffer
	* \param size : buffer size
	* \return -1 when an error occured, otherwise the lenght of sent data
	*/
	int send(const char *buf, int size);
	/*!
	* \brief Receives data from the server.
	* \param buf : destination buffer
	* \param size : destination buffer size
	* \return -1 when an error occured, otherwise the lenght of received data
	*/
	int receive(char *buf, int size);

private:
	WSADATA m_wsaData;	// Data buffer
	SOCKET m_sock;		// Server socket
	struct sockaddr_in m_server; // IP address, and port of the server to be connected to.
};

