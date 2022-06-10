#ifndef _DIR_H_
#define _DIR_H_

#include "../Command.h"

class Dir : public Command {
private:
  tstring path;
  bool flag;
  CSOCKET clientSocket;

#ifdef _WIN32
  DWORD space;
  void showDir(WIN32_FIND_DATA currentFileData, HANDLE fileHandle,
               LPCTSTR currentFileName, DWORD &spaceToPrint);
#elif __linux__
  int space;
  void showDir(std::string currentPath, int space);
#endif
public:
#ifdef _WIN32
  Dir(const tstring &path, const DWORD &space, const bool &flag,
      const SOCKET &client);
#elif __linux__
  Dir(const tstring &path, const int &space, const bool &flag,
      const int &client);
#endif
  ~Dir();
  bool execute();
};

#endif
