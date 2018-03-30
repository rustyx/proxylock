#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

char *appName = "proxylock";

void die(const char* fmt, ...)
{
	DWORD lastError = GetLastError();
	LPSTR lpMsgBuf = NULL;
	char buf[1024];
	if (lastError)
		FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR) &lpMsgBuf, 0, NULL);
	va_list va;
	va_start(va, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, va);
	va_end(va);
	if (lpMsgBuf) {
		strcat(buf, ": ");
		int len = strlen(lpMsgBuf);
		while (len && (lpMsgBuf[len - 1] == '\r' || lpMsgBuf[len - 1] == '\n'))
			lpMsgBuf[--len] = 0;
		strcat(buf, lpMsgBuf);
		LocalFree(lpMsgBuf);
	}
	MessageBoxA(NULL, buf, appName, MB_ICONSTOP);
	exit(1);
}

char *inetSettingsKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";

int CALLBACK WinMain(HINSTANCE hi, HINSTANCE hpi, LPSTR lpCmdLine, int nCmdShow)
{
	int lastValidStatus = 2;
	HKEY hkey, hkey2;
	DWORD val, len, type;
	BYTE buf[1024];
	for (;;) {
		if (RegOpenKeyEx(HKEY_CURRENT_USER, inetSettingsKey, 0, KEY_ALL_ACCESS, &hkey)) {
			die("RegOpenKey failed");
		}
		if (RegOpenKeyEx(hkey, "Connections", 0, KEY_ALL_ACCESS, &hkey2)) {
			die("RegOpenKey failed");
		}
		len = sizeof(buf);
		if (!RegQueryValueEx(hkey2, "DefaultConnectionSettings", NULL, &type, buf, &len)) {
			if ((buf[8] & 0x0C) != 0) {
				buf[8] = lastValidStatus | 1;
				if (++buf[4] == 0)
					++buf[5];
				RegSetValueEx(hkey2, "DefaultConnectionSettings", 0, type, buf, len);
			} else {
				lastValidStatus = buf[8] & 2;
			}
		}
		len = sizeof(buf);
		if (!RegQueryValueEx(hkey2, "SavedLegacySettings", NULL, &type, buf, &len)) {
			// [8] = 1
			// ProxyEnable: [8] | 0x02			"ProxyEnable"=dword:00000001
			// AutoConfigURL: [8] | 0x04		"AutoConfigURL"="http://127.0.0.1:57352/proxy.pac"
			// Proxy auto detect: [8] | 0x08
			if ((buf[8] & 0x0C) != 0) {
				buf[8] = lastValidStatus | 1;
				if (++buf[4] == 0)
					++buf[5];
				RegSetValueEx(hkey2, "SavedLegacySettings", 0, type, buf, len);
			}
		}
		val = 0; len = sizeof(val);
		RegQueryValueEx(hkey, "ProxyEnable", NULL, NULL, (LPBYTE)&val, &len);
		if (!!val ^ !!(lastValidStatus & 2)) {
			val = (lastValidStatus & 2) >> 1;
			RegSetValueEx(hkey, "ProxyEnable", 0, REG_DWORD, (LPBYTE)&val, sizeof(val));
		}
		len = sizeof(buf);
		if (!RegQueryValueEx(hkey, "AutoConfigURL", NULL, NULL, buf, &len)) {
			RegDeleteValue(hkey, "AutoConfigURL");
		}
		RegCloseKey(hkey2);
		if (RegNotifyChangeKeyValue(hkey, TRUE, REG_NOTIFY_CHANGE_LAST_SET, NULL, FALSE)) {
			die("RegNotifyChangeKeyValue failed");
		}
		RegCloseKey(hkey);
		Sleep(100);
	}
	return 0;
}
