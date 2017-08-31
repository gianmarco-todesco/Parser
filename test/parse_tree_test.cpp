#include "parsetree.h"
#include "catch.hpp"
#include <sstream>
#include <iostream>
using namespace std;


TEST_CASE( "Parse tree 1", "[parsetree]") {
  StringTokenizer st("uno due tre\nquattro cinque");
  ParseTree tree(&st);
  for(int i=0;i<4;i++) tree.addLeaf(i);
  tree.makeNode("first_line",4);
  tree.dump(cout);
}
