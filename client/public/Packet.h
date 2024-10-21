#pragma once
#include <iostream>

enum OPCODES {
	HELLO = 0x01,
	HANDSHAKE = 0x02,
	DISCONNECT = 0x10,
	HEARTBEAT = 0x20
};

struct Packet {
	OPCODES opcode;
	std::string body;
};