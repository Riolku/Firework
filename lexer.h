#include <string>
#include <iostream>
#include <vector>
using namespace std;

struct token {
  std::string str;
  int line;
  int col;
  
  token() {
    
  }
  
  token(std::string s, int l, int c) {
    line = l;
    col = c;
    str = s;
  }
  
  bool operator==(const char*other) {
    return str == other;
  }
  
  bool operator==(std::string other) {
    return str == other;
  }
  
  //debug
  void print() {
    std::cout << "'" << str << "' on line " << line << " column " << col << endl;
  }
};

void init(std::vector<std::string> & tkns);
bool match(std::string s,std::string pat,int offset);
std::vector<token> lex(int argc,char * argv[]);
void print_tokens(vector<token> a);