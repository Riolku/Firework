#include "parser.h"
#include <string>
using namespace std;

int main(int argc, char * argv[]){
  bool debug = false;
  for(int i = 0;i < argc;i++) {
    if(string(argv[i])==string("-debug")) {
      debug = true;
    }
  }
  vector<token> tokens = lex(argc,argv);
 
  //debug
  cout << "TOKENS:\n\n";
  print_tokens(tokens);
  
  cout << "\n\nPARSE TREE:\n\n";
  parse_return* res = parse(tokens, debug);
  
  //debug
  print_tree(res);
}