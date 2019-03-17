#include "TCPClient.h"
#include "Logger.h"

#pragma warning(disable: 4996)

TCPClient::TCPClient(void)
{
	int err;
	err = WSAStartup(MAKEWORD(2, 0), &m_wsaData); // initiates use of the Winsock DLL by our process.
	if (err != 0) {
		logE << "tcp client";
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

TCPClient::~TCPClient(void)
{
	if (m_sock != INVALID_SOCKET){
		closesocket(m_sock);
	}
	WSACleanup(); // terminates use of the Winsock 2 DLL
}

int TCPClient::open(char *addr, int port){
	int err;
	/*
	* AF_INET (IPv4), SOCK_STREAM (stream), the 0 value we do not wish to specify a protocol
	* and the service provider will choose the protocol to use.
	*/
	m_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (m_sock == INVALID_SOCKET){
		logE << "create socket failed" << std::endl;
		return -1;
	}

	m_server.sin_family = AF_INET;
	m_server.sin_port = htons(port);
	m_server.sin_addr.S_un.S_addr = inet_addr(addr); 

	err = connect(m_sock, (const sockaddr *)&m_server, sizeof(m_server));
	if (err == SOCKET_ERROR){
		logE << "connect failed" << std::endl;
		return -1;
	}
	return 1;
}

int TCPClient::receive(char *buf, int size){
	int len;

	if (m_sock == INVALID_SOCKET){
		logE << "socket unavailable" << std::endl;
		return -1;
	}

	len = recv(m_sock, buf, size, 0);

	if (len == SOCKET_ERROR){
		logE << "receive failed" << std::endl;
		return -1;
	}

	return len;

}

int TCPClient::send(const char *buf, int size){
	int len;

	if (m_sock == INVALID_SOCKET){
		logE << "socket unavailable" << std::endl;
		return -1;
	}

	len = ::send(m_sock, buf, size, 0);
	if (len == SOCKET_ERROR){
		logE << "send failed(tcp)" << std::endl;
		logE << "err no:" << WSAGetLastError() << std::endl;
		return -1;
	}

	return len;

}