#include <iostream>
#include <vector>
#include <mutex>
#include <chrono>
#include <thread>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <signal.h>

#include "Client.h"

#define DEFAULT_PORT 60120
#define CNC_PORT 60069
#define MAX_CONN 2048

std::vector<std::shared_ptr<Client>> g_clients;
std::mutex g_clientsMutex;

bool g_bQuit = false;

void ClientThread(std::shared_ptr<Client> client);

SSL_CTX* SSLCreateCtx();

typedef unsigned char UCHAR;

void InitCnC();

int main() {
	std::cout << "MarlborgeServer v0.1" << std::endl;
	std::cout << "By PR3C14D0" << std::endl << std::endl;

#ifdef WIN32
	WSADATA wsa;
	int nWsa = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (nWsa != 0) {
		std::cout << "[ERROR] Failed to initialize WSA. Method: WSAStartup(WORD, LPWSADATA)" << std::endl;
		return 1;
	}
#endif

	int nPort = DEFAULT_PORT;

	SOCKADDR_IN saIn = { };
	saIn.sin_family = AF_INET;
	saIn.sin_port = htons(nPort);
	inet_pton(AF_INET, "0.0.0.0", (void*)&saIn.sin_addr.s_addr);

	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/* Init OpenSSL */
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	const char* cert = "cert/cert.pem";
	const char* key = "cert/key.pem";

	char certPath[MAX_PATH];
	char keyPath[MAX_PATH];

#ifdef WIN32
	GetFullPathName(cert, MAX_PATH, certPath, nullptr);
	GetFullPathName(key, MAX_PATH, keyPath, nullptr);

	GetCurrentDirectory(MAX_PATH, certPath);
#endif // WIN32

	SSL_CTX* ctx = SSLCreateCtx();
	if (SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		return 1;
	}
	if (SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		return 1;
	}
	SSL_CTX_set_cipher_list(ctx, "HIGH:!aNULL:!MD5");
	SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
	SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);

	if (server == SOCKET_ERROR) {
		std::cout << "[ERROR] Error initializing server socket" << std::endl;
		return 1;
	}

	if (bind(server, (SOCKADDR*)&saIn, sizeof(saIn)) == SOCKET_ERROR) {
		std::cout << "Error binding socket on port " << nPort << std::endl;
		return 1;
	}

	listen(server, MAX_CONN);
	std::cout << "[INFO] Listening on port " << nPort << " for clients" << std::endl;

	InitCnC();

	while (!g_bQuit) {
		SOCKADDR_IN clientIn = { };
		int nClientSize = sizeof(clientIn);
		SOCKET clientSock = accept(server, (SOCKADDR*)&clientIn, &nClientSize);

		/* Resolve client IP */
		char clientIp[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, (void*)&clientIn.sin_addr.s_addr, clientIp, INET_ADDRSTRLEN);
		
		SSL* clientSSL = SSL_new(ctx);
		SSL_set_fd(clientSSL, clientSock);

		if (SSL_accept(clientSSL) <= 0) {
			ERR_print_errors_fp(stderr);
			continue;
		}

		std::lock_guard<std::mutex> lock(g_clientsMutex);
		std::shared_ptr<Client> client = std::make_shared<Client>(clientIp, clientSSL);
		g_clients.push_back(client);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		std::thread clientThread(ClientThread, client);
		clientThread.detach();
	}

	return 0;
}

void InitCnC() {
	SOCKET cnc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN saIn = { };
	saIn.sin_family = AF_INET;
	saIn.sin_port = htons(CNC_PORT);
	inet_pton(AF_INET, "0.0.0.0", (void*)&saIn.sin_addr.s_addr);

	bind(cnc, (SOCKADDR*)&saIn, sizeof(saIn));

	listen(cnc, MAX_CONN);

	while (!g_bQuit) {
		SOCKADDR_IN clientIn = { };
		SOCKET cncClient = accept(cnc, &clientIn, nullptr);
	}
}

void ClientThread(std::shared_ptr<Client> client) {
	Client* pClient = client.get();
	std::string ip = pClient->GetIP();
	
	while (true) {
		Packet packet = pClient->Receive();
		
		Packet handshake;
		handshake.opcode = OPCODES::HANDSHAKE;

		switch (packet.opcode) {
		case OPCODES::HELLO:
			/* Generate 16 random bytes for NONCE */
			UCHAR nonce[16];

			{
				int rc = RAND_bytes(nonce, sizeof(nonce));
				if (rc != 1) {
					ERR_print_errors_fp(stderr);
					return;
				}
			}
			
			pClient->SetNonce(nonce);
			handshake.body.resize(16);
			memcpy(&handshake.body[0], nonce, 16);
			pClient->Send(handshake);
			break;
		case OPCODES::HANDSHAKE:
			char comparingNonce[16];
			strcpy(comparingNonce, (const char*)pClient->GetNonce());

			if (!memcmp(packet.body.c_str(), comparingNonce, 16)) {
				client->m_bHandshake = true; // Set our client state to handshaken (Can now access to privileged opcodes).
		
				/* Definition of our hearbeat packet (Opcode: 0x20) */
				Packet heartbeat;
				heartbeat.opcode = OPCODES::HEARTBEAT;

				/* Generate a random byte for making it a heartbeat interval (Range 0-255. If 0 disconnect Бляяяяяяяяяя) */
				UCHAR interval = 0x00;
				{
					int rc = RAND_bytes(&interval, 1);
					if (rc != 1) {
						ERR_print_errors_fp(stderr);
						return;
					}
				}
				heartbeat.body.resize(1);
				memcpy(&heartbeat.body[0], &interval, 1);
				client->m_heartbeat = interval;
				client->Send(heartbeat);
			}
			break;
		case OPCODES::HEARTBEAT:
			if (client->m_bHandshake) {
				
			}
			else {
				std::cout << ip << " tried to make a heartbeat without handshaking..." << std::endl;
			}
			break;
		case OPCODES::DISCONNECT:
			pClient->Disconnect();
			return;
			break;
		default:
			break;
		}
	}
}

SSL_CTX* SSLCreateCtx() {
	const SSL_METHOD* method = TLSv1_2_server_method();
	SSL_CTX* ctx = SSL_CTX_new(method);
	
	if (ctx == nullptr) {
		std::cout << "[ERROR] Error creating new SSL Context" << std::endl;
		ERR_print_errors_fp(stderr);
		return nullptr;
	}

	return ctx;
}