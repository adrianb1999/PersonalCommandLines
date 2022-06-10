#ifndef _SETDATA_H_
#define _SETDATA_H_

#include "../Command.h"
#include <string>
class SetData : public Command {
private:
  tstring path;
  tstring valueName;
  tstring valueNewData;

public:
  SetData(const tstring &path, const tstring &valueName, const tstring &valueNewData);
  bool execute();
};

#endif