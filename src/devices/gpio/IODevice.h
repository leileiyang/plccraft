#include <map>

struct FuncNode {
  int func_id;
  int on;
  int reversal;
};

class IODevice {
 public:
  int Open(int port);
  int Open(int func_id);
  int Close(int port);
  int Close(int func_id);

  std::map<int, FuncNode> func_map_;

};
