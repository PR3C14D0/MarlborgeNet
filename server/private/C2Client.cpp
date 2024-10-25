#include "C2Client.h"

C2Client::C2Client(std::string ip, SSL* ssl) : Client::Client(ip, ssl) {
	this->m_username = "Guest";
	this->m_bLoggedIn = false;
}