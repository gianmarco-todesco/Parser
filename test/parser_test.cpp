#include "parser.h"
#include "catch.hpp"
#include <sstream>
#include <iostream>

using namespace std;

Grammar *makeTestGrammar1()
{
  Grammar *g = new Grammar();
  RuleBuilder(g,"Sum").n("Var").end();
  RuleBuilder(g,"Sum").n("Sum").t("+").n("Var").end();
  RuleBuilder(g,"Var").t("x").end();
  RuleBuilder(g,"Var").t("y").end();
  RuleBuilder(g,"Var").t("z").end();

  return g;
}


Grammar *makeTestGrammar2()
{
  Grammar *g = new Grammar();

  RuleBuilder(g,"Sum").n("Var").end(RuleAction("add"));
  RuleBuilder(g,"Sum").n("Sum").t("+").n("Var").end(RuleAction("add",1+4));
  RuleBuilder(g,"Var").t("x").end(RuleAction("var1",0));
  RuleBuilder(g,"Var").t("y").end(RuleAction("var2",0));
  RuleBuilder(g,"Var").t("z").end(RuleAction("var3",0));
  return g;
}

Grammar *makeTestGrammar3()
{
  Grammar *g = new Grammar();

  RuleBuilder(g,"E").n("T").end();
  RuleBuilder(g,"E").n("E").t("+").n("T").end(RuleAction("add",1+4));
  RuleBuilder(g,"T").n("F").end();
  RuleBuilder(g,"T").n("T").t("*").n("F").end(RuleAction("mul",1+4));
  RuleBuilder(g,"F").n("V").end();
  RuleBuilder(g,"F").t("(").n("E").t(")").end();
  RuleBuilder(g,"V").t("x").end(RuleAction("var1",0));
  RuleBuilder(g,"V").t("y").end(RuleAction("var2",0));
  RuleBuilder(g,"V").t("z").end(RuleAction("var3",0));
  
  return g;
}



TEST_CASE( "ParseState1", "[parser]") {
  Grammar *g = makeTestGrammar1();

  {
    ParseState state(g, "Sum");
    REQUIRE(state.getGrammar() == g);
    
    REQUIRE(state.getSignature() == "1,0;2,0;3,0;4,0;5,0;");
    REQUIRE(state.getId() == -1);
    state.setId(123);
    REQUIRE(state.getId() == 123);
    const std::vector<std::pair<int, int> > &items = state.getItems();
    REQUIRE(items.size() == 5);
    for(int i=0;i<5;i++)
      REQUIRE(items[i] == make_pair(i+1,0));

    vector<const Symbol*> symbols = state.getCurrentSymbols();
    REQUIRE(symbols.size() == 5);

    vector<const TerminalSymbol*> terminals = state.getCurrentTerminals();
    REQUIRE(terminals.size() == 3);

    REQUIRE(state.getCompleteRules().empty());
    
    ParseState state1(state, terminals[0]);

    REQUIRE(state1.getSignature() == "3,1;");
    REQUIRE(state1.getCurrentSymbols().empty());
    REQUIRE(state1.getCurrentTerminals().empty());
    REQUIRE(state1.getCompleteRules().size()==1);
    
  }
  delete g;
  REQUIRE(Symbol::getTotalCount()==0);
}

TEST_CASE( "ParseTable1", "[parser]") {
  Grammar *g = makeTestGrammar1();
  {
    ParseTable table(g);
    REQUIRE(table.getStateCount()==9);
    REQUIRE(table.getFirstState()==table.getState(0));
    for(int i=0;i<table.getStateCount();i++)
    {
      const ParseState *state = table.getState(i);
      REQUIRE(state->getId() == i);
      vector<const Symbol*>symbols = state->getCurrentSymbols();      
      for(int j=0;j<(int)symbols.size();j++)
      {
        const ParseState *nextState = state->getNextState(symbols[j]);
        REQUIRE(nextState != 0);
      }
    }
  }
  delete g;
  REQUIRE(Symbol::getTotalCount()==0);
}


