#include "Delreboot.h"
#include <Windows.h>
#include <iostream>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

Delreboot::Delreboot(const tstring &path, const CSOCKET &clientSocket)
    : path{path}, clientSocket{clientSocket} {}

bool Delreboot::execute() {

  tstring currentDirectory = Server::GetClientDirectory(clientSocket);

  if (!GetAbsolutePath(currentDirectory, path)) {
    return false;
  }
  path = currentDirectory;

  CHECK_RETURN(PathFileExists(path.c_str()))

  CHECK_RETURN(MoveFileEx(path.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT))

  path = _T("File: ") + path + _T(" deleted on reboot!\n");

  Server::AddCommandInString(path);
  return true;
}
