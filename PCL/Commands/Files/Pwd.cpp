#include "Pwd.h"

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <string.h>
#include <unistd.h>
#endif

#include "../../Network/Server.h"
#include <iostream>

#define PATH_SIZE 2024

Pwd::Pwd(const CSOCKET &clientSocket) : clientSocket{clientSocket} {}

bool Pwd::execute() {

  tstring strPath = Server::GetClientDirectory(clientSocket);
  Server::AddCommandInString(strPath, _T("\n"));

  return true;
}