TEST_CASE( "ParseTree", "[parser]") {
  
  StringTokenizer st("(2+3)*(4+5)");
  ParseTree ptree(&st);
  REQUIRE(ptree.getStackSize()==0);

  int k = 0;
  for(int i=0;i<4;i++) ptree.addLeaf(k++);
  REQUIRE(ptree.toString() == "'(','2','+','3'");  

  ptree.makeNode("A",3,0x5);
  REQUIRE(ptree.toString() == "'(',A('2','3')");

  ptree.addLeaf(k++);
  REQUIRE(ptree.toString() == "'(',A('2','3'),')'");

  ptree.takeOne(3,1);
  REQUIRE(ptree.toString() == "A('2','3')");  

  for(int i=0;i<5;i++) ptree.addLeaf(k++);
  REQUIRE(ptree.toString() == "A('2','3'),'*','(','4','+','5'");

  ptree.makeNode("A",3,0x5);
  REQUIRE(ptree.toString() == "A('2','3'),'*','(',A('4','5')");

  ptree.addLeaf(k++);
  REQUIRE(ptree.toString() == "A('2','3'),'*','(',A('4','5'),')'");
  
  ptree.takeOne(3,1);
  REQUIRE(ptree.toString() == "A('2','3'),'*',A('4','5')");

  ptree.makeNode("B",3,0x5);
  REQUIRE(ptree.toString() == "B(A('2','3'),A('4','5'))");

  REQUIRE(ptree.getStackSize() == 1);
  ParseNode nodeB = ptree.getNode(0);

  REQUIRE(nodeB.isLeaf() == false);
  REQUIRE(nodeB.getToken() == Token());
  REQUIRE(nodeB.getTag() == "B");
  REQUIRE(nodeB.getChildCount() == 2);
  REQUIRE(nodeB.getPosition() == 0);

  ParseNode nodeA1 = nodeB.getChild(0);
  REQUIRE(nodeA1.isLeaf() == false);
  REQUIRE(nodeA1.getToken() == Token());
  REQUIRE(nodeA1.getTag() == "A");
  REQUIRE(nodeA1.getChildCount() == 2);
  REQUIRE(nodeA1.getPosition() == 3);

  ParseNode leaf = nodeA1.getChild(0);
  REQUIRE(leaf.isLeaf() == true);
  REQUIRE(leaf.getToken() == Token(Token::T_Number, "2"));
  REQUIRE(leaf.getTag() == "");
  REQUIRE(leaf.getChildCount() == 0);
  REQUIRE(leaf.getPosition() == 6);

  leaf = nodeA1.getChild(1);
  REQUIRE(leaf.isLeaf() == true);
  REQUIRE(leaf.getToken() == Token(Token::T_Number, "3"));
  REQUIRE(leaf.getTag() == "");
  REQUIRE(leaf.getChildCount() == 0);
  REQUIRE(leaf.getPosition() == 9);

  ParseNode nodeA2 = nodeB.getChild(1);
  REQUIRE(nodeA2.isLeaf() == false);
  REQUIRE(nodeA2.getToken() == Token());
  REQUIRE(nodeA2.getTag() == "A");
  REQUIRE(nodeA2.getChildCount() == 2);
  REQUIRE(nodeA2.getPosition() == 12);

  leaf = nodeA2.getChild(0);
  REQUIRE(leaf.isLeaf() == true);
  REQUIRE(leaf.getToken() == Token(Token::T_Number, "4"));
  REQUIRE(leaf.getTag() == "");
  REQUIRE(leaf.getChildCount() == 0);
  REQUIRE(leaf.getPosition() == 15);

  leaf = nodeA2.getChild(1);
  REQUIRE(leaf.isLeaf() == true);
  REQUIRE(leaf.getToken() == Token(Token::T_Number, "5"));
  REQUIRE(leaf.getTag() == "");
  REQUIRE(leaf.getChildCount() == 0);
  REQUIRE(leaf.getPosition() == 18);  
}

TEST_CASE( "ParseOutput", "[parser]") {
  StringTokenizer st("thorin, balin, dwalin");
  ParseTree ptree(&st);
  REQUIRE(ptree.getStackSize()==0);

  int k = 0;
  for(int i=0;i<3;i++) ptree.addLeaf(k++);
  REQUIRE(ptree.toString() == "'thorin',',','balin'");  
  ptree.makeNode("Lst",3,0x5);
  for(int i=0;i<2;i++) ptree.addLeaf(k++);
  ptree.makeNode("Lst",3,0x5);
  
  REQUIRE(ptree.toString() == "Lst('thorin','balin','dwalin')");    
}

TEST_CASE( "Parser1", "[parser]") {

  StringTokenizer st("x+y+z");
  
  Grammar *g = makeTestGrammar2();
  Parser parser(g);
  parser.parse(&st);
  REQUIRE(parser.getParseTree()->toString() == "add(var1(),var2(),var3())");  
  delete g;
  REQUIRE(Symbol::getTotalCount()==0);
}

TEST_CASE( "Parser2", "[parser]") {

  const string tb[][2] = {
    {"x", "var1()"},
    {"x+y", "add(var1(),var2())"},
    {"x*y", "mul(var1(),var2())"},
    {"x+y+z", "add(var1(),var2(),var3())"},
    {"x*y*z", "mul(var1(),var2(),var3())"},
    {"x*y+z", "add(mul(var1(),var2()),var3())"},
    {"x+y*z", "add(var1(),mul(var2(),var3()))"},
    {"(x+y)", "add(var1(),var2())"},
    {"(x+y)*z", "mul(add(var1(),var2()),var3())"},
    {"x*(y+z)", "mul(var1(),add(var2(),var3()))"},
    {"x+y+z + x+y+z + x+y+z + x+y+z", "add("
                                      "var1(),var2(),var3(),"
                                      "var1(),var2(),var3(),"
                                      "var1(),var2(),var3(),"
                                      "var1(),var2(),var3())"},
    {"",""}
  };
  Grammar *g = makeTestGrammar3();
  Parser parser(g);
  // cout << parser.getParseTable().getStateCount() << endl;

  for(int i=0;tb[i][0] != "";i++)
  {

    StringTokenizer st(tb[i][0]);
    bool ret = parser.parse(&st);
    REQUIRE(ret == true);
    //REQUIRE(toString(*parser.getOutput()) == tb[i][1]);
  }

  delete g;

}
