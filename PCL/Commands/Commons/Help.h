#ifndef _HELP_H_
#define _HELP_H_

#include "../Command.h"
#include <string>
#include <vector>

class Help : public Command {
public:
  bool execute();
};

#endif