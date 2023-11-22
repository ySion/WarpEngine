#include <cstdio>
#include <windows.h>
#include <stdexcept>

void AllocTerminal() {
	if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
		if (!AllocConsole()) {
			throw std::runtime_error{ "AllocConsole error" };
		}
	}

	FILE* fp;
	freopen_s(&fp, "conin$", "r", stdin);
	freopen_s(&fp, "conout$", "w", stdout);
	freopen_s(&fp, "conout$", "w", stderr);
}
