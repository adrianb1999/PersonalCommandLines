#ifndef _PARSER_H_
#define _PARSER_H_

#include "Commands/Command.h"
#include <string>
#include <vector>

class Parser {
private:
  std::vector<Command *> commandsToExecute;
  bool splitCommand(std::vector<tstring> &, tstring &);
  bool execute();
public:
  bool parse(tstring &commands, CSOCKET &currentClient);
  Parser();
};

#endif
