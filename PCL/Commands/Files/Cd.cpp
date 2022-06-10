#include "Cd.h"
#include <iostream>
#include <string>
#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "../../Network/Server.h"

Cd::Cd(const tstring &path, CSOCKET &clientSocket)
    : path{path}, clientSocket{clientSocket} {}

bool Cd::execute() {

  tstring currentDirectory = Server::GetClientDirectory(clientSocket);

  if (!GetAbsolutePath(currentDirectory, path)) {
    return false;
  }

  Server::ChangeClientDirectory(clientSocket, currentDirectory);

  currentDirectory =
      _T("Current Directory changed to: ") + currentDirectory + _T("\n");

  Server::AddCommandInString(currentDirectory);

  return true;
}
