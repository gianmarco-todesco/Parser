#include "grammar.h"
#include "catch.hpp"
#include <sstream>

using namespace std;

template<class T>
string toString(const T &t) {
  ostringstream ss;
  ss << t;
  return ss.str();
}

TEST_CASE( "SymbolTotalCount", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  {
    NonTerminalSymbol A("A");
    REQUIRE(Symbol::getTotalCount() == 1);  
  }
  REQUIRE(Symbol::getTotalCount() == 0);  
}

TEST_CASE( "NonTerminalSymbol", "[grammar]") {
  NonTerminalSymbol symbol("A");
  REQUIRE(symbol.isTerminal()==false);
  REQUIRE(symbol.isConstant()==false);
  REQUIRE(symbol.matches(Token(Token::T_Ident, "A"))==false);
  REQUIRE(symbol.getName() == "A");
  REQUIRE(toString(symbol) == "A");
}


TEST_CASE( "TextTerminalSymbol", "[grammar]") {
  TextTerminalSymbol symbol("A");
  REQUIRE(symbol.isTerminal()==true);
  REQUIRE(symbol.isConstant()==true);
  REQUIRE(symbol.matches(Token(Token::T_Ident, "A"))==true);
  REQUIRE(symbol.getName() == "'A'");
  REQUIRE(toString(symbol) == "'A'");

  TextTerminalSymbol symbol2("A","myname");
  REQUIRE(symbol2.isTerminal()==true);
  REQUIRE(symbol2.isConstant()==true);
  REQUIRE(symbol2.matches(Token(Token::T_Ident, "A"))==true);
  REQUIRE(symbol2.getName() == "myname");
  REQUIRE(toString(symbol2) == "myname");
}


TEST_CASE( "TokenTypeTerminalSymbol", "[grammar]") {
  TokenTypeTerminalSymbol symbol("special", Token::T_Special);
  REQUIRE(symbol.isTerminal()==true);
  REQUIRE(symbol.isConstant()==false);
  REQUIRE(symbol.matches(Token(Token::T_Ident, "A"))==false);
  REQUIRE(symbol.matches(Token(Token::T_Special, "A"))==true);
  REQUIRE(symbol.matches(Token(Token::T_Special, ";"))==true);
  REQUIRE(symbol.getName() == "special");
  REQUIRE(toString(symbol) == "special");
}


TEST_CASE( "NumberTerminalSymbol", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  NumberTerminalSymbol symbol;
  REQUIRE(symbol.isTerminal()==true);
  REQUIRE(symbol.isConstant()==false);
  REQUIRE(symbol.matches(Token(Token::T_Ident, "A"))==false);
  REQUIRE(symbol.matches(Token(Token::T_Number, "12.34"))==true);
  REQUIRE(symbol.getName() == "number");
  REQUIRE(toString(symbol) == "number");
}

TEST_CASE( "KeywordsTerminalSymbol", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  KeywordsTerminalSymbol symbol("a","b","c");
  REQUIRE(symbol.isTerminal()==true);
  REQUIRE(symbol.isConstant()==false);
  REQUIRE(symbol.matches(Token(Token::T_Ident, "A"))==false);
  REQUIRE(symbol.matches(Token(Token::T_Ident, "d"))==false);
  REQUIRE(symbol.matches(Token(Token::T_Ident, "a"))==true);
  REQUIRE(symbol.matches(Token(Token::T_Ident, "b"))==true);
  REQUIRE(symbol.matches(Token(Token::T_Ident, "c"))==true);
  REQUIRE(symbol.getName() == "['a', 'b', 'c']");
  REQUIRE(toString(symbol) == symbol.getName());
}


TEST_CASE( "AnyTerminalSymbol", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  AnyTerminalSymbol symbol;
  REQUIRE(symbol.isTerminal()==true);
  REQUIRE(symbol.isConstant()==false);
  REQUIRE(symbol.matches(Token(Token::T_Ident, "A"))==true);
  REQUIRE(symbol.matches(Token(Token::T_Number, "12.34"))==true);
  REQUIRE(symbol.matches(Token(Token::T_Special, ";"))==true);
  REQUIRE(symbol.getName() == "any");
  REQUIRE(toString(symbol) == symbol.getName());
}


TEST_CASE( "RuleAction", "[grammar]") {
  RuleAction a1,a2("A"),a3("A",0xffffffff),a4("B",123);
  REQUIRE(a1.getGroupName() == "");
  REQUIRE(a1.getMask() == 0xffffffff);
  REQUIRE(a2.getGroupName() == "A");
  REQUIRE(a2.getMask() == 0xffffffff);
  REQUIRE(a3.getGroupName() == "A");
  REQUIRE(a3.getMask() == 0xffffffff);
  REQUIRE(a4.getGroupName() == "B");
  REQUIRE(a4.getMask() == 123);
}

