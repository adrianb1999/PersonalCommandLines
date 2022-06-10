#ifndef _CD_H_
#define _CD_H_

#include "../Command.h"

class Cd : public Command {
private:
  tstring path;
  CSOCKET clientSocket;

public:
  Cd(const tstring &path, CSOCKET& clientSocket);
  bool execute();
};

#endif
