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

  TerminalSymbol *eof = g->addTerminal(
    new TokenTypeTerminalSymbol("EOF", Token::T_Eof));

  RuleBuilder(g,"Sum").n("Var").end(RuleAction("add"));
  RuleBuilder(g,"Sum").n("Sum").t("+").n("Var").end(RuleAction("add",1+4));
  RuleBuilder(g,"Var").t("x").end(RuleAction("var1",0));
  RuleBuilder(g,"Var").t("y").end(RuleAction("var2",0));
  RuleBuilder(g,"Var").t("z").end(RuleAction("var3",0));
  RuleBuilder(g,"S").n("Sum").t(eof).end();
  
  return g;
}

Grammar *makeTestGrammar3()
{
  Grammar *g = new Grammar();

  TerminalSymbol *eof = g->addTerminal(
    new TokenTypeTerminalSymbol("EOF", Token::T_Eof));

  RuleBuilder(g,"E").n("T").end();
  RuleBuilder(g,"E").n("E").t("+").n("T").end(RuleAction("add",1+4));
  RuleBuilder(g,"T").n("F").end();
  RuleBuilder(g,"T").n("T").t("*").n("F").end(RuleAction("mul",1+4));
  RuleBuilder(g,"F").n("V").end();
  RuleBuilder(g,"F").t("(").n("E").t(")").end();
  RuleBuilder(g,"V").t("x").end(RuleAction("var1",0));
  RuleBuilder(g,"V").t("y").end(RuleAction("var2",0));
  RuleBuilder(g,"V").t("z").end(RuleAction("var3",0));
  RuleBuilder(g,"S").n("E").t(eof).end();
  
  return g;
}


void serialize(
  ostringstream &ss, 
  const ParseOutput &po, 
  const ParseOutputNode &node)
{
  if(node.isToken())
  {
    Token token = node.getToken();
    ss<<"'"<<token.getText()<<"'";
  }
  else
  {
    ss << node.getGroupName() << "(";
    for(int i=0;i<node.getChildCount();i++)
    {
      if(i>0) ss<<",";
      serialize(ss,po,po.getChildNode(node,i));      
    }
    ss << ")";
  }
}

string toString(const ParseOutput &po)
{
  ostringstream ss;
  for(int i=0;i<po.getCount();i++)
  {
    if(i>0) ss<<",";
    serialize(ss,po,po.getNode(i));
  }
  return ss.str();
}

TEST_CASE( "ParseState1", "[parser]") {
  Grammar *g = makeTestGrammar1();

  {
    ParseState state(g, "Sum");
    REQUIRE(state.getGrammar() == g);
    REQUIRE(state.getSignature() == "0,0;1,0;2,0;3,0;4,0;");
    REQUIRE(state.getId() == -1);
    state.setId(123);
    REQUIRE(state.getId() == 123);
    const std::vector<std::pair<int, int> > &items = state.getItems();
    REQUIRE(items.size() == 5);
    for(int i=0;i<5;i++)
      REQUIRE(items[i] == make_pair(i,0));

    vector<const Symbol*> symbols = state.getCurrentSymbols();
    REQUIRE(symbols.size() == 5);

    vector<const TerminalSymbol*> terminals = state.getCurrentTerminals();
    REQUIRE(terminals.size() == 3);

    REQUIRE(state.getCompleteRules().empty());
    
    ParseState state1(state, terminals[0]);
    REQUIRE(state1.getSignature() == "2,1;");
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
    ParseTable table(g, "Sum");
    REQUIRE(table.getStateCount()==8);
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


TEST_CASE( "ParseOutputNode", "[parser]") {
  Token token(Token::T_Special, "!");
  ParseOutputNode node1(1,token);
  REQUIRE(node1.isToken() == true);
  REQUIRE(node1.isGroup() == false);
  REQUIRE(node1.getToken() == token);
  REQUIRE(node1.getChildCount() == 0);
  REQUIRE(node1.getPosition() == 1);

  ParseOutputNode node2(2,"Group", 3);
  REQUIRE(node2.isToken() == false);
  REQUIRE(node2.isGroup() == true);
  REQUIRE(node2.getToken().getType() == Token::T_None);
  REQUIRE(node2.getGroupName() == "Group");
  REQUIRE(node2.getChildCount() == 3);
  REQUIRE(node2.getPosition() == 2);
}

TEST_CASE( "ParseOutput", "[parser]") {
  StringTokenizer st("(2+3)*(4+5)");
  ParseOutput po(&st);
  REQUIRE(po.getCount()==0);

  int k = 0;
  for(int i=0;i<4;i++) po.addToken(k++);
  REQUIRE(toString(po) == "'(','2','+','3'");  

  po.makeGroup("A",3,1+4);
  REQUIRE(toString(po) == "'(',A('2','3')");

  po.addToken(k++);
  REQUIRE(toString(po) == "'(',A('2','3'),')'");

  po.takeOne(1,3);
  REQUIRE(toString(po) == "A('2','3')");  

  for(int i=0;i<5;i++) po.addToken(k++);
  REQUIRE(toString(po) == "A('2','3'),'*','(','4','+','5'");

  po.makeGroup("A",3,1+4);
  REQUIRE(toString(po) == "A('2','3'),'*','(',A('4','5')");

  po.addToken(k++);
  REQUIRE(toString(po) == "A('2','3'),'*','(',A('4','5'),')'");
  
  po.takeOne(1,3);
  REQUIRE(toString(po) == "A('2','3'),'*',A('4','5')");

  po.makeGroup("B",3,1+4);
  REQUIRE(toString(po) == "B(A('2','3'),A('4','5'))");
}

TEST_CASE( "Parser1", "[parser]") {

  StringTokenizer st("x+y+z");
  
  Grammar *g = makeTestGrammar2();
  Parser parser(g,"S");
  parser.parse(&st);
  REQUIRE(toString(*parser.getOutput()) == "add(var1(),var2(),var3())");  
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
  Parser parser(g,"S");
  // cout << parser.getParseTable().getStateCount() << endl;

  for(int i=0;tb[i][0] != "";i++)
  {

    StringTokenizer st(tb[i][0]);
    bool ret = parser.parse(&st);
    REQUIRE(ret == true);
    REQUIRE(toString(*parser.getOutput()) == tb[i][1]);
  }

  delete g;

}
