#include "Cpy.h"
#ifdef _WIN32
#include <Windows.h>
#include <memory>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#elif __linux__
#include <fcntl.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <iostream>

Cpy::Cpy(const tstring &initialPath, const tstring &destinationPath,
         const CSOCKET &clientSocket)
    : initialPath{initialPath}, destinationPath{destinationPath},
      clientSocket{clientSocket} {}

bool Cpy::execute() {

  tstring currentDirectory = Server::GetClientDirectory(clientSocket);

  if (!GetAbsolutePath(currentDirectory, initialPath)) {
    return false;
  }

  initialPath = currentDirectory;

#ifdef _WIN32
  CHECK_FILE_RETURN(GetFileAttributes(initialPath.c_str()))

  if (GetFileAttributes(initialPath.c_str()) != FILE_ATTRIBUTE_DIRECTORY) {
    CHECK_RETURN(CopyFile(initialPath.c_str(), destinationPath.c_str(), FALSE))
    return true;
  }

  if (!PathFileExists(destinationPath.c_str()))
    if (!CreateDirectory(destinationPath.c_str(), NULL))
      return false;

  tstring newInitialPath = initialPath + _T("\\*");

  WIN32_FIND_DATA currentFileData{};

  HANDLE fileHandle = INVALID_HANDLE_VALUE;

  recCopy(currentFileData, fileHandle, newInitialPath.c_str(), destinationPath);

#elif __linux__
  int outFileDescriptor = open(destinationPath.c_str(), O_CREAT);

  CHECK_RETURN_LINUX(outFileDescriptor)

  int inFileDescriptor = open(initialPath.c_str(), O_RDWR);

  CHECK_RETURN_LINUX(inFileDescriptor)

  CHECK_RETURN_LINUX(sendfile(outFileDescriptor, inFileDescriptor, NULL,
                              sizeof(initialPath)))
#endif

  Server::AddCommandInString(_T("Copy succes!\n"));
  return true;
}

#ifdef _WIN32
void Cpy::recCopy(WIN32_FIND_DATA currentFileData, HANDLE fileHandle,
                  LPCTSTR currentFileName, tstring newDestinationPath) {

  fileHandle = FindFirstFile(currentFileName, &currentFileData);
  do {
    if (currentFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      tstring fileName = currentFileData.cFileName;
      if (fileName == _T(".") || fileName == _T(".."))
        continue;

      tstring newDirectory =
          newDestinationPath + _T("\\") + currentFileData.cFileName;

      if (!PathFileExists(newDirectory.c_str()))
        if (!CreateDirectory(newDirectory.c_str(), NULL))
          continue;

      tstring initialFileName = currentFileName;
      initialFileName.pop_back();
      initialFileName = initialFileName + currentFileData.cFileName + _T("\\*");

      tstring fileDestination = newDestinationPath;
      fileDestination = fileDestination + _T("\\") + currentFileData.cFileName;

      recCopy(currentFileData, fileHandle, initialFileName.c_str(),
              fileDestination);
      continue;
    }

    tstring fileName = currentFileName;
    fileName.pop_back();
    fileName = fileName + currentFileData.cFileName;

    tstring fileDestination = newDestinationPath;
    fileDestination = fileDestination + _T("\\") + currentFileData.cFileName;

    CopyFile(fileName.c_str(), fileDestination.c_str(), FALSE);

  } while (FindNextFile(fileHandle, &currentFileData) != 0);
}
#endif
