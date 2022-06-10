#include "Del.h"
#ifdef _WIN32
#include <Windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#elif __linux__
#include <stdio.h>
#include <string.h>
#endif

#include "../../Network/Server.h"
#include <iostream>

Del::Del(const tstring &path, const CSOCKET &clientSocket)
    : path{path}, clientSocket{clientSocket} {}

bool Del::execute() {
  tstring currentDirectory = Server::GetClientDirectory(clientSocket);

  if (!GetAbsolutePath(currentDirectory, path)) {
    return false;
  }
  path = currentDirectory;

#ifdef _WIN32
  CHECK_RETURN(DeleteFile(path.c_str()))

#elif __linux__

  CHECK_RETURN_LINUX(remove(path.c_str()))

#endif
  path = _T("Deletted: ") + path + _T("\n");
  Server::AddCommandInString(path);
  return true;
}
