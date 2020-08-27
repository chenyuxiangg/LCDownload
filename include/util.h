#ifndef UTIL_H
#define UTIL_H
#ifdef WIN32
#include <windows.h>
#include <direct.h>
#include <io.h>
#endif

#ifdef LINUX
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <string>
using std::string;

LPCWSTR n2w(const string&);

#endif
