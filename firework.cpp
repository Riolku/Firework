#include "parser.h"
#include <string>
using namespace std;

int main(int argc, char * argv[]){
  bool debug = false, match = false, list_tokens = false;
  for(int i = 0;i < argc;i++) {
    if(string(argv[i])==string("-debug")) {
      debug = true;
    }
    else if(string(argv[i])==string("-match")) {
      match = true;
    }
    else if(string(argv[i])==string("-tokens")) {
      list_tokens = true;
    }
  }
  vector<token> tokens = lex(argc,argv);
 
  //debug
  if(list_tokens) cout << "TOKENS:\n\n";
  if(list_tokens) print_tokens(tokens);
  
  cout << "\n\nPARSE TREE:\n\n";
  parse_return* res = parse(tokens, debug, match);
  
  //debug
  print_tree(res);
}