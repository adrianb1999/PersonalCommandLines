#ifndef _SUSPENDPROC_H_
#define _SUSPENDPROC_H_

#include "../Command.h"
#include <map>
#include <vector>

class SuspendProc : public Command {
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
  SuspendProc(const DWORD &processID, const bool &child);
#elif __linux__
  SuspendProc(const int &processID, const bool &child);
#endif
  bool execute();
};

#endif
