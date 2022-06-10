#ifndef _FILE_H_
#define _FILE_H_

#include "../Command.h"
#include <string>

class File : public Command {
private:
  tstring path;
  CSOCKET clientSocket;

public:
  File(const tstring &path, const CSOCKET &clientSocket);
  bool execute();
};

#endif
