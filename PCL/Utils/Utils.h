#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <queue>
#include <string>

#ifdef _WIN32

#define _WINSOCKAPI_
#include <windows.h>
#include <ws2tcpip.h>

//#include <Windows.h>
#include <tchar.h>
#endif

#include "../Network/Server.h"

// DEF UNICODE
#ifdef _UNICODE
#define tstring std::wstring
#define tcout std::wcout
#define tcin std::wcin
#define to_tstring std::to_wstring
#else
#define tstring std::string
#define tcout std::cout
#define tcin std::cin
#define to_tstring std::to_string
#define _T(x) x
#define TCHAR char
#endif

// Check if value is False
#define CHECK_RETURN(x)                                                        \
  if (!x) {                                                                    \
    Server::AddCommandInString(GetLastErrorAsString(), _T("\n"));              \
    return false;                                                              \
  }
// Check if value is -1 (LINUX)
#define CHECK_RETURN_LINUX(x)                                                  \
  if (x == -1) {                                                               \
    Server::AddCommandInString(strerror(errno), _T("\n"));                         \
    return false;                                                              \
  }

// Check if hande is invalid
#define CHECK_HANDLE_RETURN(x)                                                 \
  if (x == INVALID_HANDLE_VALUE) {                                             \
    Server::AddCommandInString(GetLastErrorAsString(), _T("\n"));              \
    return false;                                                              \
  }

// Check if handle key is invalid
#define CHECK_HANLE_KEY_RETURN(x)                                              \
  if (x == NULL) {                                                             \
    Server::AddCommandInString(_T("Invalid path!"), _T("\n"));                 \
    return false;                                                              \
  }

// Check if return value is error succes
#define CHECK_RETURN_VALUE_ERROR_SUCCESS(x)                                    \
  if (x != ERROR_SUCCESS) {                                                    \
    Server::AddCommandInString(_T("Error no: "), to_tstring(x));               \
    Server::AddCommandInString(_T("\n"));                                      \
    return false;                                                              \
  }

// Check if file atribute is invalid
#define CHECK_FILE_RETURN(x)                                                   \
  if (x == INVALID_FILE_ATTRIBUTES) {                                          \
    Server::AddCommandInString(to_tstring(x), _T("\n"));                       \
    return false;                                                              \
  }

#define CHECK_SET_FILE_POINTER_RETURN(x)                                       \
  if (x == INVALID_SET_FILE_POINTER) {                                         \
    Server::AddCommandInString(GetLastErrorAsString(), _T("\n"));              \
    return false;                                                              \
  }

// Print the exception and return false
#define RETURN_EXCEPTION()                                                     \
  {                                                                            \
    Server::AddCommandInString((TCHAR *)e.what(), _T("\n"));                   \
    return false;                                                              \
  }
// Print the error and return false
#define RETURN_ERROR()                                                         \
  {                                                                            \
    Server::AddCommandInString(GetLastErrorAsString(), _T("\n"));              \
    return false;                                                              \
  }

#define RETURN_INVALID_COMMAND()                                               \
  {                                                                            \
    Server::AddCommandInString(_T("Invalid command!\n"));                      \
    return false;                                                              \
  }

#define PRINT_SPACE(x)                                                         \
  for (DWORD i = 0; i < x; i++)                                                \
    if (i == x - 1)                                                            \
      Server::AddCommandInString(_T("|__"));                                   \
    else                                                                       \
      Server::AddCommandInString(_T("   "));

#ifdef _WIN32

void CloseHandleDeleter(HANDLE *p);
void CloseFindDeleter(HANDLE *p);
void CloseRegKeyDeleter(HKEY *p);

tstring GetLastErrorAsString();
HKEY GetHKEYByString(const tstring &hKeyString);
DWORD GetRegValueTypeByString(const tstring &valueType);
bool GetRegistryHive(tstring &path, tstring &hive);

#endif
tstring lowerString(tstring str);
bool isNumber(std::string str);
bool GetAbsolutePath(tstring &currentDirectory, tstring path);

#endif
