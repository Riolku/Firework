#include<bits/stdc++.h>
#include "parser.h"

using namespace std;

parser::parser(function<parse_return*(vector<token>, int)> f) {
	func = f;
}

parser::parser(parse_return* f(vector<token>, int)) {
  func = function<parse_return*(vector<token>, int)>(f);
}

parser::parser(const char * param) {
	func = [=](vector<token> tokens, int pos) {
    string s = param;
		if (tokens[pos] == s) {
      return new parse_return(tokens[pos].str, tokens[pos].line, tokens[pos].col, pos + 1, "<string constant '" + s + "'>");
		}
    return new parse_return("Failed to match string literal '" + s + "'.\n", s);
	};
}

parser::parser(string s) {
	func = [=](vector<token> tokens, int pos) {
		parse_return * ret;
    ret -> name = s;
		if (tokens[pos] == s) {
      ret = new parse_return(tokens[pos].str, tokens[pos].line, tokens[pos].col, pos + 1, "<string constant '" + s + "'>");
		}
		else {
      ret = new parse_return("Failed to match string literal '" + s + "'.\n");
		}
		return ret;
	};
}

template<class ... Ts> parser parse_consecutive(string nm, Ts ... args) {
  vector<parser> funcs = { args... };
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tkns, int pos) {
    int curr_pos = pos;
    parse_return * ret = new parse_return();
    ret -> name = nm;
    for(int i = 0; i < funcs.size() ; i++) {
      parse_return * current = funcs[i](tkns, curr_pos);
      if(!current->success()) {
        return new parse_return("ERROR");
      }
      curr_pos = current->pos;
      ret->nodes.push_back(current);
    }
    
    ret -> pos = curr_pos; 
    return ret;
  });
}

template<class ... Ts> parser parse_or(string nm, Ts ... args) {
  vector<parser> funcs = { args... };
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tkns, int pos) {
    for(int i = 0; i < funcs.size() ; i++) {
      parse_return * current = funcs[i](tkns, pos);
      if(current->success()) {
        current -> name = nm;
        return current;
      }
    }
    return new parse_return("ERROR");
  });
}

parser parse_list(string nm, parser elem, parser sep) {
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tkns, int pos) {    
    parse_return * ret = new parse_return();
    ret->name = nm;
    parse_return * first = elem(tkns, pos);
    if(!first->success()) {
      return new parse_return("ERROR");
    }
    ret->nodes.push_back(first);
    int curr_pos = first->pos;
    while(1) {
      parse_return * parsed_sep = sep(tkns, curr_pos);
      if(!parsed_sep->success()) {
        break;
      }
      curr_pos = parsed_sep->pos;
      parse_return * parsed_elem = elem(tkns, curr_pos);
      if(!parsed_elem->success()) {
        break;
      }
      
      ret->nodes.push_back(parsed_sep);
      ret->nodes.push_back(parsed_elem);
      
      curr_pos = parsed_elem->pos;
    }
    
    ret -> pos = curr_pos;
    return ret;
    
  });
}

parser parse_repeat(string nm, parser to_rep) {
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tkns, int pos) {
    parse_return * ret = new parse_return();
    ret->name = nm;
    parse_return * first = to_rep(tkns, pos);
    if(!first->success()) {
      return new parse_return("ERROR");
    }
    ret->nodes.push_back(first);
    int curr_pos = first->pos;
    while(1) {
      parse_return * parsed = to_rep(tkns, curr_pos);
      if(!parsed->success()) {
        break;
      }
      ret -> nodes.push_back(parsed);
      
      curr_pos = parsed->pos;
    }
    
    ret -> pos = curr_pos; 
    return ret;
  });
}


parse_return* parser::operator () (vector<token> tokens, int pos) const {
  if(pos >= tokens.size()) {
    return new parse_return("ERROR");
  }
	return func(tokens, pos);
}

parser parse_optional(string nm, parser a) {
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tokens, int pos) { 
    parse_return* parsed = a(tokens, pos);
    if(parsed->success()) {
      parsed->name = nm;
      return parsed;
    }
    return new parse_return("<EMPTY STRING>", tokens[pos].line, tokens[pos].col, pos, nm);
  });
}

parser parse_any() {
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tokens, int pos) { 
    return new parse_return(tokens[pos].str, tokens[pos].line, tokens[pos].col, pos + 1, "ANY");
  });
}

parser parse_except(string nm, parser arg1, parser arg2) {
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tokens, int pos) { 
    parse_return* first = arg1(tokens, pos);
    parse_return* second = arg2(tokens, pos);
    if(!second->success()) {
      return first;
    }
    return new parse_return("ERROR");
  });
}

void print_tree(parse_return* tree, string indent) {
  if(!tree) return;
  if(!tree -> success()) {
    cerr << tree->error << endl;
    return;
  }
  if(tree -> name[0] == '<') {
    cout << indent << tree -> base << endl;
    return;
  }
  cout << indent << tree->name << ":" << tree-> base << endl;
  for(auto a : tree->nodes) {
    print_tree(a, indent+"  ");
  }
}

namespace fireworkLang {
  
  namespace val {
    //positive number
    parse_return * pos_num (vector<token> tokens, int pos) {
      string to_match = tokens[pos].str;
      if(all_of(to_match.begin(), to_match.end(), [](char a){return isdigit(a);})){
        return new parse_return(tokens[pos].str, tokens[pos].line, tokens[pos].col, pos + 1, "number"); 
      }
      return new parse_return("Failed to match number.");
    }

    //identifier
    parse_return * identifier(vector<token> tokens, int pos) {
      string to_match = tokens[pos].str;
      if(isalpha(to_match[0]) || to_match[0] == '_') {
        if(all_of(to_match.begin(), to_match.end(), [](char a){
          return a == '_' || isalnum(a);
        })) {
          return new parse_return(tokens[pos].str, tokens[pos].line, tokens[pos].col, pos + 1, "identifier"); 
        }
      }
      return new parse_return("Failed to match identifier.");
    }

    //negative number
    namespace number {
      popt(neg_sign, "-");
      pconsec(num, neg_sign, pos_num);
    } using namespace number;
    
    //string
    namespace str {
      pexc(fw_string_elem, ANY, "\"");
      prep(fw_string_content, fw_string_elem);
      pconsec(fw_string, "\"", fw_string_content, "\"");
    } using namespace str;
    
    //decimal
    pconsec(fw_float, num, ".", pos_num);
    
    //value
    por(value, fw_float, fw_string, num, identifier, pos_num);
  } using namespace val;
  
  parse_return* expression (vector<token> tkns, int pos);

  namespace unary {
    //index access
    pconsec(indexAccessor, "[", value, "]");
    
    //tuple (list of comma sep expressions)
    plist(tuple, value, ",");
    
    
  }
  
  auto main = num;
  
}

parse_return * parse(vector<token> tkns) {
  return fireworkLang::main(tkns, 0);
}