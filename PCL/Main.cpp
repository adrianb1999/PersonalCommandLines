#include "Commands/Commons/Quit.h"
#include "Parser.h"
#include "Utils/Utils.h"
#include <iostream>
#include <thread>
#include <vector>

#include "Network/Server.h"

#ifdef __linux__
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define BUFFSIZE 1024

static tstring initialWorkingDirectory;
static int listeningSocket;

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#elif __linux__
void sig_handler(int singum) {
  Server::EmptyString();
  initialWorkingDirectory.clear();
  close(listeningSocket);
}

#endif

int main() {

  TCHAR path[BUFFSIZE] = {};
  tstring mode;
  tcout << "1- server\n0- local\n";
  getline(tcin, mode);

#ifdef _WIN32
  CHECK_RETURN(GetCurrentDirectory(sizeof(path), path))
#elif __linux__
  signal(SIGINT, sig_handler);
  if (getcwd(path, 1024) == NULL) {
    std::cout << strerror(errno) << '\n';
    return false;
  }
#endif

  initialWorkingDirectory = path;

  if (mode == _T("1")) {
    tstring port, address;
    tcout << "Server mode!\nPort: ";
    getline(tcin, port);
    tcout << "Adress: ";
    getline(tcin, address);
    Server server(stoi(port), address);

#ifdef _WIN32
    SOCKET listening;

    if (!server.StartServer(listening)) {
      return 0;
    }

    while (!Quit::canCloseServer()) {
      sockaddr_in client;
      int clientSize = sizeof(client);

      SOCKET clientSocket = accept(listening, (sockaddr *)&client, &clientSize);

      if (clientSocket == INVALID_SOCKET) {
        server.CloseServer(listening);
        return 0;
      }
      Server::AddClientInList(clientSocket, initialWorkingDirectory);

      HANDLE newClientThread =
          CreateThread(NULL, 0, Server::ClientFunction, (LPVOID)clientSocket, 0, 0);

      if (newClientThread == NULL) {
        continue;
      }
      CloseHandle(newClientThread);
    }
    server.CloseServer(listening);

#elif __linux__

    int listening;
    if (!server.StartServer(listening)) {
      return 0;
    }
    while (!Quit::canCloseServer()) {
      sockaddr_in client;
      socklen_t clientSize = sizeof(client);

      int clientSocket = accept(listening, (sockaddr *)&client, &clientSize);
      if (clientSocket == -1) {
        server.CloseServer(clientSocket);
        return 0;
      }

      int *clientAddres = new int;
      *clientAddres = clientSocket;

      Server::AddClientInList(*clientAddres, initialWorkingDirectory);

      pthread_t clientThread{};
      pthread_create(&clientThread, NULL, Server::ClientFunctionLinux,
                     (void *)clientAddres);
      pthread_detach(clientThread);
    }
#endif
    return 0;
  }

  if (mode != _T("0")) {
    tcout << _T("Invalid input!");
    return 0;
  }
  tcout << "Local machine!\n";

  tstring commands{};
  Parser pObj;
  CSOCKET client{};

  Server::AddClientInList(client, initialWorkingDirectory);

  while (!Quit::canQuit()) {
    getline(tcin, commands);
    pObj.parse(commands, client);
    tcout << Server::GetList();
    Server::EmptyString();
  }
  return 0;
}
