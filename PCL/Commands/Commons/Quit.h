#ifndef _QUIT_H_
#define _QUIT_H_

#include "../Command.h"
class Quit : public Command {
  bool killServer;
  static bool quitState;
  static bool quitServer;

public:
  Quit(const bool &killServer);
  bool execute();
  static bool canQuit();
  static bool canCloseServer();
};

#endif