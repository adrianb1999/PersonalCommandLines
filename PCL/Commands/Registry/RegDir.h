#ifndef _REGDIR_H_
#define _REGDIR_H_

#include "../Command.h"
#include <string>
class RegDir : public Command {
private:
  tstring path;

public:
  RegDir(const tstring &path);
  bool execute();
  bool recShow(LSTATUS &returnValue, HKEY handleKey, const LPCTSTR lbSubKey,
               DWORD currentSpace, tstring hKeyString);
};

#endif
