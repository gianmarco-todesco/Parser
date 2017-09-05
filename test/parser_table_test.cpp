#include "parsertable.h"
#include "catch.hpp"
#include <sstream>
#include <iostream>

using namespace std;


TEST_CASE( "ParserTable 1", "[parsertable]") {
  Grammar g;
  RuleBuilder(&g,"S").n("L").t("=").n("R").end();
  RuleBuilder(&g,"S").n("R").end();
  RuleBuilder(&g,"L").t("*").n("R").end();
  RuleBuilder(&g,"L").n("V").end();
  RuleBuilder(&g,"R").n("L").end();
  RuleBuilder(&g,"V").t("x").end();
  RuleBuilder(&g,"V").t("y").end();
  RuleBuilder(&g,"V").t("z").end();

  ParserState s0(&g);
  // s0.dump();
  
}


TEST_CASE( "ParserTable 2", "[parsertable]") {
  Grammar g;
  RuleBuilder(&g,"E").n("E").t("+").n("T").end();
  RuleBuilder(&g,"E").n("T").end();
  RuleBuilder(&g,"T").n("T").t("*").n("F").end();
  RuleBuilder(&g,"T").n("F").end();
  RuleBuilder(&g,"F").t("(").n("E").t(")").end();
  RuleBuilder(&g,"F").t("id").end();

  ParserState s0(&g);
  //s0.dump();

  ParserState s1(s0, g.symbols().t("("));
  //s1.dump();

  ParserState s2(s1, g.symbols().t("id"));
  //s2.dump();
  
}

TEST_CASE( "ParserTable 3", "[parsertable]") {
  Grammar g;
  RuleBuilder(&g,"E").n("E").t("+").n("T").end();
  RuleBuilder(&g,"E").n("T").end();
  RuleBuilder(&g,"T").n("T").t("*").n("F").end();
  RuleBuilder(&g,"T").n("F").end();
  RuleBuilder(&g,"F").t("(").n("E").t(")").end();
  RuleBuilder(&g,"F").t("id").end();

  ParserTable table(&g);
  for(int i=0;i<table.getStateCount();i++)
  {
    //table.getState(i)->dump();
  }
  
}


TEST_CASE( "ParserTable 4", "[parsertable]") {
  Grammar g;
  RuleBuilder(&g,"S").n("A").t("a").end();
  RuleBuilder(&g,"S").n("B").t("b").end();
  RuleBuilder(&g,"A").t("c").end();
  RuleBuilder(&g,"B").t("c").end();
  ParserTable table(&g);
  // table.dump(cout);
  REQUIRE(table.isAmbiguous() == false);  
}


TEST_CASE( "ParserTable 5", "[parsertable]") {
  Grammar g;
  RuleBuilder(&g,"S").t("i").n("S").end();
  RuleBuilder(&g,"S").t("i").n("S").t("e").n("S").end();
  RuleBuilder(&g,"S").t("p").end();
  ParserTable table(&g);
  // table.dump(cout);
  REQUIRE(table.isAmbiguous() == true);  
}


