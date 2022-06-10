#ifndef _DELETEVALUE_H_
#define _DELETEVALUE_H_

#include "../Command.h"
#include <string>

class DeleteValue : public Command {
private:
  tstring path;
  tstring valueName;

public:
  DeleteValue(const tstring &path, const tstring &valueName);
  bool execute();
};

#endif