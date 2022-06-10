#ifndef _SERVER_H_
#define _SERVER_H_

#ifdef _WIN32
#include <tchar.h>
#include <ws2tcpip.h>

#endif

#include <iostream>
#include <map>
#include <queue>
#include <string>

#ifdef _WIN32
#define CSOCKET SOCKET // Cross-Platform Socket
#elif __linux__
#define CSOCKET int // Cross-Platform Socket
#endif

#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

class Server {
private:
  int port;
  tstring address;
  static std::map<CSOCKET, tstring> clientList;
  static tstring resultToBeSend;

public:
  Server(const int &port, const tstring &address);

  static void AddCommandInString(tstring command);
  static void AddCommandInString(tstring command, tstring secondCommand);
  static void EmptyString();
  static tstring GetList();

  static void AddClientInList(CSOCKET, tstring);
  static tstring GetClientDirectory(CSOCKET);
  static void ChangeClientDirectory(CSOCKET, tstring);
  static void DeleteClient(CSOCKET);

//Callback functions
#ifdef _WIN32
  static DWORD WINAPI ClientFunction(LPVOID lpParam);
#elif __linux__
  static void *ClientFunctionLinux(void *lpParam);
#endif
  bool StartServer(CSOCKET &listenSocket);
  bool CloseServer(CSOCKET &sock);
};

#endif
