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

class Client {
private:
	SSL* m_sock;
	std::string m_ip;
	unsigned char m_nonce[16];
public:
	Client(std::string ip, SSL* sock);
	std::string GetIP();

	void SetNonce(unsigned char* nonce);
	unsigned char* GetNonce();

	bool m_bHandshake;
	unsigned char m_heartbeat;

	Packet Receive();
	void Send(Packet packet);
	void Disconnect();
};