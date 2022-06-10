#ifndef _MOVE_H_
#define _MOVE_H_

#include "../Command.h"
#include <string>
class Move : public Command {
private:
  tstring existingFile;
  tstring newFile;
  CSOCKET clientSocket;

public:
  Move(const tstring &existingFile, const tstring &newFile,
       const CSOCKET &clientSocket);
  bool execute();
};

#endif
