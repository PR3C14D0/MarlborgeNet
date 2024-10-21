#include <iostream>
#include <Windows.h>
#include <libloaderapi.h>
#include <TlHelp32.h>

DWORD GetPID(const char* procName);

int main() {
	const char* procName = "chrome.exe";
	DWORD dwPid = GetPID(procName);
	std::cout << "Process ID:" << dwPid << std::endl;

	const char* dllName = "MarlborgeDLL.dll";
	char dllPath[MAX_PATH];

	GetFullPathName(dllName, MAX_PATH, dllPath, nullptr);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	LPVOID lpAddr = VirtualAllocEx(hProcess, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	WriteProcessMemory(hProcess, lpAddr, dllPath, MAX_PATH, nullptr);

	HANDLE hThread = CreateRemoteThreadEx(hProcess, nullptr, NULL, (LPTHREAD_START_ROUTINE)LoadLibrary, lpAddr, NULL, nullptr, nullptr);

	if (hThread) {
		CloseHandle(hThread);
	}

	if (hProcess) {
		CloseHandle(hProcess);
	}

	return 0;
}

DWORD GetPID(const char* procName) {
	DWORD dwPid = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	
	PROCESSENTRY32 pe = { };
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnap, &pe)) {
		do {
			if (!strcmp(procName, pe.szExeFile)) {
				dwPid = pe.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnap, &pe));
	}

	return dwPid;
}