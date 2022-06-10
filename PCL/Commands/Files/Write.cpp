#include "Write.h"

#ifdef _WIN32
#include <Windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#elif __linux__
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#endif

#include <iostream>

Write::Write(const tstring &path, const tstring &textToInsert,
             const int &positionToInsert, const CSOCKET &clientSocket)
    : path{path}, textToInsert{textToInsert},
      positionToInsert{positionToInsert}, clientSocket{clientSocket} {}

bool Write::execute() {

  tstring currentDirectory = Server::GetClientDirectory(clientSocket);

  if (!GetAbsolutePath(currentDirectory, path)) {
    return false;
  }
  path = currentDirectory;

#ifdef _WIN32
  DWORD noOfBytesReaded{};
  size_t fileSize = textToInsert.size() * sizeof(TCHAR);

  // READ
  HANDLE readHandle =
      CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  std::unique_ptr<HANDLE, void (*)(HANDLE *)> readHandlePointer(
      &readHandle, CloseHandleDeleter);

  CHECK_HANDLE_RETURN(readHandle)

  DWORD readFileSize = 0;

  readFileSize = GetFileSize(readHandle, NULL);

  TCHAR *readBuffer = new TCHAR[readFileSize];

  CHECK_SET_FILE_POINTER_RETURN(
      SetFilePointer(readHandle, (LONG)positionToInsert, NULL, FILE_BEGIN))

  CHECK_RETURN(
      ReadFile(readHandle, readBuffer, readFileSize, &noOfBytesReaded, NULL))

  // WRITE
  CHECK_SET_FILE_POINTER_RETURN(
      SetFilePointer(readHandle, (LONG)positionToInsert, NULL, FILE_BEGIN))

  CHECK_RETURN(WriteFile(readHandle, readBuffer, noOfBytesReaded, &noOfBytesReaded, NULL))

  DWORD writePointer = (DWORD)positionToInsert + noOfBytesReaded;

  CHECK_SET_FILE_POINTER_RETURN(
      SetFilePointer(readHandle, (LONG)writePointer, NULL, FILE_BEGIN))

  CHECK_RETURN(WriteFile(readHandle, textToInsert.c_str(),
                         (DWORD)(textToInsert.size() * sizeof(TCHAR)),
                         (LPDWORD)&fileSize, NULL));

#elif __linux__
  int fileDescriptor = open(path.c_str(), O_WRONLY);
  CHECK_RETURN_LINUX(fileDescriptor)
  CHECK_RETURN_LINUX(
      write(fileDescriptor, (void *)textToInsert.c_str(), textToInsert.size()))

#endif
  Server::AddCommandInString(_T("Write succes!\n"));
  return true;
}