TEST_CASE( "Rule 1", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  Grammar grammar, *g = &grammar; 
  NonTerminalSymbol A("A");
  TextTerminalSymbol x("x");
  RuleAction action("Group");

  vector<Symbol*> symbols;
  symbols.push_back(&A);
  symbols.push_back(&x);
  Rule rule(&A, symbols, action);
  REQUIRE(rule.getId() == -1);
  rule.setId(123); 
  REQUIRE(rule.getId() == 123);
  // REQUIRE(rule.getGrammar() == g);
  REQUIRE(rule.getLeftSymbol() == &A);
  REQUIRE(rule.getLength() == 2);
  REQUIRE(rule.getRightSymbol(0) == &A);
  REQUIRE(rule.getRightSymbol(1) == &x);
  REQUIRE(rule.getRightSymbol(2) == 0);
  REQUIRE(rule.getAction().getGroupName() == action.getGroupName());

  ostringstream ss; ss << rule;
  REQUIRE(ss.str() == "A -> A 'x'");
}

TEST_CASE( "Rule 2", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  Grammar grammar, *g = &grammar; 
  NonTerminalSymbol A("A");
  RuleAction action("Group");

  vector<Symbol*> symbols;
  Rule rule(&A, symbols, action);
  REQUIRE(rule.getLength() == 0);
  REQUIRE(rule.getRightSymbol(0) == 0);

  REQUIRE(toString(rule)  == "A ->");
}

TEST_CASE( "Grammar 1", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  Grammar grammar, *g = &grammar;
  REQUIRE(g->getRuleCount()==0);
  vector<const Rule*>rules;
  g->getRulesByLeftSymbol(rules,"A");
  REQUIRE(rules.empty());

  REQUIRE(g->getNonTerminal("A", false) == 0);
  REQUIRE(g->getNonTerminal("A") != 0);
  REQUIRE(g->getNonTerminal("A")->getName() == "A");
  REQUIRE(g->getNonTerminal("A", false) != 0);

  NonTerminalSymbol *A = g->getNonTerminal("A");
  TerminalSymbol *x = g->addTerminal(new TextTerminalSymbol("x"));
  vector<Symbol*> symbols;
  symbols.push_back(x);
  g->addRule(new Rule(A,symbols, RuleAction("")));
  symbols.clear();
  symbols.push_back(A);
  symbols.push_back(x);
  g->addRule(new Rule(A,symbols, RuleAction("")));
  
  REQUIRE(g->getRuleCount()==2);
  REQUIRE(toString(*g->getRule(0))=="A -> 'x'");
  REQUIRE(toString(*g->getRule(1))=="A -> A 'x'");

  g->getRulesByLeftSymbol(rules,"A");
  REQUIRE(rules.size() == 2);
}

TEST_CASE( "Grammar 2", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  Grammar *g = new Grammar();
  RuleBuilder(g,"Sum").n("Var").end();
  RuleBuilder(g,"Sum").n("Sum").t("+").n("Var").end();
  RuleBuilder(g,"Var").t("x").end();
  RuleBuilder(g,"Var").t("y").end();
  RuleBuilder(g,"Var").t("z").end();

  REQUIRE(g->getNonTerminal("Sum",false) != 0);
  REQUIRE(g->getNonTerminal("Var",false) != 0);

  REQUIRE(g->getRuleCount()==5);
  REQUIRE(toString(*g->getRule(0)) == "Sum -> Var");
  REQUIRE(toString(*g->getRule(1)) == "Sum -> Sum '+' Var");
  REQUIRE(toString(*g->getRule(2)) == "Var -> 'x'");
  REQUIRE(toString(*g->getRule(3)) == "Var -> 'y'");
  REQUIRE(toString(*g->getRule(4)) == "Var -> 'z'");
  
  for(int i=0; i<g->getRuleCount();i++)
  {
    REQUIRE(g->getRule(i)->getId() == i);
  }

  vector<const Rule*> rules;
  g->getRulesByLeftSymbol(rules,"Sum");
  REQUIRE(rules.size()==2);
  rules.clear();
  g->getRulesByLeftSymbol(rules,"Var");
  REQUIRE(rules.size()==3);
  
  delete g;
  REQUIRE(Symbol::getTotalCount() == 0);
  
}

