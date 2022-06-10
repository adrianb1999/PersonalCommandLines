#ifndef _DELREBOOT_H_
#define _DELREBOOT_H_

#include "../Command.h"
#include <string>
class Delreboot : public Command {
private:
  tstring path;
  CSOCKET clientSocket;

public:
  Delreboot(const tstring &path, const CSOCKET &clientSocket);
  bool execute();
};

#endif
