#pragma once
#include <iostream>
#include <Windows.h>

namespace Memory {
	LPVOID CreateTrampoline(LPVOID addr);
}