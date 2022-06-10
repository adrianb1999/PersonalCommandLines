#ifndef _DELETEKEY_H_
#define _DELETEKEY_H_

#include "../Command.h"
#include <Windows.h>
#include <string>

class DeleteKey : public Command {
private:
  tstring path;

public:
  DeleteKey(tstring path);
  bool execute();
};

#endif