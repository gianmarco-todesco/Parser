#include "parsetree.h"
#include "catch.hpp"
#include <sstream>
#include <iostream>
using namespace std;


TEST_CASE( "Parse tree 1", "[parsetree]") {
  StringTokenizer st("uno \"due\" 3\nquattro \t cinque ");
  ParseTree tree(&st);
  for(int i=0;i<4;i++) tree.addLeaf(i);
  tree.makeNode("first_line",4);
  for(int i=0;i<2;i++) tree.addLeaf(4+i);
  tree.makeNode("second_line",2);
  tree.makeNode("full",2);

  REQUIRE(tree.getStackSize() == 1);
  REQUIRE(tree.toString() == "full(first_line('uno',q\"due\",3,EOL),second_line('quattro','cinque'))");
  ParseNode top = tree.getNode(0);
  REQUIRE(top.getTag() == "full");
  REQUIRE(top.getChildCount() == 2);
  REQUIRE(top.getChild(0).getTag() == "first_line");
  REQUIRE(top.getChild(1).getTag() == "second_line");

  REQUIRE(top.getText() == "uno \"due\" 3\nquattro \t cinque");
  REQUIRE(top.getChild(0).getText() == "uno \"due\" 3\n");
  REQUIRE(top.getChild(1).getText() == "quattro \t cinque");



  //tree.dump(cout);
}
