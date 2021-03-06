#include "parser.h"
namespace fireworkLang {
  
  parse_return* expr (vector<token> tokens, int pos);
  parse_return* assign(vector<token> tokens, int pos);
  parse_return* prog(vector<token> tokens, int pos);
  
  //positive number
  parse_return * pos_num (vector<token> tokens, int pos) {
    string to_match = tokens[pos].str;
    if(all_of(to_match.begin(), to_match.end(), [](char a){return isdigit(a);})){
      if(debug || match_only) cerr << "Matched number '" + (to_match == "\n" ? "\\n" : to_match) + "'." << endl;
      return new parse_return(tokens[pos].str, tokens[pos].line, tokens[pos].col, pos + 1, "number"); 
    }
    if(debug) cerr << "Failed to match number '" + (to_match == "\n" ? "\\n" : to_match) + "' at position " << pos << endl;
    return new parse_return("Failed to match number.");
  }

  //identifier
  parse_return * identifier(vector<token> tokens, int pos) {
    string to_match = tokens[pos].str;
    if(to_match=="not") goto failure;
    if(isalpha(to_match[0]) || to_match[0] == '_') {
      if(all_of(to_match.begin(), to_match.end(), [](char a){
        return a == '_' || isalnum(a);
      })) {
        if(debug || match_only) cerr << "Matched identifier '" + (to_match == "\n" ? "\\n" : to_match) + "' at position " << pos << endl;
        return new parse_return(tokens[pos].str, tokens[pos].line, tokens[pos].col, pos + 1, "identifier"); 
      }
    }
    failure:
    if(debug) cerr << "Failed to match identifier '" + (to_match == "\n" ? "\\n" : to_match) + "' at position " << pos << endl;
    return new parse_return("Failed to match identifier.");
  }
  
  //identifier list
  prep(list_of_identifiers, identifier);
  
  //negative number
  popt(neg_sign, "-");
  pconsec(num, neg_sign, pos_num);

  //string
  pexc(fw_string_elem, ANY, "\"");
  prep(fw_string_content, fw_string_elem);
  pconsec(fw_string, "\"", fw_string_content, "\"");

  //decimal
  pconsec(fw_float, num, ".", pos_num);

  //value
  por(value, fw_float, fw_string, num, identifier, pos_num);

  //index access
  pconsec(indexAccessor, "[", expr, "]");

  //tuple (list of comma sep expressions)
  plist(tuple_content, expr, ",");
  pconsec(fw_tuple, "(", tuple_content, ")");
    
  //dot operator, and -> operator
  por(classAccessor, ".", "->");
  
  plist(accessedMember, value, classAccessor);
  
  //preunary
  por(preUnarySyms, "++", "--", "~", "-", "+", "!", "not", "&", "*");
  prep(preUnarySymRep, preUnarySyms);
  
  //postunary
  
  plist(arguments, expr, ",");
  pconsec(functionCall, "(", arguments, ")");
  por(postUnarySyms, "++", "--", indexAccessor, functionCall);
  prep(postUnarySymRep, postUnarySyms);
  
  //unary
  pconsec(unary, preUnarySymRep, accessedMember, postUnarySymRep);
  
  //exponents!!!
  plist(exponent, unary, "**");
  
  //multiplication
  por(mul_div_symbol, "*", "/", "//", "%");
  plist(mul_div, exponent, mul_div_symbol);
  
  //addition
  por(add_sub_symbol, "+", "-");
  plist(add_sub, mul_div, add_sub_symbol);
  
  //binary operators
  por(binary_operator, "&", "^", "|", ">>", "<<");
  plist(binary, add_sub, binary_operator);
  
  //in operators
  por(in_operator, "in", "not in");
  plist(in_expression, binary, in_operator);
  
  //comparisons
  por(comparison_operator, ">", ">=", "<", "<=", "==", "!=");
  plist(comparison, in_expression, comparison_operator);
  
  //comparison and/or
  por(logical_and_or_operator, "and", "&&", "or", "||");
  plist(logical_and_or_expression, comparison, logical_and_or_operator);
  
  //ternary
  pconsec(ternary, logical_and_or_expression, "?", logical_and_or_expression, ":", logical_and_or_expression);
  
  //expression
  por(expression, ternary, logical_and_or_expression);
  
  //assignment
  por(assignment_symbol, "=", "-=", "+=", "~=", "%=", "^=", "&=", "*=", "**=", "&&=", "|=", "||=", "/=", "//=");
  pconsec(assignment, list_of_identifiers, assignment_symbol, expr);
  
  //statement
  por(statement, assignment, expr, "");
  
  //end statement
  por(end_statement, "\n", ";");
  
  //program
  plist(program, statement, end_statement);
  
  auto main = program;
  
  parse_return* expr (vector<token> tokens, int pos){
    return expression(tokens, pos);
  }
  parse_return* assign(vector<token> tokens, int pos) {
    return assignment(tokens, pos);
  }
  parse_return* prog(vector<token> tokens, int pos) {
    return program(tokens, pos);
  }
}

parse_return * parse(vector<token> tokens, bool dbg, bool mtch) {
  debug = dbg;
  match_only = mtch;
  return compress(fireworkLang::main(tokens, 0));
}