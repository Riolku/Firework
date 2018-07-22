template<class ... Ts> parser parse_consecutive(string nm, Ts ... args) {
  vector<parser> funcs = { args... };
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tokens, int pos) {
    if(pos >= tokens.size()) {
      if(debug) cerr << "Failed to match '" << nm << "' at position " << pos << " : Out of range" << endl;
      return new parse_return("ERROR", nm);
    }
    int curr_pos = pos;
    parse_return * ret = new parse_return();
    ret -> name = nm;
    for(int i = 0; i < funcs.size() ; i++) {
      parse_return * current = funcs[i](tokens, curr_pos);
      if(!current->success()) {
        if(debug) cerr << "Failed to match '" << nm << "' at position " << pos << endl;
        return new parse_return("ERROR", nm);
      }
      curr_pos = current->pos;
      ret->nodes.push_back(current);
    }
    
    ret -> pos = curr_pos; 
    if(debug || match_only) cerr << "Matched '" << nm << "' at position " << pos << endl;
    return ret;
  });
}

template<class ... Ts> parser parse_or(string nm, Ts ... args) {
  vector<parser> funcs = { args... };
  return function<parse_return*(vector<token>, int)> ([=] (vector<token> tokens, int pos) {
    if(pos >= tokens.size()) {
      if(debug) cerr << "Failed to match '" << nm << "' at position " << pos << " : Out of range" << endl;
      return new parse_return("ERROR", nm);
    }
    parse_return * ret = new parse_return();
    ret -> name = nm;
    for(int i = 0; i < funcs.size() ; i++) {
      parse_return * current = funcs[i](tokens, pos);
      if(current->success()) {
        ret->nodes.push_back(current);
        ret->pos = current -> pos;
        if(debug || match_only) cerr << "Matched '" << nm << "' at position " << pos << endl;
        return ret;
      }
    }
    if(debug) cerr << "Failed to match '" << nm << "' at position " << pos << endl;
    return new parse_return("ERROR", nm);
  });
}