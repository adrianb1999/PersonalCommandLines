#include "Move.h"
#ifdef _WIN32
#include <Windows.h>
#include <iostream>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#elif __linux__
#include <stdio.h>
#include <string.h>
#endif

Move::Move(const tstring &existingFile, const tstring &newFile,
           const CSOCKET &clientSocket)
    : existingFile{existingFile}, newFile{newFile}, clientSocket{clientSocket} {
}

bool Move::execute() {

  tstring currentDirectory = Server::GetClientDirectory(clientSocket);

  if (!GetAbsolutePath(currentDirectory, existingFile)) {
    return false;
  }

  existingFile = currentDirectory;

#ifdef _WIN32

  CHECK_RETURN(MoveFile(existingFile.c_str(), newFile.c_str()))

#elif __linux__

  CHECK_RETURN_LINUX(rename(existingFile.c_str(), newFile.c_str()))

#endif
  existingFile =
      _T("File: ") + existingFile + _T(" moved to: ") + newFile + _T("\n");
  Server::AddCommandInString(existingFile);
  return true;
}
