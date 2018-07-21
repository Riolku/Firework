function fwk() {
  ~/workspace/fwk $@
}

function compile() {
  g++ parser.cpp lexer.cpp firework.cpp -std=c++11 -o fwk
}