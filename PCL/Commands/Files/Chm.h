#ifndef _CHM_H_
#define _CHM_H_

#include "../Command.h"
#include <string>
class Chm : public Command {
private:
  tstring path;
  tstring flag;
  CSOCKET clientSocket;

public:
  Chm(const tstring &path, const tstring &flag, const CSOCKET &clientSocket);
  bool execute();
};

#endif
