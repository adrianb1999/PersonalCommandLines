#ifndef _UNCRITPROC_H_
#define _UNCRITPROC_H_

#include "../Command.h"
class UnCritProc : public Command {
private:
  DWORD processID;

public:
  UnCritProc(const DWORD &processID);
  bool execute();
};

#endif
