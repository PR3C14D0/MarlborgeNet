#pragma once
#include <iostream>
#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#endif
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

#include "Packet.h"

class Server {
private:
	SSL* m_sock;
public:
	Server(SSL* sock);

	unsigned char m_heartbeat;

	Packet Receive();
	void Send(Packet packet);
	void Disconnect();
};