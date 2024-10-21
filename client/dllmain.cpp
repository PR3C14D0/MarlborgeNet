#include <iostream>

#include <thread>
#include <chrono>

#include "Packet.h"
#include "Server.h"
#include <Psapi.h>

void Main();

#define IP "127.0.0.1" // TODO: Get the IP from one of the proxies from our MongoDB Database.
#define PORT 60120 // TODO: Read from our MongoDB database.

BOOL DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		Main();
		break;
	}
	return TRUE;
}

void HeartbeatThread();

Server* g_server = nullptr;
UINT g_nACK = 0;

typedef void(*tMain)(void);

LPVOID FindBySignature(const char* signature, size_t sigSize, LPVOID startAddress, size_t size);

void Main() { /* Signature: 40 55 57 48 81 EC 98 07 00 00 */
#ifndef NDEBUG
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
#endif

#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif // WIN32

	std::cout << "MarlborgeDLL Injected." << std::endl;
	std::cout << "By PR3C14D0" << std::endl;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saIn = { };
	saIn.sin_port = htons(PORT);
	saIn.sin_family = AF_INET;
	inet_pton(AF_INET, IP, (void*)&saIn.sin_addr.s_addr);

	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	const SSL_METHOD* method = TLSv1_2_client_method();
	SSL_CTX* ctx = SSL_CTX_new(method);
	if (ctx == nullptr) {
		std::cout << "[ERROR] Error creating a new SSL context" << std::endl;
		ERR_print_errors_fp(stderr);
		return;
	}
	
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);

	connect(sock, (SOCKADDR*)&saIn, sizeof(saIn));
	SSL* ssl = SSL_new(ctx);
	SSL_set_fd(ssl, sock);

	SSL_connect(ssl);

	g_server = new Server(ssl);
	Packet hello;
	hello.opcode = OPCODES::HELLO;

	g_server->Send(hello);
	Packet handshake = g_server->Receive();
	g_server->Send(handshake);

	Packet heartbeat = g_server->Receive();

	g_server->m_heartbeat = heartbeat.body[0];
	std::thread heartbeatThread(HeartbeatThread);
	heartbeatThread.detach();

	while (true) {
		Packet packet = g_server->Receive();
		switch (packet.opcode) {
			case OPCODES::HEARTBEAT:
				g_nACK++;
				break;
		}
	}
}

void HeartbeatThread() {
	std::this_thread::sleep_for(std::chrono::seconds(g_server->m_heartbeat));

	std::cout << "Heartbeat" << std::endl;
	Packet heartbeat;
	heartbeat.opcode = OPCODES::HEARTBEAT;
	g_server->Send(heartbeat);

	return HeartbeatThread();
}

int main() {
	if (IsDebuggerPresent()) {
		return 1;
	}
	else {
		char mainSig[] = { 0x40, 0x55, 0x57, 0x48, 0x81, 0xEC, 0x98, 0x07, 0x00, 0x00 };
		size_t sigLength = strlen(mainSig);

		HMODULE hModule = GetModuleHandle(nullptr);

		MODULEINFO info;
		GetModuleInformation(GetCurrentProcess(), hModule, &info, sizeof(info));

		LPVOID startAddr = info.lpBaseOfDll;
		size_t imageSize = info.SizeOfImage;

		LPVOID addr = FindBySignature(mainSig, sigLength, startAddr, imageSize);

		std::cout << addr << std::endl;
		
		LPVOID tramp = nullptr;
		LPVOID tempTramp = addr;
		do {
			tramp = VirtualAlloc(tempTramp, 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			tempTramp = (char*)tempTramp + 0x1000;
		} while (tramp == nullptr);
		DWORD relAddr = (DWORD)addr - (DWORD)tramp - 5;
		*(char*)tramp = 0xE9;
		*(DWORD*)((char*)tramp + 1) = relAddr;
		((tMain)tramp)();
	}
	return 0;
}

LPVOID FindBySignature(const char* signature, size_t sigSize, LPVOID startAddress, size_t size) {
	const unsigned char* sig = reinterpret_cast<const unsigned char*>(signature);
	unsigned char* current = reinterpret_cast<unsigned char*>(startAddress);

	for (size_t i = 0; i < size - sigSize; ++i) {
		if (memcmp(current + i, sig, sigSize) == 0) {
			return current + i; 
		}
	}
	return nullptr;
}