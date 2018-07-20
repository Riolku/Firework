#include "parser.h"
using namespace std;

int main(int argc, char * argv[]){
  vector<token> tokens = lex(argc,argv);
 
  //debug
//   cout << "TOKENS:\n\n";
//   print_tokens(tokens);
  
  cout << "PARSE TREE:\n\n";
  parse_return* res = parse(tokens);
  
  //debug
  print_tree(res);
}