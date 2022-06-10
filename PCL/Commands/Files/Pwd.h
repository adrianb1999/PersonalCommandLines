#ifndef _PWD_H_
#define _PWD_H_

#include "../Command.h"
class Pwd : public Command {
private:
  CSOCKET clientSocket;

public:
  Pwd(const CSOCKET &clientSocket);
  bool execute();
};

#endif
