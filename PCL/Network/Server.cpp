#include "Server.h"
#include "../Parser.h"
#include "../Commands/Commons/Quit.h"
#ifdef _WIN32
#elif __linux__
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

tstring Server::resultToBeSend;
std::map<CSOCKET, tstring> Server::clientList;

Server::Server(const int &port, const tstring &address)
    : port{port}, address{address} {}

void Server::AddCommandInString(tstring command) {
  resultToBeSend = resultToBeSend + command;
}

void Server::AddCommandInString(tstring command, tstring secondCommand) {
  resultToBeSend = resultToBeSend + command + secondCommand;
}

void Server::EmptyString() { resultToBeSend.clear(); }

tstring Server::GetList() { return resultToBeSend; }

void Server::AddClientInList(CSOCKET client, tstring clientDirectory) {
  clientList.insert(std::pair<CSOCKET, tstring>(client, clientDirectory));
}

tstring Server::GetClientDirectory(CSOCKET client) {
  return clientList.at(client);
}

void Server::ChangeClientDirectory(CSOCKET client, tstring clientDirectory) {
  clientList.at(client) = clientDirectory;
}

void Server::DeleteClient(CSOCKET client) { clientList.erase(client); }

bool Server::StartServer(CSOCKET &listenSocket) {
#ifdef _WIN32
  WSADATA wsData;

  int wsOk = WSAStartup(MAKEWORD(2, 2), &wsData);
  if (wsOk != 0) {
    std::cerr << "Can't Initialize winsock! Quitting";
    return false;
  }

  // Create a socket
  SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
  if (listening == INVALID_SOCKET) {
    std::cerr << "Can't create a socket! Quitting with error #"
              << WSAGetLastError() << '\n';
    return false;
  }

  // Bind the ip address and port to a socket
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons((u_short)port);

  if (InetPton(AF_INET, address.c_str(), &hint.sin_addr.S_un.S_addr) != 1) {
    std::cerr << "Error #" << WSAGetLastError() << '\n';
    return false;
  }

  if (bind(listening, (sockaddr *)&hint, sizeof(hint)) == SOCKET_ERROR) {
    std::cerr << "Bind error! Quitting with error #" << WSAGetLastError()
              << '\n';
    return false;
  }

  // Tell Winsock the socket is for listening
  if (listen(listening, SOMAXCONN) == SOCKET_ERROR) {
    std::cerr << "Listen error! Quitting with error #" << WSAGetLastError()
              << '\n';
    return false;
  }
#elif __linux__
  // Create a socket
  int listening = socket(AF_INET, SOCK_STREAM, 0);

  if (listening == -1) {
    std::cout << "Can't create a socket! Quitting\n";
    return false;
  }

  // Bind the ip address and port to a socket
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(54000);
  inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

  bind(listening, (sockaddr *)&hint, sizeof(hint));

  // Tell Winsock the socket is for listening
  listen(listening, SOMAXCONN);

  // Wait for a connection
  // sockaddr_in client;
  // socklen_t clientSize = sizeof(client);
#endif
  listenSocket = listening;
  return true;
}

bool Server::CloseServer(CSOCKET &sock) {
#ifdef _WIN32

  closesocket(sock);
  WSACleanup();

#elif __linux__
  if (close(sock) == -1) {
    std::cout << strerror(errno) << '\n';
    return false;
  }
#endif
  return true;
}

#ifdef _WIN32
DWORD WINAPI Server::ClientFunction(LPVOID lpParam) {
  
  SOCKET currentClient = (SOCKET)lpParam;
  tstring commands;

  tcout << _T("Client ") << currentClient << _T(" connected!\n");

  Parser pObj;

  while (!Quit::canQuit()) {
    TCHAR buf[4096];
    ZeroMemory(buf, 4096);

    int bytesIn = recv(currentClient, (char *)buf, 4096, 0);

    if (bytesIn <= 0) {
      break;
    }

    commands = buf;

    pObj.parse(commands, currentClient);

    tstring currentCommandToBeSend = Server::GetList();
    Server::EmptyString();

    int sizeOfCommand = (int)currentCommandToBeSend.size() * sizeof(TCHAR);

    send(currentClient, (char *)&sizeOfCommand, sizeof(sizeOfCommand), 0);

    char *currentPosition = (char *)currentCommandToBeSend.c_str();

    int bytesSend;

    while (sizeOfCommand) {
      bytesSend = send(currentClient, currentPosition, sizeOfCommand, 0);
      sizeOfCommand -= bytesSend;
      currentPosition += bytesSend;
    }
  }
  tcout << _T("Client ") << currentClient << _T(" disconnected!\n");
  closesocket(currentClient);
  return 0;
}
#elif __linux__
void *Server::ClientFunctionLinux(void *lpParam) {

  tcout << "Client connected!\n";

  int *clientAdr = (int *)lpParam;
  int currentClient = *clientAdr;
  delete clientAdr;

  tstring commands;
  Parser pObj;

  while (!Quit::canQuit()) {
    TCHAR buf[4096];
    memset(buf, 0, 4096);

    int bytesIn = recv(currentClient, (char *)buf, 4096, 0);

    if (bytesIn == -1) { // Client disconnected!
      std::cerr << "Error in recv(). Quitting\n";
      break;
    }
    commands = buf;
    pObj.parse(commands, currentClient);

    tstring currentCommandToBeSend = Server::GetList();
    Server::EmptyString();

    int sizeOfCommand = (int)currentCommandToBeSend.size();

    send(currentClient, (char *)&sizeOfCommand, sizeof(sizeOfCommand), 0);

    char *currentPosition = (char *)currentCommandToBeSend.c_str();

    int bytesSend;

    while (sizeOfCommand) {
      bytesSend = send(currentClient, currentPosition, sizeOfCommand, 0);
      sizeOfCommand -= bytesSend;
      currentPosition += bytesSend;
    }
  }

  tcout << "Client " << currentClient << "disconnected!\n";
  Server::DeleteClient(currentClient);
  close(currentClient);

  pthread_t currentThdread = pthread_self();
  pthread_cancel(currentThdread);
  pthread_exit(NULL);
}
#endif
