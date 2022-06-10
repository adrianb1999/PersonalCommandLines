#include "Read.h"

#ifdef _WIN32
#include <Windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#elif __linux__
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <iostream>

Read::Read(const tstring &path, const int &position, const int &noOfBytes,
           const bool &byteString, const CSOCKET &clientSocket)
    : path{path}, position{position}, noOfBytes{noOfBytes},
      byteString{byteString}, clientSocket{clientSocket} {}

bool Read::execute() {

  tstring currentDirectory = Server::GetClientDirectory(clientSocket);

  if (!GetAbsolutePath(currentDirectory, path)) {
    return false;
  }
  path = currentDirectory;

#ifdef _WIN32
  DWORD noOfBytesRead{};
  DWORD fileSize{};

  HANDLE readHandle = CreateFile(path.c_str(), GENERIC_READ, 0, NULL,
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  std::unique_ptr<HANDLE, void (*)(HANDLE *)> readHandlePointer(
      &readHandle, CloseHandleDeleter);

  fileSize = GetFileSize(readHandle, NULL);

  if (fileSize > (DWORD)noOfBytes)
    fileSize = noOfBytes;

  fileSize++;
  char *readBuffer = new char[fileSize];
  fileSize--;
  CHECK_HANDLE_RETURN(readHandle)

  CHECK_SET_FILE_POINTER_RETURN(
      SetFilePointer(readHandle, position, NULL, FILE_BEGIN))

  CHECK_RETURN(ReadFile(readHandle, readBuffer, fileSize, &noOfBytesRead, NULL))

  readBuffer[noOfBytesRead] = _T('\0');

  if (byteString) {
    for (size_t index = 0; index < noOfBytesRead / sizeof(TCHAR); index++) {
      std::cout << ((char *)readBuffer)[index];
    }
    tcout << _T('\n');
  } else {
    tstring output = (LPTSTR)readBuffer;
    output.erase(noOfBytesRead / sizeof(TCHAR), output.size());
    Server::AddCommandInString(output);
    Server::AddCommandInString(_T("\n"));
  }

#elif __linux__
  struct stat sb;
  CHECK_RETURN_LINUX(stat(path.c_str(), &sb))
  int fileDescriptor = open(path.c_str(), O_RDONLY);

  int fileSize = sb.st_size;

  if (fileSize > noOfBytes)
    fileSize = noOfBytes;

  fileSize++;
  char *readBuffer = new char[fileSize];
  memset(readBuffer, 0, fileSize);
  fileSize--;

  CHECK_RETURN_LINUX(fileDescriptor)
  CHECK_RETURN_LINUX(read(fileDescriptor, readBuffer, fileSize))
  CHECK_RETURN_LINUX(close(fileDescriptor))

  if (byteString) {
    for (size_t index = 0; index < fileSize; index++) {
      tcout << ((char *)readBuffer)[index];
    }
    tcout << _T('\n');
  } else {
    tstring output = (char *)readBuffer;
    Server::AddCommandInString(output, _T("\n"));
  }
#endif
  delete[] readBuffer;
  return true;
}
