#ifndef _READ_H_
#define _READ_H_

#include "../Command.h"
#include <string>

class Read : public Command {
private:
  tstring path;
  int position;
  int noOfBytes;
  bool byteString;
  CSOCKET clientSocket;

public:
  Read(const tstring &path, const int &position, const int &noOfBytes,
       const bool &byteString, const CSOCKET &clientSocket);
  bool execute();
};

#endif
