#ifndef _GETKEY_H_
#define _GETKEY_H_

#include "../Command.h"
#include <Windows.h>

class GetKey : public Command {
private:
  tstring path;

public:
  bool execute();
  GetKey(const tstring &path);
};

#endif