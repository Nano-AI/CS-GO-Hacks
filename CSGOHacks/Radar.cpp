#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Offsets.h"


DWORD dwPid;
HANDLE hProcess;
DWORD client;

uintptr_t GetModuleBaseAddress(const char* modName) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!strcmp(modEntry.szModule, modName)) {
					CloseHandle(hSnap);
					return (uintptr_t)modEntry.modBaseAddr;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
}

template<typename T> T RPM(SIZE_T address) {
	T buffer;
	ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
	return buffer;
}

template<typename T> void WPM(SIZE_T address, T buffer) {
	WriteProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(buffer), NULL);
}

int main() {
	GetWindowThreadProcessId(FindWindowA(0, "Counter-Strike: Global Offensive"), &dwPid);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwPid);
	client = GetModuleBaseAddress("client.dll");

	while (!GetAsyncKeyState(VK_END)) {
		for (int i = 1; i < 64; i++) {
			DWORD dwCurrentEntity = RPM<DWORD>(client + dwEntityList + i * 0x10);
			if (dwCurrentEntity) {
				WPM<bool>(dwCurrentEntity + m_bSpotted, true);
			}
		}
		Sleep(50);
	}
}