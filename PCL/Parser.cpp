#include "Parser.h"
#include "Commands/Command.h"
#include "Commands/Commons/Help.h"
#include "Commands/Commons/Quit.h"
#include "Commands/Files/Cd.h"
#include "Commands/Files/Chm.h"
#include "Commands/Files/Cpy.h"
#include "Commands/Files/Del.h"
#include "Commands/Files/Dir.h"
#include "Commands/Files/File.h"
#include "Commands/Files/Move.h"
#include "Commands/Files/Pwd.h"
#include "Commands/Files/Read.h"
#include "Commands/Files/Write.h"
#include "Commands/Processes/CreateProc.h"
#include "Commands/Processes/KillProc.h"
#include "Commands/Processes/ListProcs.h"
#include "Commands/Processes/SuspendProc.h"
#ifdef _WIN32
#include "Commands/Files/Delreboot.h"
#include "Commands/Processes/CritProc.h"
#include "Commands/Processes/UnCritProc.h"
#include "Commands/Registry/CreateKey.h"
#include "Commands/Registry/CreateValue.h"
#include "Commands/Registry/DeleteKey.h"
#include "Commands/Registry/DeleteValue.h"
#include "Commands/Registry/GetKey.h"
#include "Commands/Registry/RegDir.h"
#include "Commands/Registry/SetData.h"
#endif
#include <iostream>

#define PWD _T("pwd")
#define CD _T("cd")
#define DEL _T("del")
#define FILE _T("file")
#define DELREBOOT _T("delreboot")
#define HELP _T("help")
#define READ _T("read")
#define WRITE _T("write")
#define DIR _T("dir")
#define CHM _T("chm")
#define CPY _T("cpy")
#define MOVE _T("move")
#define QUIT _T("quit")
#define GETKEY _T("getKey")
#define CREATEKEY _T("createKey")
#define DELETEKEY _T("deleteKey")
#define CREATEVALUE _T("createValue")
#define DELETEVALUE _T("deleteValue")
#define SETDATA _T("setData")
#define REGDIR _T("regDir")
#define CREATEPROC _T("createProc")
#define KILLPROC _T("killProc")
#define CRITPROC _T("critProc")
#define UNCRITPROC _T("unCritProc")
#define LISTPROCS _T("listProcs")
#define SUSPENDPROC _T("suspendProc")
#define QUITSERVER _T("quitserver")

Parser::Parser() {}

bool Parser::splitCommand(std::vector<tstring> &commandList,
                          tstring &commands) {

  tstring individualCommand = _T("");
  bool quotationMarkIsActive = false;

  for (auto command : commands) {
    if (command == _T('\"')) {
      if (!quotationMarkIsActive) {
        quotationMarkIsActive = true;
        continue;
      } else {
        quotationMarkIsActive = false;
        commandList.push_back(individualCommand);
        individualCommand.clear();
        continue;
      }
    }
    if (quotationMarkIsActive) {
      individualCommand = individualCommand + command;
      continue;
    } else if (command != _T(' ')) {
      individualCommand = individualCommand + command;
    } else if (individualCommand != _T("")) {
      commandList.push_back(individualCommand);
      individualCommand.clear();
    }
  }
  if (quotationMarkIsActive) {
    tcout << _T("Invalid input\n");
    return false;
  }
  if (individualCommand[0] != _T('\"'))
    commandList.push_back(individualCommand);
  return true;
}

