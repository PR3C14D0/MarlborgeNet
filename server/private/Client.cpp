#include "Client.h"

Client::Client(std::string ip, SSL* sock) {
	this->m_sock = sock;
	this->m_ip = ip;
	memset(this->m_nonce, 0, sizeof(this->m_nonce));

	this->m_bHandshake = false;
	this->m_heartbeat = 0x00;
}

std::string Client::GetIP() {
	return this->m_ip;
}

Packet Client::Receive() {
	char buff[1024] = { };
	int nRecvSize = SSL_read(this->m_sock, buff, 1024);

	std::string received;
	if (nRecvSize > 0) {
		received.resize(nRecvSize);
		memcpy(&received[0], buff, nRecvSize);
	}

	int nBodySize = nRecvSize - 1;

	Packet packet;
	if (nRecvSize > 0) {
		packet.opcode = static_cast<OPCODES>(received[0]);
		packet.body = received.substr(1, nBodySize);
	}

	return packet;
}

void Client::SetNonce(unsigned char* nonce) {
	memcpy(this->m_nonce, nonce, 16);
	return;
}

unsigned char* Client::GetNonce() {
	return this->m_nonce;
}

void Client::Send(Packet packet) {
	std::string buffer;
	buffer.resize(packet.body.size() + 1);
	buffer[0] = packet.opcode;
	memcpy(&buffer[1], packet.body.c_str(), packet.body.size());
	SSL_write(this->m_sock, &buffer[0], buffer.size());
}

void Client::Disconnect() {
	SSL_shutdown(this->m_sock);
	delete this;
}