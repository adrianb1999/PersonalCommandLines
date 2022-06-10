#include "ListProcs.h"

#ifdef _WIN32
#include <Psapi.h>
#include <TlHelp32.h>
#elif __linux__
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define BUFFSIZE 1024

ListProcs::ListProcs(const bool &tree) : tree{tree} {}

bool ListProcs::execute() {
  std::vector<int> eraseList;
  std::map<int, int> listProcs;
#ifdef _WIN32

  if (!tree) {
    DWORD processesList[BUFFSIZE];
    DWORD nrOfBytesReturned;
    DWORD nrOfProcesses;

    CHECK_RETURN(
        EnumProcesses(processesList, sizeof(processesList), &nrOfBytesReturned))

    nrOfProcesses = nrOfBytesReturned / sizeof(DWORD);
    for (size_t i = 0; i < nrOfProcesses; i++) {

      tstring process = to_tstring(processesList[i]);
      Server::AddCommandInString(process, _T("\n"));
    }
    return true;
  }

  PROCESSENTRY32 processEntry;
  memset(&processEntry, 0, sizeof(PROCESSENTRY32));
  processEntry.dwSize = sizeof(PROCESSENTRY32);

  HANDLE snapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  std::unique_ptr<HANDLE, void (*)(HANDLE *)> snapHandlePointer(&snapHandle,
                                                        CloseHandleDeleter);

  BOOL canContinue = true;
  if (Process32First(snapHandle, &processEntry)) {
    while (canContinue) {
      listProcs.insert({processEntry.th32ProcessID, processEntry.th32ParentProcessID});
      canContinue = Process32Next(snapHandle, &processEntry);
    }
  }

#elif __linux__
  DIR *processesDirectory;

  processesDirectory = opendir("/proc");

  if (processesDirectory == NULL) {
    Server::AddCommandInString(strerror(errno), "\n");
    return false;
  }

  std::string processID;

  while (dirent *directoryEntityProcess = readdir(processesDirectory)) {
    if (directoryEntityProcess->d_type != DT_DIR)
      continue;

    processID = directoryEntityProcess->d_name;

    if (!isNumber(processID))
      continue;

    if (!tree) {
      Server::AddCommandInString(processID, "\n");
      continue;
    }

    std::string directoryPPID = "/proc/" + processID + "/stat";

    int fileDescriptor = open(directoryPPID.c_str(), O_RDONLY);

    CHECK_RETURN_LINUX(fileDescriptor)

    char *buffer = new char[BUFFSIZE];
    memset(buffer,0,BUFFSIZE);

    CHECK_RETURN_LINUX(read(fileDescriptor, buffer, BUFFSIZE))

    processID = buffer;
    
    int PPID = getPPID_LINUX(processID);
    
    delete [] buffer;

    if (PPID == -1) {
      continue;
    }

    processID = directoryEntityProcess->d_name;
    int intPID = 0;

    try {
      intPID = stoi(processID);
    } catch (const std::invalid_argument &e) {
      Server::AddCommandInString(e.what(), "\n");
      continue;
    } catch (const std::out_of_range &e) {
      Server::AddCommandInString(e.what(), "\n");
      continue;
    }

    listProcs.insert({stoi(processID), PPID});
    CHECK_RETURN_LINUX(close(fileDescriptor))
  }

  CHECK_RETURN_LINUX(closedir(processesDirectory))

#endif

  crossingTree(listProcs, eraseList);
  listProcs.clear();
  eraseList.clear();
  return true;
}

void ListProcs::recTree(int currentPID, int space, std::map<int, int> &list,
                        std::vector<int> &eraseList) {
  for (auto currentElement : list) {

    if (currentElement.first == 0) {
      eraseList.push_back(currentElement.first);
      continue;
    }
    if (currentElement.second == currentPID) {

      for (int i = 0; i <= space; i++)
        Server::AddCommandInString(_T(" "));

      tstring process = to_tstring(currentElement.first);

      Server::AddCommandInString(process, _T("\n"));

      eraseList.push_back(currentElement.first);
      recTree(currentElement.first, ++space, list, eraseList);
      space--;
    }
  }
}

void ListProcs::crossingTree(std::map<int, int> &list,
                             std::vector<int> &eraseList) {
  int currentPID = 0;
  while (list.size() > 1) {

    auto begining = list.begin();
    currentPID = begining->first;

    if (currentPID != 0)
      getParrent(currentPID, list);

    recTree(currentPID, 0, list, eraseList);

    for (auto eraseCurrent : eraseList) {
      list.erase(eraseCurrent);
    }
  }
}

void ListProcs::getParrent(int &processID, std::map<int, int> list) {
  for (auto currentPID : list) {
    if (currentPID.first == processID) {
      processID = currentPID.second;
      getParrent(processID, list);
      break;
    }
  }
}

#ifdef __linux__
int ListProcs::getPPID_LINUX(std::string processInfo) {
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
