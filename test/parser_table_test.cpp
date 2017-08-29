#include "parsertable.h"
#include "catch.hpp"
#include <sstream>

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
  s0.dump();
  
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
  s0.dump();

  ParserState s1(s0, g.symbols().t("("));
  s1.dump();

  ParserState s2(s1, g.symbols().t("id"));
  s2.dump();
  
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
    table.getState(i)->dump();
  }
  
}
