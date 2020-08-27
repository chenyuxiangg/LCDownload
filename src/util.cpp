#include "util.h"

LPCWSTR n2w(const string& str) {
	WCHAR wsname[256]; 
	memset(wsname, 0, sizeof(wsname)); 
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), strlen(str.c_str()) + 1, wsname, 
	sizeof(wsname) / sizeof(wsname[0])); 
	return wsname;
}