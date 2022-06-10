#include "Chm.h"
#ifdef _WIN32
#include <Windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#elif __linux__
#include <string.h>
#include <sys/stat.h>
#endif
#include <iostream>

Chm::Chm(const tstring &path, const tstring &flag, const CSOCKET &clientSocket)
    : path{path}, flag{flag}, clientSocket{clientSocket} {}

bool Chm::execute() {
  int atributesFlag = 0;
  int currentFlag = 0;

  tstring currentDirectory = Server::GetClientDirectory(clientSocket);

  if (!GetAbsolutePath(currentDirectory, path)) {
    return false;
  }

  path = currentDirectory;

  tstring currentFlagStr = {};

  for (auto c : flag) {
    if (c != _T(' ')) {
      currentFlagStr = currentFlagStr + c;
    } else {
      try {
        currentFlag = stoi(currentFlagStr);
      } catch (const std::invalid_argument &e) {
        RETURN_EXCEPTION()
      } catch (const std::out_of_range &e) {
        RETURN_EXCEPTION()
      }
      atributesFlag = atributesFlag | currentFlag;
      currentFlag = 0;
      currentFlagStr = {};
    }
  }

  try {
    currentFlag = stoi(currentFlagStr);
  } catch (const std::invalid_argument &e) {
    RETURN_EXCEPTION()
  } catch (const std::out_of_range &e) {
    RETURN_EXCEPTION()
  }

  atributesFlag = atributesFlag | currentFlag;

#ifdef _WIN32

  CHECK_RETURN(SetFileAttributes(path.c_str(), atributesFlag))

#elif __linux__

  CHECK_RETURN_LINUX(chmod(path.c_str(), atributesFlag))

#endif
  Server::AddCommandInString(_T("Chm succes!\n"));
  return true;
}
