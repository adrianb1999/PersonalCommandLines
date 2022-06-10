#ifndef _CREATEPROC_H_
#define _CREATEPROC_H_

#include "../Command.h"

class CreateProc : public Command {
private:
  tstring path;
  CSOCKET clientSocket;

public:
  CreateProc(const tstring &path, const CSOCKET &clientSocket);
  bool execute();
};

#endif
