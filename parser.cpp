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
    if(pos >= tokens.size()) {
      if(debug) cerr << "Failed to match string literal '" + s + "' at position " << pos << ": Out of range" << endl;
      return new parse_return("Failed to match string literal '" + s + "'.\n", s);
    }
		if (tokens[pos] == s) {
      if(debug || match_only) cerr << "Matched string literal '" + s + "' at position " << pos << endl;
      return new parse_return(tokens[pos].str, tokens[pos].line, tokens[pos].col, pos + 1, "<string constant '" + s + "'>");
		}
    if(debug) cerr << "Failed to match string literal '" + s + "' at position " << pos << endl;
    return new parse_return("Failed to match string literal '" + s + "'.\n", s);
	};
}

parser::parser(string s) {
	func = [=](vector<token> tokens, int pos) {
		parse_return * ret;
    if(pos >= tokens.size()) {
      if(debug) cerr << "Failed to match string literal '" + s + "' at position " << pos << ": Out of range" << endl;
      return new parse_return("Failed to match string literal '" + s + "'.\n", s);
    }
		if (tokens[pos] == s) {
      if(debug || match_only) cerr << "Matched string literal '" + s + "' at position " << pos << endl;
      return new parse_return(tokens[pos].str, tokens[pos].line, tokens[pos].col, pos + 1, "<string constant '" + s + "'>");
		}
    if(debug) cerr << "Failed to match string literal '" + s + "' at position " << pos << endl;
    return new parse_return("Failed to match string literal '" + s + "'.\n");
	};
}

parser parse_list(string nm, parser elem, parser sep) {
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tokens, int pos) {    
    parse_return * ret = new parse_return();
    ret->name = nm;
    parse_return * first = elem(tokens, pos);
    if(!first->success()) {
      ret->pos = pos;
      return ret;
    }
    ret->nodes.push_back(first);
    int curr_pos = first->pos;
    parse_return * parsed_sep, * parsed_elem;
    while(1) {
      parsed_sep = sep(tokens, curr_pos);
      if(!parsed_sep->success()) {
        if(debug) cerr << "Failed to match " << parsed_sep->name << " separator at position " << curr_pos << endl;
        break;
      }
      curr_pos = parsed_sep->pos;
      parsed_elem = elem(tokens, curr_pos);
      if(!parsed_elem->success()) {
        if(debug) cerr << "Failed to match " << parsed_elem->name <<" element at position " << curr_pos << endl;
        break;
      }
      
      ret->nodes.push_back(parsed_sep);
      ret->nodes.push_back(parsed_elem);
      
      curr_pos = parsed_elem->pos;
    }
    
    ret -> pos = curr_pos;
    if(debug || match_only) cerr << "Matched '" << nm << "' at position " << pos << endl;
    return ret;
    
  });
}

parser parse_repeat(string nm, parser to_rep) {
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tokens, int pos) {
    parse_return * ret = new parse_return();
    ret->name = nm;
    int curr_pos = pos;
    parse_return * parsed;
    while(1) {
      parsed = to_rep(tokens, curr_pos);
      if(!parsed->success()) {
        if(debug) cerr << "Failed to match " << parsed->name << " to repeat at position " << curr_pos << endl;
        break;
      }
      ret -> nodes.push_back(parsed);
      
      curr_pos = parsed->pos;
    }
    
    ret -> pos = curr_pos; 
    if(debug || match_only) cerr << "Matched '" << nm << "' at position " << pos << endl;
    return ret;
  });
}


parse_return* parser::operator () (vector<token> tokens, int pos) const {
  return func(tokens, pos);
}

parser parse_optional(string nm, parser a) {
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tokens, int pos) {
    if(pos >= tokens.size()) {
      if(debug) cerr << "Failed to match '" << nm << "' at position " << pos << " : Out of range" << endl;
      return new parse_return("ERROR", nm);
    }
    parse_return* parsed = a(tokens, pos);
    if(parsed->success()) {
      if(debug || match_only) cerr << "Matched '" << nm << "' at position " << pos << endl;
      parsed->name = nm;
      return parsed;
    }
    if(debug || match_only) cerr << "Matched '" << "<EMPTY STRING>" << "' at position " << pos << endl;
    return new parse_return("<EMPTY STRING>", tokens[pos].line, tokens[pos].col, pos, nm);
  });
}

parser parse_any() {
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tokens, int pos) { 
    if(pos >= tokens.size()) {
      if(debug) cerr << "Failed to match '" << "ANY" << "' at position " << pos << " : Out of range" << endl;
      return new parse_return("Out of range.");
    }
    return new parse_return(tokens[pos].str, tokens[pos].line, tokens[pos].col, pos + 1, "ANY");
  });
}

parser parse_except(string nm, parser arg1, parser arg2) {
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tokens, int pos) { 
    if(pos >= tokens.size()) {
      if(debug) cerr << "Failed to match '" << nm << "' at position " << pos << " : Out of range" << endl;
      return new parse_return("ERROR", nm);
    }
    parse_return* first = arg1(tokens, pos);
    parse_return* second = arg2(tokens, pos);
    if(!second->success()) {
      if(debug || match_only) cerr << "Matched '" << nm << "' at position " << pos << endl;
      return first;
    }
    if(debug) cerr << "Failed to match '" << nm << "' at position " << pos << endl;
    return new parse_return("ERROR", nm);
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
    print_tree(a, indent+" ");
  }
}

parse_return * compress(parse_return * tree) {
  if(tree->isBase()) return tree;
  vector<parse_return*> v = tree->nodes;
  tree->nodes.clear();
  for(int i = 0; i < v.size(); i++) {
    parse_return * res = compress(v[i]);
    if(res) {
      tree->nodes.push_back(res);
    }
  }
  if(tree->nodes.size() == 0) {
    return NULL;
  }
  if(tree->nodes.size() == 1) {
    return compress(tree->nodes[0]);
  }
  return tree;
}