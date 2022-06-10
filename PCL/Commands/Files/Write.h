#ifndef _WRITE_H_
#define _WRITE_H_

#include "../Command.h"
#include <string>

class Write : public Command {
private:
  tstring path;
  tstring textToInsert;
  int positionToInsert;
  CSOCKET clientSocket;

public:
  Write(const tstring &path, const tstring &textToInsert,
        const int &positionToInsert, const CSOCKET &clientSocket);
  bool execute();
};

#endif
