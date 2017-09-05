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


TEST_CASE( "Parse tree 2", "[parsetree]") {
  StringTokenizer st("p x y");
  ParseTree tree(&st);
  tree.addLeaf(0);
  tree.addLeaf(1);
  tree.makeNode("V",1);
  tree.makeNode("A",1);
  tree.addLeaf(2);
  tree.makeNode("V",1);
  tree.makeNode("A",2);
  tree.makeNode("S",2,0x2);

  REQUIRE(tree.getStackSize() == 1);
  REQUIRE(tree.toString() == "S(A(V('x'),V('y')))");
  ParseNode root = tree.getNode(0);
  REQUIRE(root.getTag() == "S" );
  REQUIRE(root.toString() == tree.toString() );
  REQUIRE(root.getText() == "p x y");
  REQUIRE(root.getChildCount() == 1);
  ParseNode A = root.getChild(0);
  REQUIRE(A.getTag() == "A" );
  REQUIRE(A.toString() == "A(V('x'),V('y'))" );
  REQUIRE(A.getText() == "x y" );
  REQUIRE(A.getChildCount() == 2 );
  ParseNode V = A.getChild(0);
  REQUIRE(V.getTag() == "V" );
  REQUIRE(V.toString() == "V('x')" );
  REQUIRE(V.getText() == "x" );
  REQUIRE(V.getChildCount() == 1 );
  REQUIRE(V.getChild(0).toString() == "'x'");
  V = A.getChild(1);
  REQUIRE(V.getTag() == "V" );
  REQUIRE(V.toString() == "V('y')" );
  REQUIRE(V.getText() == "y" );
  REQUIRE(V.getChildCount() == 1 );
  REQUIRE(V.getChild(0).toString() == "'y'");
  
  
}
