#include "CreateProc.h"
#ifdef _WIN32
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#elif __linux__
#include <spawn.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#endif

CreateProc::CreateProc(const tstring &path, const CSOCKET &clientSocket)
    : path{path}, clientSocket{clientSocket} {}
bool CreateProc::execute() {

  tstring currentDirectory = Server::GetClientDirectory(clientSocket);

  if (!GetAbsolutePath(currentDirectory, path)) {
    return false;
  }

  path = currentDirectory;

#ifdef _WIN32
  STARTUPINFO startupInfo;
  PROCESS_INFORMATION processInformation;

  memset(&startupInfo, 0, sizeof(STARTUPINFO));

  startupInfo.cb = sizeof(startupInfo);

  if (!CreateProcess(path.c_str(), NULL, NULL, NULL, FALSE,
                     NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo,
                     &processInformation)) {
    GetLastErrorAsString();
  }

  CloseHandle(processInformation.hThread);
  CloseHandle(processInformation.hProcess);

#elif __linux__
  pid_t processID;
  CHECK_RETURN_LINUX(posix_spawn(&processID, path.c_str(), NULL, NULL,
                                 (char *const *)path.c_str(), NULL))
#endif
  Server::AddCommandInString(_T("CreateProc succes!\n"));
  return true;
}
