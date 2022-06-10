#ifndef _CPY_H_
#define _CPY_H_

#include "../Command.h"
#include <string>

class Cpy : public Command {
private:
  tstring initialPath;
  tstring destinationPath;
  CSOCKET clientSocket;
#ifdef _WIN32
  void recCopy(WIN32_FIND_DATA currentFileData, HANDLE fileHandle,
               LPCTSTR currentFileName, tstring newDestinationPath);
#endif
public:
  Cpy(const tstring &initialPath, const tstring &destinationPath,
      const CSOCKET &clientSocket);
  bool execute();
};

#endif
