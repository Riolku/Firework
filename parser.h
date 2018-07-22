#include <bits/stdc++.h>
#include "lexer.h"

#define ANY parse_any()

#define pstr(f) parser f(#f)
#define pconsec(f, args...) parser f = parse_consecutive(string(#f), args)
#define por(f, args...) parser f = parse_or(string(#f), args)
#define plist(f, arg1, arg2) parser f = parse_list(string(#f), arg1, arg2)
#define prep(f, arg) parser f = parse_repeat(string(#f), arg)
#define popt(f, arg) parser f = parse_optional(string(#f), arg)
#define pexc(f, arg1, arg2) parser f = parse_except(string(#f), arg1, arg2)
  
typedef struct parse_return {
	string name;
	string error;
	int line;
	int col;
  int pos;
	vector<parse_return*> nodes;
	string base;
  parse_return() {
    error = "";
    name = "";
  }
  
  parse_return(string s, int l, int c, int p, string nm="<unnamed string constant>") {
    base = s;
    line = l;
    col = c;
    name = nm;
    pos = p;
  }
    
  parse_return(string err_msg, string nm = "<unnamed>") {
    error = err_msg;
    name = nm;
  }
  
	bool success() {
		return error == "";
	}
  
  
} parse_return;

class parser {
public:
	function<parse_return*(vector<token>, int)> func;
	parser(const char* s);
  parser(string s);
	parser(function<parse_return*(vector<token>, int)> f);
  parser(parse_return* f(vector<token>, int));
	parse_return* operator() (vector<token> tokens, int pos) const;
  parser operator|(parser other);
};

template<class ... Ts> parser parse_consecutive(string nm, Ts ... args);
template<class ... Ts> parser parse_or(string nm, Ts ... args);  
parser parse_list(string nm, parser a, parser b);
parser parse_repeat(string nm, parser a);
parser parse_optional(string nm, parser a);
parser parse_any(); 
parser parse_except(string nm, parser arg1, parser arg2);

void print_tree(parse_return* tree, string indent = "");

parse_return* parse(vector<token> tokens, bool debug);