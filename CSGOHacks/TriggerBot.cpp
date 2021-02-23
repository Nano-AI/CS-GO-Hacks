#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Offsets.h"

const int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN); const int xhairx = SCREEN_WIDTH / 2;
const int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN); const int xhairy = SCREEN_HEIGHT / 2;

HWND hwnd;
DWORD procId;
HANDLE hProcess;
uintptr_t moduleBase;
HDC hdc;
int closest; //Used in a thread to save CPU usage.

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

uintptr_t getLocalPlayer() {
	return RPM< uintptr_t>(moduleBase + dwLocalPlayer);
}

uintptr_t getPlayer(int index) {  //Each player has an index. 1-64
	return RPM< uintptr_t>(moduleBase + dwEntityList + index * 0x10); //We multiply the index by 0x10 to select the player we want in the entity list.
}

int getTeam(uintptr_t player) {
	return RPM<int>(player + m_iTeamNum);
}

int getCrosshiarId(uintptr_t player) {
	return RPM<int>(player + m_iCrosshairId);
}

int main() {
	hwnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId(hwnd, &procId);
	moduleBase = GetModuleBaseAddress("client.dll");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	while (!GetAsyncKeyState(VK_END)) { //press the "end" key to end the hack
		int CrosshairID = getCrosshiarId(getLocalPlayer());
		int CrosshairTeam = getTeam(getPlayer(CrosshairID - 1));
		int LocalTeam = getTeam(getLocalPlayer());
		// Change CrosshairID < 32 if < 64 doesn't work
		if (CrosshairID > 0 && CrosshairID < 64 && LocalTeam != CrosshairTeam)
			if (GetAsyncKeyState(VK_MENU)) {
				mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, 0, 0);
				Sleep(50);
			}
	}
}
