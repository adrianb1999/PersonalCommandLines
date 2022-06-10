#include "Utils.h"
#include <map>

#ifdef _WIN32
#include <Windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#elif __linux__
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef _WIN32
void CloseFindDeleter(HANDLE *p) {
  if (!FindClose(*p)) {
    Server::AddCommandInString(GetLastErrorAsString(), _T("\n"));
  }
}

void CloseHandleDeleter(HANDLE *p) {
  if (!CloseHandle(*p)) {
    Server::AddCommandInString(GetLastErrorAsString(), _T("\n"));
  }
}

void CloseRegKeyDeleter(HKEY *p) {
  LSTATUS closeStatus = RegCloseKey(*p);
  if (closeStatus != ERROR_SUCCESS) {
    Server::AddCommandInString(to_tstring(closeStatus), _T("\n"));
  }
}

HKEY GetHKEYByString(const tstring &hKeyString) {
  static const std::map<const tstring, HKEY> hKeyMap = {
      {_T("HKEY_CLASSES_ROOT"), HKEY_CLASSES_ROOT},
      {_T("HKEY_CURRENT_CONFIG"), HKEY_CURRENT_CONFIG},
      {_T("HKEY_CURRENT_USER"), HKEY_CURRENT_USER},
      {_T("HKEY_LOCAL_MACHINE"), HKEY_LOCAL_MACHINE},
      {_T("HKEY_USERS"), HKEY_USERS}};
  try {
    return hKeyMap.at(hKeyString);
  } catch (const std::out_of_range &e) {
    Server::AddCommandInString((TCHAR *)e.what(), _T("\n"));
    return NULL;
  }
}

DWORD GetRegValueTypeByString(const tstring &valueType) {

  static const std::map<const tstring, DWORD> regTypeMap = {
      {_T("REG_BINARY"), REG_BINARY},                           // 3ul
      {_T("REG_DWORD"), REG_DWORD},                             // 4ul
      {_T("REG_DWORD_LITTLE_ENDIAN"), REG_DWORD_LITTLE_ENDIAN}, // 4ul
      {_T("REG_DWORD_BIG_ENDIAN"), REG_DWORD_BIG_ENDIAN},       // 5ul
      {_T("REG_EXPAND_SZ"), REG_EXPAND_SZ},                     // 2ul
      {_T("REG_LINK"), REG_LINK},                               // 6ul
      {_T("REG_MULTI_SZ"), REG_MULTI_SZ},                       // 7ul
      {_T("REG_NONE"), REG_NONE},                               // 0ul
      {_T("REG_QWORD"), REG_QWORD},                             // 11ul
      {_T("REG_QWORD_LITTLE_ENDIAN"), REG_QWORD_LITTLE_ENDIAN}, // 11ul
      {_T("REG_SZ"), REG_SZ}};                                  // 1ul
  try {
    return regTypeMap.at(valueType);
  } catch (const std::out_of_range &e) {
    Server::AddCommandInString((TCHAR *)e.what(), _T("\n"));
    return NULL;
  }
}

tstring GetLastErrorAsString() {

  DWORD errorMessageID = ::GetLastError();
  if (errorMessageID == 0) {
    return tstring();
  }
  LPTSTR messageBuffer = nullptr;

  size_t size = FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR)&messageBuffer, 0, NULL);

  tstring message(messageBuffer, size);

  LocalFree(messageBuffer);

  return message;
}

bool GetRegistryHive(tstring &path, tstring &hive) {
  if (path[0] != _T('H')) {
    Server::AddCommandInString(_T("Invalid path!\n"));
    return false;
  }
  size_t position = path.find('\\');
  hive = path.substr(0, position);
  if (position == -1) {
    Server::AddCommandInString(_T("Invalid path!\n"));
    return false;
  }
  path.erase(0, position + 1);
  return true;
}

#endif

tstring lowerString(tstring str) {
  for (auto &c : str) {
    if (c >= _T('A') && c <= _T('Z')) {
      c = c + 32;
    }
  }
  return str;
}

// LINUX :
bool isNumber(std::string str) {
  for (auto &c : str)
    if (c < '0' || c > '9') {
      return false;
    }
  return true;
}

bool GetAbsolutePath(tstring &currentDirectory, tstring path) {

  TCHAR buff[1024];

#ifdef _WIN32

  if (path[0] == _T('.')) {
    path = currentDirectory + _T("\\") + path;
  }

  if (!PathFileExists(path.c_str())) {
    path = currentDirectory + _T("\\") + path;
    CHECK_RETURN(PathFileExists(path.c_str()))
  }

  CHECK_RETURN(GetFullPathName(path.c_str(), 1024, buff, NULL))

#elif __linux__

  if (path[0] == _T('.')) {
    path = currentDirectory + _T("/") + path;
  } else {
    size_t position = path.find("/");
    if (position == -1) {
      path = currentDirectory + "/" + path;
    }
  }
  struct stat sb;
  if (realpath(path.c_str(), buff) == NULL) {
    Server::AddCommandInString(strerror(errno), "\n");
    return false;
  }
#endif
  currentDirectory = buff;
  return true;
}
