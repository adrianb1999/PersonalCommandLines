#ifndef _CREATEKEY_H_
#define _CREATEKEY_H_

#include "../Command.h"
#include <Windows.h>

class CreateKey : public Command {
private:
  tstring path;
  tstring subKeyName;

public:
  CreateKey(const tstring &path, const tstring &subKeyName);
  bool execute();
};

#endif