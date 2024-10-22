#include <iostream>
#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#endif
#include "Color.h"
#include <string>
#include <csignal>

#define SERVER_IP 127.0.0.1 // TODO: Retrieve IP from database.

void PrintBanner();

enum COMMAND {
	EXIT = 0xFF,
	UNKNOWN = 0xF0
};

COMMAND TranslateCommand(std::string command);

bool g_bQuit = false;

void SignalHandler(int signal);

int main() {
#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SetConsoleOutputCP(CP_UTF8);
#endif
	PrintBanner();

	std::signal(SIGINT, SignalHandler);

	SOCKET server = socket();

	while (!g_bQuit) {
		std::cout << YELLOW << "\tpreciado > " << RESET;
		std::string input;
		std::getline(std::cin, input);

		COMMAND cmd = TranslateCommand(input);

		switch (cmd) {
		case EXIT:
			std::cout << BOLDBLUE << "\tDisconnecting..." << RESET << std::endl;
			g_bQuit = true;
			break;
		case UNKNOWN:
		default:
			break;
		}
	}
	return 0;
}

void PrintBanner() {
	std::cout << "\033]0;Marlborge Network\007";
	std::cout << RED << u8R"(
			⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀
			⢻⢷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣾⡇
			⢸⠀⠻⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⣤⢤⣶⠶⠶⢶⣶⡤⣤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⣴⠋⢀⠇
			⠈⣇⠀⠈⠻⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡴⠞⠋⢉⡴⠞⠋⣿⠀⠀⠀⡯⠙⠳⣦⡉⠙⠲⣤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⠟⠁⠀⣼⠀
			⠀⠹⣆⠀⠀⠈⠛⢦⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠞⠋⠀⢀⣰⠏⠀⠀⠀⢻⡄⠀⢰⠇⠀⠀⠈⢻⣄⠀⠀⠙⢷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡶⠋⠁⠀⠀⣰⠃⠀
			⠀⠀⠹⣇⠀⠀⠀⠀⠉⠳⢦⣄⡀⠀⠀⠀⢀⡾⠃⠀⣠⠞⠋⠁⠀⠀⠀⠀⠀⠉⠉⠉⠀⠀⠀⠀⠀⠉⠙⢷⣄⠀⠙⢧⡀⠀⠀⠀⢀⣠⡶⠛⠁⠀⠀⠀⠀⣴⠃⠀⠀
			⠀⠀⠀⠙⢧⡀⠀⠀⠀⠀⠀⠈⠙⠳⠶⢤⣿⣄⣀⣸⠋⢠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠀⢹⣆⣀⣨⣷⡤⠶⠚⠋⠁⠀⠀⠀⠀⠀⢠⡾⠃⠀⠀⠀
			⠀⠀⠀⠀⠈⠻⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⡇⠈⡇⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⣼⠀⣼⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⠏⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠈⠻⢦⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⣧⠀⠀⠀⠀⠀⠀⠀⣷⠀⠀⠀⠀⠀⠀⠀⡟⠀⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡴⠛⠁⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠳⣦⣀⠀⠀⠀⠀⠀⠀⢀⡟⠀⡏⠀⠀⠀⠀⠀⠀⢀⣿⠀⠀⠀⠀⠀⠀⠀⣿⠀⢿⡀⠀⠀⠀⠀⠀⠀⣠⡴⠞⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⡆⠉⢻⡶⢤⣀⡀⢀⡾⠁⣼⠃⠀⠀⠀⠀⠀⠀⣸⠹⣆⠀⠀⠀⠀⠀⠀⠹⣆⠘⢧⡀⢀⣠⡤⢶⡟⠉⢰⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⡏⣧⠀⢸⠃⠀⣨⠿⠋⣠⠞⠁⠀⠀⠀⠀⠀⠀⢸⡏⠀⣹⡆⠀⠀⠀⠀⠀⠀⠘⢦⣈⠛⢿⡅⠀⢸⡇⠀⡿⢻⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡕⣿⣧⣸⡀⠀⠛⡶⢶⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⣹⠰⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠶⣾⠃⠀⢸⣇⡼⡿⢸⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣇⠘⢯⡙⠷⣄⣸⠇⠀⠹⣆⠀⠀⠀⠀⠀⠀⠀⣴⠃⠀⠹⣄⠀⠀⠀⠀⠀⠀⢀⣼⠃⠀⢹⣆⣠⠞⣫⡿⠁⣼⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣄⢀⠙⢷⡘⣿⣷⡶⣄⠙⠷⣄⡀⠀⠀⠀⠘⠁⠀⠀⠀⠈⠃⠀⠀⠀⢀⣴⠞⢁⣤⢶⣾⡿⢡⡾⠋⡀⣰⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣿⠀⠸⣇⠈⣻⣷⣿⠳⣤⡈⠙⠓⠄⠀⠈⠳⡄⠀⣰⠛⠁⠀⠠⠞⠋⢀⣴⠟⣇⣿⡟⠀⣾⠀⠀⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡄⠀⠻⣾⠏⠸⣿⣦⡈⠛⠶⢤⣤⣤⣤⠴⡷⠶⣿⠦⣤⣤⣤⡤⠾⠋⢁⣼⣿⠁⠹⣶⠏⠀⢰⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢷⣄⠀⣿⠀⠀⠘⠿⣿⣦⣤⢴⣿⡿⠃⠀⡷⠛⣦⠀⠘⢿⣷⠦⣤⣶⣿⠟⠁⠀⢀⡿⢀⣰⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣷⠘⣷⣄⠀⠀⠀⠉⠉⠉⠁⠀⠀⠀⡇⠀⡟⠀⠀⠀⠉⠉⠉⠉⠀⠀⠀⣠⣾⠁⡟⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣴⡏⢹⢷⣦⣄⡀⠀⣀⣤⡤⢤⡀⡧⠀⠇⢀⡤⢤⣤⡀⠀⣀⣠⣴⣿⡏⢻⣼⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠀⠀⠀⠈⣧⢸⡾⠁⣨⣿⡟⠙⢯⣀⠀⠀⠀⠀⠀⠀⢀⣀⡿⠉⢻⢿⡁⠘⣿⠃⡿⠀⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⡓⠶⠶⠿⡛⠥⠞⠉⢠⣿⣄⡀⠉⠉⠻⣦⣀⡴⠛⠉⠉⢀⣴⣿⡀⠙⠲⠬⣻⠷⠶⠶⢚⡿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠦⣄⣀⣀⣀⣠⣴⡋⢻⣿⡛⢳⠒⣤⠼⣿⠧⣤⢲⡞⢻⣿⠋⢹⡦⣄⣀⣀⣀⣤⠔⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠹⣆⠈⠛⣾⣿⣧⣿⠙⠛⠓⠛⠚⠛⢋⣇⡾⣿⣷⠋⠀⣼⠋⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣷⡀⣿⣿⣆⠙⠃⠀⠀⠀⠀⠀⠘⠋⣼⡿⣿⢠⡾⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢳⡿⣮⡙⠛⣟⣻⣯⣯⣽⣟⣿⠛⢋⣴⣷⠟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣼⣏⠛⣋⣤⠶⠒⠶⣤⣙⠛⣹⢰⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣧⡉⠉⠉⠀⣠⣤⡄⠀⠉⠉⢁⣼⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
			⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⠲⠤⠞⠋⠀⠙⠶⠤⠖⠋⠁⠀⠀

)" << "\t\t\t\t\t      " BG_RED << BOLDWHITE << "Marlborge Network" << RESET << std::endl;
}

void SignalHandler(int signal) {
	if (signal == SIGINT) {
		std::cout << BOLDBLUE << "\nInterruption received (Ctrl+C). Exiting...\n" << RESET << std::endl;
		g_bQuit = true;
	}
}

COMMAND TranslateCommand(std::string command) {
	COMMAND cmd = COMMAND::UNKNOWN;

	if (command == "exit") {
		cmd = COMMAND::EXIT;
	}

	return cmd;
}