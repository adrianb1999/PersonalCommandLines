#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "../Utils/Utils.h"

// std::queue<tstring> commandsToBeSend;

class Command {
public:
 // static std::queue<tstring> commandsToBeSend;
  virtual bool execute() = 0;
  virtual ~Command();
};

#endif
