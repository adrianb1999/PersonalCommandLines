#ifndef _LISTPROCS_H_
#define _LISTPROCS_H_

#include "../Command.h"
#include <map>
#include <vector>

class ListProcs : public Command {
private:
  bool tree;
  bool execute();
  void recTree(int, int, std::map<int, int> &, std::vector<int> &);
  void crossingTree(std::map<int, int> &, std::vector<int> &);
  void getParrent(int &, std::map<int, int>);
#ifdef __linux__
  int getPPID_LINUX(std::string);
#endif
public:
  ListProcs(const bool &tree);
};

#endif