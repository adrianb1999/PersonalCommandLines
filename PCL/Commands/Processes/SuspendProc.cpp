#include "SuspendProc.h"
#ifdef _WIN32
#include <TlHelp32.h>
SuspendProc::SuspendProc(const DWORD &processID, const bool &child)
    : processID{processID}, child{child} {}

#elif __linux__
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

SuspendProc::SuspendProc(const int & processID, const bool &child)
    : processID{ processID }, child{child} {}
#endif

bool SuspendProc::execute() {

  std::vector<int> procToBeSuspended;

#ifdef _WIN32
  if (!child) {

    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

    std::unique_ptr<HANDLE, void (*)(HANDLE *)> hProc_ptr(&processHandle,
                                                          CloseHandleDeleter);
    CHECK_HANDLE_RETURN(processHandle)
    if (SuspendThread(processHandle) == -1) {
      Server::AddCommandInString(GetLastErrorAsString(), _T("\n"));
      return false;
    }
    return true;
  }

  PROCESSENTRY32 processEntry;
  memset(&processEntry, 0, sizeof(PROCESSENTRY32));
  processEntry.dwSize = sizeof(PROCESSENTRY32);

  HANDLE processChildHandle = INVALID_HANDLE_VALUE;

  std::unique_ptr<HANDLE, void (*)(HANDLE *)> processChildHandlePointer(
      &processChildHandle, CloseHandleDeleter);

  HANDLE snapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  std::unique_ptr<HANDLE, void (*)(HANDLE *)> hSnap_ptr(&snapHandle,
                                                        CloseHandleDeleter);

  CHECK_HANDLE_RETURN(snapHandle);

  BOOL canContinue = true;

  if (Process32First(snapHandle, &processEntry)) {
    while (canContinue) {
      if (processEntry.th32ParentProcessID == processID) {
        procToBeSuspended.push_back(processEntry.th32ProcessID);
      }
      canContinue = Process32Next(snapHandle, &processEntry);
    }
  }

  procToBeSuspended.push_back(processID);

  for (auto kill : procToBeSuspended) {
    processChildHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, kill);

    CHECK_HANDLE_RETURN(processChildHandle)

    if (SuspendThread(processChildHandle) == -1) {
      Server::AddCommandInString(GetLastErrorAsString(), _T("\n"));
      return false;
    }
  }
#elif __linux__
  if (!child) {
    CHECK_RETURN_LINUX(kill(processID, SIGSTOP))
    CHECK_RETURN_LINUX(kill(processID, SIGCONT))

    return true;
  }
  DIR *newDirectory;

  newDirectory = opendir("/proc");

  if (newDirectory == NULL) {
    Server::AddCommandInString(strerror(errno), "\n");
    return false;
  }

  std::string currentPID;

  while (dirent *dirp = readdir(newDirectory)) {
    if (dirp->d_type != DT_DIR)
      continue;

    currentPID = dirp->d_name;

    if (!isNumber(currentPID))
      return false;

    std::string directoryPPID = "/proc/" + currentPID + "/stat";

    int fileDescriptor = open(directoryPPID.c_str(), O_RDONLY);

    CHECK_RETURN_LINUX(fileDescriptor)

    char *lpBuffer = new char[255];

    CHECK_RETURN_LINUX(read(fileDescriptor, lpBuffer, 255))

    currentPID = lpBuffer;
    int PPID = getPPID_LINUX(currentPID);

    if (PPID == -1) {
      continue;
    }

    currentPID = dirp->d_name;
    int intPID = 0;

    try {
      intPID = stoi(currentPID);
    } catch (const std::invalid_argument &e) {
      Server::AddCommandInString(e.what(), "\n");
      continue;
    } catch (const std::out_of_range &e) {
      Server::AddCommandInString(e.what(), "\n");
      continue;
    }

    if (PPID == processID) {
      procToBeSuspended.push_back(stoi(currentPID));
    }
    CHECK_RETURN_LINUX(close(fileDescriptor))
  }
  CHECK_RETURN_LINUX(closedir(newDirectory))

  for (auto current : procToBeSuspended) {
    CHECK_RETURN_LINUX(kill(current, SIGSTOP))
    CHECK_RETURN_LINUX(kill(current, SIGCONT))
  }

#endif
  Server::AddCommandInString(_T("Suspend Process Succes!\n"));
  return true;
}

#ifdef __linux__
int SuspendProc::getPPID_LINUX(std::string processInfo) {
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
