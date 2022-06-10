#ifndef _KILLPROC_H_
#define _KILLPROC_H_

#include "../Command.h"
#include <map>
#include <vector>

class KillProc : public Command {
private:
#ifdef _WIN32
  DWORD processID;
#elif __linux__
  int processID;
  int getPPID_LINUX(std::string);
#endif
  bool child;

public:
#ifdef _WIN32
  KillProc(const DWORD &processID, const bool &child);
#elif __linux__
  KillProc(const int &processID, const bool &child);
#endif
  bool execute();
};

#endif
