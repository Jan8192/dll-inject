#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <chrono>
#include <thread>

DWORD getProcId(const char* procName) {
	DWORD procId = 0;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(snap, &procEntry)) {
			do
			{
				if (!_stricmp(procEntry.szExeFile, procName)) {
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(snap, &procEntry));
		}
	}

	CloseHandle(snap);
	return procId;
}

int main()
{
	const char* dll = "";
	const char* proc = "csgo.exe";
	DWORD procId = 0;

	while (!procId) {
		procId = getProcId(proc);
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

	if (hProc && hProc != INVALID_HANDLE_VALUE) {
		void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		if (loc) {
			WriteProcessMemory(hProc, loc, dll, strlen(dll) + 1, 0);
		}

		HANDLE hTHread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
		if (hTHread) {
			CloseHandle(hTHread);
		}
		if (hProc) {
			CloseHandle(hProc);
		}
	}

	return 0;
}