bool Parser::parse(tstring &commands, CSOCKET &currentClient) {
  std::vector<tstring> commandList;
  if (!splitCommand(commandList, commands))
    return false;

  size_t commandListSize = commandList.size();

  for (size_t index = 0; index < commandListSize; index++) {

    // PRINT WORKING DIRECTORY
    if (commandList[index] == PWD) {
      commandsToExecute.push_back(new Pwd(currentClient));

      // CHANGE DIRECTORY
    } else if (commandList[index] == CD) {
      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()
      commandsToExecute.push_back(new Cd(commandList[++index], currentClient));

      // CREATE FILE
    } else if (commandList[index] == FILE) {
      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()
      commandsToExecute.push_back(
          new File(commandList[++index], currentClient));

      // DELETE FILE
    } else if (commandList[index] == DEL) {
      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()
      commandsToExecute.push_back(new Del(commandList[++index], currentClient));

      // READ FILE
    } else if (commandList[index] == READ) {
      if (index + 3 >= commandListSize)
        RETURN_INVALID_COMMAND()

      tstring readFileName = commandList[++index];
      int readPosition;
      int readNrOfBytes;

      try {
        readPosition = stoi(commandList[++index]);
        readNrOfBytes = stoi(commandList[++index]);

      } catch (const std::invalid_argument &e) {

        RETURN_EXCEPTION()

      } catch (const std::out_of_range &e) {

        RETURN_EXCEPTION()
      }

      if (index + 1 >= commandList.size()) {
        commandsToExecute.push_back(new Read(
            readFileName, readPosition, readNrOfBytes, false, currentClient));
        continue;
      }
      if (commandList[index + 1] == _T("-b")) {
        index++;
        commandsToExecute.push_back(new Read(
            readFileName, readPosition, readNrOfBytes, true, currentClient));
      } else {
        commandsToExecute.push_back(new Read(
            readFileName, readPosition, readNrOfBytes, false, currentClient));
      }

      // WRITE IN FILE
    } else if (commandList[index] == WRITE) {
      if (index + 2 >= commandListSize)
        RETURN_INVALID_COMMAND()

      tstring fileName = commandList[++index];
      tstring stringToWrite = commandList[++index];
      int positionToWrite = 0;

      if (index + 1 >= commandList.size()) {
        commandsToExecute.push_back(
            new Write(fileName, stringToWrite, positionToWrite, currentClient));
        continue;
      }

      if (commandList[index + 1][0] <= 57 && commandList[index + 1][0] >= 48) {
        try {
          positionToWrite = stoi(commandList[++index]);

        } catch (const std::invalid_argument &e) {

          RETURN_EXCEPTION()

        } catch (const std::out_of_range &e) {

          RETURN_EXCEPTION()
        }
        commandsToExecute.push_back(
            new Write(fileName, stringToWrite, positionToWrite, currentClient));
      } else
        commandsToExecute.push_back(
            new Write(fileName, stringToWrite, positionToWrite, currentClient));

      // PRINT ALL DIR
    } else if (commandList[index] == DIR) {
      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()

      if (index + 2 >= commandListSize) {
        commandsToExecute.push_back(
            new Dir(commandList[++index], 0, false, currentClient));
        continue;
      }
      if (commandList[index + 2] == _T("-r")) {
        commandsToExecute.push_back(
            new Dir(commandList[++index], 0, true, currentClient));
        index++;
      } else
        commandsToExecute.push_back(
            new Dir(commandList[++index], 0, false, currentClient));

      // SET FILE FLAGS
    } else if (commandList[index] == CHM) {
      if (index + 2 >= commandListSize)
        RETURN_INVALID_COMMAND()

      tstring fileName = commandList[++index];

      commandsToExecute.push_back(
          new Chm(fileName, commandList[++index], currentClient));

      // COPY FILE
    } else if (commandList[index] == CPY) {
      if (index + 2 >= commandListSize)
        RETURN_INVALID_COMMAND()

      tstring initialPath = commandList[++index];
      tstring finalPath = commandList[++index];

      commandsToExecute.push_back(
          new Cpy(initialPath, finalPath, currentClient));

      // MOVE FILE
    } else if (commandList[index] == MOVE) {
      if (index + 2 >= commandListSize)
        RETURN_INVALID_COMMAND()

      tstring initialPath = commandList[++index];
      tstring finalPath = commandList[++index];

      commandsToExecute.push_back(
          new Move(initialPath, finalPath, currentClient));

      // HELP COMMAND
    } else if (commandList[index] == HELP) {
      commandsToExecute.push_back(new Help());

      // QUIT COMMAND
    } else if (lowerString(commandList[index]) == QUIT) {

      commandsToExecute.push_back(new Quit(false));

    } else if (lowerString(commandList[index]) == QUITSERVER) {

      commandsToExecute.push_back(new Quit(true));

      // Criticalize PROCESS
    } else if (commandList[index] == KILLPROC) {
      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()
      int pid;
      try {
        pid = stoi(commandList[++index]);

      } catch (const std::invalid_argument &e) {

        RETURN_EXCEPTION()

      } catch (const std::out_of_range &e) {

        RETURN_EXCEPTION()
      }
      if (index + 1 >= commandListSize) {
        commandsToExecute.push_back(new KillProc(pid, false));
        continue;
      }
      if (commandList[index + 1] == _T("-c")) {
        index++;
        commandsToExecute.push_back(new KillProc(pid, true));
      } else
        commandsToExecute.push_back(new KillProc(pid, false));

      // SUSPEND PROCESS
    } else if (commandList[index] == SUSPENDPROC) {
      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()
      int pid;
      try {
        pid = stoi(commandList[++index]);

      } catch (const std::invalid_argument &e) {

        RETURN_EXCEPTION()

      } catch (const std::out_of_range &e) {

        RETURN_EXCEPTION()
      }
      if (index + 1 >= commandListSize) {
        commandsToExecute.push_back(new SuspendProc(pid, false));
        continue;
      }
      if (commandList[index + 1] == _T("-c")) {
        index++;
        commandsToExecute.push_back(new SuspendProc(pid, true));
      } else
        commandsToExecute.push_back(new SuspendProc(pid, false));

      // LIST ALL PROCESSES
    } else if (commandList[index] == LISTPROCS) {
      if (index + 1 >= commandListSize) {
        commandsToExecute.push_back(new ListProcs(false));
        continue;
      } else if (commandList[index + 1] == _T("-tree")) {
        commandsToExecute.push_back(new ListProcs(true));
        continue;
      }
      commandsToExecute.push_back(new ListProcs(false));

      // CREATE PROCESS
    } else if (commandList[index] == CREATEPROC) {
      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()
      commandsToExecute.push_back(
          new CreateProc(commandList[++index], currentClient));
    }
#ifdef _WIN32
    // DEL REBOOT
    else if (commandList[index] == DELREBOOT) {
      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()
      commandsToExecute.push_back(
          new Delreboot(commandList[++index], currentClient));
    }
    // GET REGISTRY KEY
    else if (commandList[index] == GETKEY) {

      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()
      commandsToExecute.push_back(new GetKey(commandList[++index]));

      // CREATE REGISTRY KEY
    } else if (commandList[index] == CREATEKEY) {
      if (index + 2 >= commandListSize)
        RETURN_INVALID_COMMAND()
      tstring path = commandList[++index];
      commandsToExecute.push_back(new CreateKey(path, commandList[++index]));

      // DELETE REGISTRY KEY
    } else if (commandList[index] == DELETEKEY) {

      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()
      commandsToExecute.push_back(new DeleteKey(commandList[++index]));

      // CREATE REGISTRY VALUE
    } else if (commandList[index] == CREATEVALUE) {

      if (index + 3 >= commandListSize)
        RETURN_INVALID_COMMAND()

      tstring regPath = commandList[++index];
      tstring regValueName = commandList[++index];
      tstring regValueType = commandList[++index];
      tstring regData = _T("");
      if (index + 1 < commandListSize) {
        regData = commandList[++index];
      }
      commandsToExecute.push_back(
          new CreateValue(regPath, regValueName, regValueType, regData));

      // DELETE REGISTRY VALUE
    } else if (commandList[index] == DELETEVALUE) {
      if (index + 2 >= commandListSize)
        RETURN_INVALID_COMMAND()
      tstring regPath = commandList[++index];
      tstring regValueName = commandList[++index];
      commandsToExecute.push_back(new DeleteValue(regPath, regValueName));

      // SET REGISTRY DATA
    } else if (commandList[index] == SETDATA) {

      if (index + 3 >= commandListSize)
        RETURN_INVALID_COMMAND()

      tstring regPath = commandList[++index];
      tstring regValueName = commandList[++index];
      tstring regNewData = commandList[++index];
      commandsToExecute.push_back(
          new SetData(regPath, regValueName, regNewData));

      // REGISTRY DIR
    } else if (commandList[index] == REGDIR) {
      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()
      commandsToExecute.push_back(new RegDir(commandList[++index]));

      // CRITICALIZE PROCESS
    } else if (commandList[index] == CRITPROC) {
      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()

      DWORD pid;
      try {
        pid = stoi(commandList[++index]);

      } catch (const std::invalid_argument &e) {

        RETURN_EXCEPTION()

      } catch (const std::out_of_range &e) {

        RETURN_EXCEPTION()
      }

      commandsToExecute.push_back(new CritProc(pid));

      // UNCRITICALIZE PROCESS
    } else if (commandList[index] == UNCRITPROC) {
      if (index + 1 >= commandListSize)
        RETURN_INVALID_COMMAND()

      DWORD pid;
      try {
        pid = stoi(commandList[++index]);

      } catch (const std::invalid_argument &e) {

        RETURN_EXCEPTION()

      } catch (const std::out_of_range &e) {

        RETURN_EXCEPTION()
      }
      commandsToExecute.push_back(new UnCritProc(pid));
    }
#endif
    else {
      if (commandList[index] != _T(""))
        Server::AddCommandInString(_T("Invalid command!\n"));
    }
  }
  execute();
  commands.clear();
  return true;
}

bool Parser::execute() {
  for (auto &cmd : commandsToExecute) {
    if (!cmd->execute()) {
      delete cmd;
      commandsToExecute.clear();
      return false;
    }
    delete cmd;
  }
  commandsToExecute.clear();
  return true;
}
