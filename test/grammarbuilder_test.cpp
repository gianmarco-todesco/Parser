#include "grammarbuilder.h"
#include "catch.hpp"
#include <sstream>
#include <iostream>

using namespace std;



TEST_CASE( "grammar builder 1", "[grammarbuilder]") {
  REQUIRE(Symbol::getTotalCount()==0);
  SECTION("body")
  {
    
    StringTokenizer st(
          "A -> B\n"
          "A -> A B\n"
          "B -> 'x'\n"
          "B -> 'y'\n"
          "B -> 'z'\n"
          );

    GrammarBuilder gb;
    Grammar *g = gb.build(&st);

    // g->dump(cout);

    StringTokenizer st2("x y z z");
    Parser parser(g);
    bool ret = parser.parse(&st2);
    REQUIRE(ret);
    REQUIRE(parser.getParseTree()->toString() == "A(B('x'),B('y'),B('z'),B('z'))");
    delete g;
  }
  REQUIRE(Symbol::getTotalCount()==0);
}



TEST_CASE( "grammar builder 2", "[grammarbuilder]") {
  REQUIRE(Symbol::getTotalCount()==0);
  SECTION("body")
  {
    
    StringTokenizer st(
          "A -> C 'x' A\n"
          "A -> \n"
          "B -> 'x' C 'y'\n"
          "B -> 'x' C \n"
          "C -> 'x' B 'x'\n"
          "C -> 'z'\n"
          );

    GrammarBuilder gb;
    Grammar *g = gb.build(&st);

    // g->dump(cout);

    
    StringTokenizer st2("x x z x x");
    Parser parser(g);
    bool ret = parser.parse(&st2);
    REQUIRE(ret);
    delete g;
  }
  REQUIRE(Symbol::getTotalCount()==0);
}
