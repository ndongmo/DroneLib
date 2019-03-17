#include "UDPSocket.h"
#include "Logger.h"

#pragma warning(disable: 4996)

UDPSocket::UDPSocket(void)
{
	int err;
	err = WSAStartup(MAKEWORD(2, 0), &m_wsaData); // initiates use of the Winsock DLL by our process.
	if (err != 0) {
		logE << "udp client";
		switch (err) {
		case WSASYSNOTREADY:
			logE << "WSASYSNOTREADY" << std::endl;
			break;
		case WSAVERNOTSUPPORTED:
			logE << "WSAVERNOTSUPPORTED" << std::endl;
			break;
		case WSAEINPROGRESS:
			logE << "WSAEINPROGRESS" << std::endl;
			break;
		case WSAEPROCLIM:
			logE << "WSAEPROCLIM" << std::endl;
			break;
		case WSAEFAULT:
			logE << "WSAEFAULT" << std::endl;
			break;
		}
	}
	m_sock = INVALID_SOCKET;
}


UDPSocket::~UDPSocket(void)
{
	if (m_sock != INVALID_SOCKET){
		closesocket(m_sock);
	}

	WSACleanup(); // terminates use of the Winsock 2 DLL
}

//open the listening socket
int UDPSocket::open(char *addr, int port){
	int err;
	/* 
	 * AF_INET (IPv4), SOCK_DGRAM (datagram), the 0 value we do not wish to specify a protocol 
	 * and the service provider will choose the protocol to use.
	*/
	m_sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (m_sock == INVALID_SOCKET){
		logE << "create socket failed" << std::endl;
		return -1;
	}

	//setting client port
	m_client_addr.sin_family = AF_INET;
	m_client_addr.sin_port = htons(port);
	m_client_addr.sin_addr.S_un.S_addr = inet_addr(addr);

	//setting server port
	m_server_addr.sin_family = AF_INET;
	m_server_addr.sin_port = htons(port);
	m_server_addr.sin_addr.S_un.S_addr = INADDR_ANY; // we do not care what local address is assigned

	// The bind function is required on an unconnected socket
	err = bind(m_sock, (struct sockaddr *)&m_server_addr, sizeof(m_server_addr));
	if (err == SOCKET_ERROR){
		logE << "bind failed" << std::endl;
		return -1;
	}

	return 1;
}

int UDPSocket::send(const char *buf, int size){
	int len;
	if (m_sock == INVALID_SOCKET){
		logE << "socket unavailable" << std::endl;
		return -1;
	}
	// Sends a datagram to the client address
	len = sendto(m_sock, buf, size, 0, (struct sockaddr *)&m_client_addr, sizeof(m_client_addr));
	if (len == SOCKET_ERROR){
		logE << "send failed(udp)" << std::endl;
		return -1;
	}
	return len;
}

int UDPSocket::receive(char *buf, int size){
	int len;
	int addrLen;

	if (m_sock == INVALID_SOCKET){
		logE << "socket unavailable" << std::endl;
		return -1;
	}

	addrLen = sizeof(m_server_addr);

	// receive datagrams on the bound socket
	len = recvfrom(m_sock, buf, size, 0, (struct sockaddr *)&m_server_addr, &addrLen);

	if (len == SOCKET_ERROR){
		logE << "receive failed" << std::endl;
		return -1;
	}

	return len;

}