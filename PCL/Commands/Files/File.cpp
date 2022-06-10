#include "File.h"
#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "../../Network/Server.h"
#include <iostream>

File::File(const tstring &path, const CSOCKET &clientSocket)
    : path{path}, clientSocket{clientSocket} {}

bool File::execute() {

  tstring currentDirectory = Server::GetClientDirectory(clientSocket);

#ifdef _WIN32
  if (path[0] == _T('.')) {
    path = currentDirectory + _T("\\") + path;
  } else {
    size_t position = path.find(_T('\\'));
    if (position == -1) {
      path = currentDirectory + _T("\\") + path;
    }
  }

  HANDLE fileHandle = CreateFile(path.c_str(), GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                            CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

  std::unique_ptr<HANDLE, void (*)(HANDLE *)> fileHandlePointer(&fileHandle,
                                                        CloseHandleDeleter);
  CHECK_HANDLE_RETURN(fileHandle)

#elif __linux__
  if (path[0] == _T('.')) {
    path = currentDirectory + _T("/") + path;
  } else {
    int position = path.find(_T('/'));
    if (position == -1) {
      path = currentDirectory + _T("/") + path;
    }
  }

  int fileDescriptor;
  fileDescriptor = open(path.c_str(), O_CREAT);

  CHECK_RETURN_LINUX(fileDescriptor)
  CHECK_RETURN_LINUX(close(fileDescriptor))

#endif

  path = _T("File ") + path + _T(" created!\n");

  Server::AddCommandInString(path);

  return true;
}
