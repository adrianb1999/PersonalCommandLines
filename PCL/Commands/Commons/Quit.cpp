#include "Quit.h"

bool Quit::quitState = false;
bool Quit::quitServer = false;

Quit::Quit(const bool &killServer) : killServer{killServer} {}

bool Quit::canQuit() {
  if (quitState)
    return true;
  return false;
}

bool Quit::execute() {
  if (killServer)
    quitServer = true;
  quitState = true;
  return true;
}

bool Quit::canCloseServer() {
  if (quitServer)
    return true;
  return false;
}