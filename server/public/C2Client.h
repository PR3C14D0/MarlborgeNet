#pragma once
#include <iostream>
#include "Client.h"

class C2Client : public Client {
public:
	C2Client(std::string ip, SSL* ssl);
	std::string m_username;

	bool m_bLoggedIn;
};