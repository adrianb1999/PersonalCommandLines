#include "Dir.h"
#ifdef _WIN32
#include <Windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#elif __linux__
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include "../../Network/Server.h"

#include <iostream>

#ifdef _WIN32
Dir::Dir(const tstring &path, const DWORD &space, const bool &flag,
         const SOCKET &clientSocket)
    : path{path}, space{space}, flag{flag}, clientSocket{clientSocket} {}
#elif __linux__
Dir::Dir(const tstring &path, const int &space, const bool &flag,
         const int &clientSocket)
    : path{path}, space{space}, flag{flag}, clientSocket{clientSocket} {}
#endif
Dir::~Dir() { space = 0; }

bool Dir::execute() {
#ifdef _WIN32

  tstring currentDirectory = Server::GetClientDirectory(clientSocket);

  if (!GetAbsolutePath(currentDirectory, path)) {
    return false;
  }

  path = currentDirectory;

  path = path + _T("\\*");
  WIN32_FIND_DATA currentFileData{};
  HANDLE fileHandle = INVALID_HANDLE_VALUE;

  showDir(currentFileData, fileHandle, path.c_str(), space);
#elif __linux__
  struct stat sb;
  const char *myDir = path.c_str();
  if (stat(myDir, &sb) == -1) {
    Server::AddCommandInString(strerror(errno), "\n");
    return false;
  }
  showDir(path, 0);

#endif
  return true;
}

#ifdef _WIN32
void Dir::showDir(WIN32_FIND_DATA currentFileData, HANDLE fileHandle, LPCTSTR currentFileName,
                  DWORD &spaceToPrint) {

  fileHandle = FindFirstFile(currentFileName, &currentFileData);
  do {
    if (currentFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

      tstring fileName = currentFileData.cFileName;

      if (fileName == _T(".") || fileName == _T(".."))
        continue;

      if (!flag) {
        Server::AddCommandInString(fileName, _T("\n"));
        continue;
      }

      PRINT_SPACE(spaceToPrint)
      Server::AddCommandInString(fileName, _T("\n"));

      tstring subDirectory = currentFileName;
      subDirectory.pop_back();
      subDirectory = subDirectory + fileName + _T("\\*");

      spaceToPrint++;
      showDir(currentFileData, fileHandle, subDirectory.c_str(), spaceToPrint);
      spaceToPrint--;

      continue;
    }

    PRINT_SPACE(spaceToPrint)
    Server::AddCommandInString(currentFileData.cFileName, _T("\n"));

  } while (FindNextFile(fileHandle, &currentFileData) != 0);
}
#elif __linux__
void Dir::showDir(std::string currentPath, int space) {

  DIR *directory = opendir(currentPath.c_str());
  struct stat fileStatus;
  const char *currentDir;
  struct dirent *directoryEntity;

  while ((directoryEntity = readdir(directory)) != NULL) {
    currentDir = directoryEntity->d_name;
    if (strcmp(currentDir, ".") == 0 || strcmp(currentDir, "..") == 0)
      continue;
    for (int i = 0; i < space; i++) {
      Server::AddCommandInString(" ");
    }

    Server::AddCommandInString(currentDir, "\n");
    if (!flag)
      continue;

    std::string subDirectory = currentPath;
    subDirectory = subDirectory + "/" + directoryEntity->d_name;

    if (stat(subDirectory.c_str(), &fileStatus) == -1)
      continue;

    if (fileStatus.st_mode & S_IFDIR) {
      space++;
      showDir(subDirectory, space);
      space--;
    }
  }
  closedir(directory);
}
#endif
