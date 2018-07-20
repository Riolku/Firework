#include <bits/stdc++.h>
#include "lexer.h"
using namespace std;


void init(vector<string> & tkns){
  tkns = {
    "`","#","(",")","[","]","{","}",":",";","\n","\"","\'","\\\"","\\\'",",",".","?","++","--","~","!","@","$","%","^","&","&&","*","**","-","+","|","||","<","<<",">",">>","/","//","=","~=","!=","@=","$=","%=","^=","&=","&&=","*=","**=","-=","+=","|=","||=","<=","<<=",">=",">>=","/=","//=","=="
  };  
}

bool match(string s,string pat,int offset){
  int i;
  for(i = offset;i < min(s.size(),pat.size() + offset);i++){
    if(s[i] != pat[i - offset]) return false;
  }
  return i - offset == pat.size();
}

vector<token> lex(int argc,char * argv[]){
  if(argc < 2){
    cerr << "No input files!\n";
    exit(1);
  }
  ifstream inFile(argv[1]);
  if(!inFile){
    cerr << "File '" << argv[1] << "' not found!\n";
    exit(1);
  }
  stringstream sstr;
  sstr << inFile.rdbuf();
  inFile.close();
  string program = sstr.str();
  vector<token> lexed;
  vector<string> tokens;
  init(tokens);
  
  int line=1;
  int col=1;
  
  string current = "";
  for(int i = 0; i < program.size(); i++){
    int longestMatch = 0;
    string tkn = "";
    for(string s:tokens){
      if(match(program,s,i)){
        if(s.size() > longestMatch){
          longestMatch = s.size();
          tkn = s;
        }
      }
    }
    if(longestMatch){
      if(current.size()){
        lexed.push_back(token(current, line, col - current.size()));
        current = "";
      }
      lexed.push_back(token(tkn, line, col));
      if(tkn=="\n") {
        line++;
        col = 1;
      }
      else{
        col += tkn.size();
      }
      i += tkn.size() - 1;
    }
    else if(!isspace(program[i])){
      if(!isalnum(program[i]) && program[i] != '_') {
        cerr << "Lex Error: Unexpected token on line " << line << " column " << col << ": '" << program[i] << "'" << endl;
      }
      current += program[i];
      col++;
    }
    else{
      if(current.size()) lexed.push_back(token(current, line, col - current.size()));
      current  = "";
      col++;
    }
  }
  if(current.size()) lexed.push_back(token(current, line, col - current.size()));
  
  return lexed;
  
}
//debugging
void print_tokens(vector<token> a) {
  for(auto b:a) {
    b.print();
  }
}