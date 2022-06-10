#ifndef _DEL_H_
#define _DEL_H_

#include "../Command.h"

class Del : public Command {
private:
  tstring path;
  CSOCKET clientSocket;

public:
  Del(const tstring &path, const CSOCKET &clientSocket);
  bool execute();
};

#endif
