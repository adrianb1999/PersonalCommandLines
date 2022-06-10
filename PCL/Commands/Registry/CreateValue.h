#ifndef _CREATEVALUE_H_
#define _CREATEVALUE_H_

#include "../Command.h"
#include <string>

class CreateValue : public Command {
private:
  tstring path;
  tstring valueName;
  tstring valueType;
  tstring valueData;

public:
  CreateValue(const tstring &path, const tstring &valueName,
              const tstring &valueType, const tstring &valueData);
  bool execute();
};

#endif
