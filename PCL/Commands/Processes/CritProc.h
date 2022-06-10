#ifndef _CRITPROC_H_
#define _CRITPROC_H_

#include "../Command.h"
class CritProc : public Command {
private:
  DWORD processID;
public:
  CritProc(const DWORD &processID);
  bool execute();
};

#endif