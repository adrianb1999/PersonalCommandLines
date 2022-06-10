#include "KillProc.h"
#ifdef _WIN32
#include <TlHelp32.h>

KillProc::KillProc(const DWORD &processID, const bool &child)
    : processID{processID}, child{child} {}

#elif __linux__
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFSIZE 255

KillProc::KillProc(const int &processID, const bool &child)
    : processID{processID}, child{child} {}

#endif

bool KillProc::execute() {

  std::vector<int> procToBeKilled;

#ifdef _WIN32
  if (!child) {
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

    std::unique_ptr<HANDLE, void (*)(HANDLE *)> processHandlePointer(&processHandle,
                                                          CloseHandleDeleter);
    CHECK_HANDLE_RETURN(processHandle)
    CHECK_RETURN(TerminateProcess(processHandle, 1))

    return true;
  }

  PROCESSENTRY32 processEntry;
  memset(&processEntry, 0, sizeof(PROCESSENTRY32));
  processEntry.dwSize = sizeof(PROCESSENTRY32);

  HANDLE processChildHandle = INVALID_HANDLE_VALUE;

  std::unique_ptr<HANDLE, void (*)(HANDLE *)> hChildProc_ptr(
      &processChildHandle, CloseHandleDeleter);

  HANDLE snapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  std::unique_ptr<HANDLE, void (*)(HANDLE *)> snapHandlePointer(&snapHandle,
                                                        CloseHandleDeleter);

  CHECK_HANDLE_RETURN(snapHandle);

  BOOL canContinue = true;

  if (Process32First(snapHandle, &processEntry)) {
    while (canContinue) {
      if (processEntry.th32ParentProcessID == processID) {
        procToBeKilled.push_back(processEntry.th32ProcessID);
      }
      canContinue = Process32Next(snapHandle, &processEntry);
    }
  }
  procToBeKilled.push_back(processID);

  for (auto kill : procToBeKilled) {
    processChildHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, kill);

    CHECK_HANDLE_RETURN(processChildHandle)

    CHECK_RETURN(TerminateProcess(processChildHandle, 1))
  }
#elif __linux__
  if (!child) {
    CHECK_RETURN_LINUX(kill(processID, SIGTERM))
    return true;
  }
  DIR *processesDirectory;

  processesDirectory = opendir("/proc");

  if (processesDirectory == NULL) {
    Server::AddCommandInString(strerror(errno), "\n");
    return false;
  }

  std::string currentProcessID;

  while (dirent *directoryEntityProcess = readdir(processesDirectory)) {
    if (directoryEntityProcess->d_type != DT_DIR)
      continue;

    currentProcessID = directoryEntityProcess->d_name;

    if (!isNumber(currentProcessID))
      return false;

    std::string directoryPPID = "/proc/" + currentProcessID + "/stat";

    int fileDescriptor = open(directoryPPID.c_str(), O_RDONLY);

    CHECK_RETURN_LINUX(fileDescriptor)

    char *buffer = new char[BUFFSIZE];

    CHECK_RETURN_LINUX(read(fileDescriptor, buffer, BUFFSIZE))

    currentProcessID = buffer;
    int parentProcessID = getPPID_LINUX(currentProcessID);

    if (parentProcessID == -1) {
      continue;
    }

    currentProcessID = directoryEntityProcess->d_name;
    int intProcessID = 0;

    try {
      intProcessID = stoi(currentProcessID);
    } catch (const std::invalid_argument &e) {
      Server::AddCommandInString(e.what(), "\n");
      continue;

    } catch (const std::out_of_range &e) {
      Server::AddCommandInString(e.what(), "\n");
      continue;
    }
    if (parentProcessID == processID) {
      procToBeKilled.push_back(stoi(currentProcessID));
    }
    CHECK_RETURN_LINUX(close(fileDescriptor))
  }
  CHECK_RETURN_LINUX(closedir(processesDirectory))

  for (auto current : procToBeKilled) {
    CHECK_RETURN_LINUX(kill(current, SIGTERM))
  }

#endif
  Server::AddCommandInString(_T("Kill Process Succes!\n"));
  return true;
}

#ifdef __linux__
int KillProc::getPPID_LINUX(std::string processInfo) {
  int space = 0;

  std::string parentProcessID;
  int intParentProcessID;
  for (size_t index = 0; index < processInfo.size(); index++) {
    if (processInfo[index] == ' ') {
      space++;
      continue;
    }
    if (space == 4)
      break;
    if (space == 3) {
      parentProcessID = parentProcessID + processInfo.at(index);
    }
  }
  try {
    intParentProcessID = stoi(parentProcessID);
  } catch (const std::invalid_argument &e) {
    Server::AddCommandInString(e.what(), "\n");
    return -1;
  } catch (const std::out_of_range &e) {
    Server::AddCommandInString(e.what(), "\n");
    return -1;
  }
  return intParentProcessID;
}
#endif
