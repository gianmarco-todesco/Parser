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
    SymbolPool pool;
    REQUIRE(Symbol::getTotalCount() == 2);  
    pool.getNonTerminalSymbol("A");
    REQUIRE(Symbol::getTotalCount() == 3);  
  }
  REQUIRE(Symbol::getTotalCount() == 0);  
}


TEST_CASE( "NonTerminalSymbol", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  SECTION("body")
  {
    SymbolPool pool;
    REQUIRE(Symbol::getTotalCount() == 2);  
    const NonTerminalSymbol *s = pool.getNonTerminalSymbol("A");
    REQUIRE(s->isTerminal()==false);
    REQUIRE(s->isConstant()==false);
    REQUIRE(s->matches(Token(Token::T_Ident, "A"))==false);
    REQUIRE(s->getName() == "A");
    REQUIRE(toString(*s) == "A");
    const NonTerminalSymbol *s1 = pool.getNonTerminalSymbol("A");
    REQUIRE(s1 == s);
    s1 = pool.getNonTerminalSymbol("B");
    REQUIRE(s1 != s);
    REQUIRE(Symbol::getTotalCount() == 4);    
  }
  REQUIRE(Symbol::getTotalCount() == 0);
}


TEST_CASE( "TextTerminalSymbol", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  SECTION("body")
  {
    SymbolPool pool;
    const TerminalSymbol &symbol = *pool.getTextTerminalSymbol("A");
    REQUIRE(symbol.isTerminal()==true);
    REQUIRE(symbol.isConstant()==true);
    REQUIRE(symbol.matches(Token(Token::T_Ident, "A"))==true);
    REQUIRE(symbol.getName() == "'A'");
    REQUIRE(toString(symbol) == "'A'");

    const TerminalSymbol &symbol2 = *pool.getTextTerminalSymbol("A");
    REQUIRE(&symbol == &symbol2);
  }
  REQUIRE(Symbol::getTotalCount() == 0);

}



TEST_CASE( "TokenTypeTerminalSymbol", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  SECTION("body")
  {
    SymbolPool pool;
    const TerminalSymbol &symbol = *pool.getSpecialTerminalSymbol();
    REQUIRE(symbol.isTerminal()==true);
    REQUIRE(symbol.isConstant()==false);
    REQUIRE(symbol.matches(Token(Token::T_Ident, "A"))==false);
    REQUIRE(symbol.matches(Token(Token::T_Special, "A"))==true);
    REQUIRE(symbol.matches(Token(Token::T_Special, ";"))==true);
    REQUIRE(symbol.getName() == "special");
    REQUIRE(toString(symbol) == "special");
  }
  REQUIRE(Symbol::getTotalCount() == 0);
}

TEST_CASE( "NumberTerminalSymbol", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  SECTION("body")
  {
    SymbolPool pool;
    const TerminalSymbol &symbol = *pool.getNumberTerminalSymbol();
    REQUIRE(symbol.isTerminal()==true);
    REQUIRE(symbol.isConstant()==false);
    REQUIRE(symbol.matches(Token(Token::T_Ident, "A"))==false);
    REQUIRE(symbol.matches(Token(Token::T_Number, "12.34"))==true);
    REQUIRE(symbol.getName() == "number");
    REQUIRE(toString(symbol) == "number");
  }
  REQUIRE(Symbol::getTotalCount() == 0);
}



TEST_CASE( "AnyTerminalSymbol", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  SECTION("body")
  {
    SymbolPool pool;
    const TerminalSymbol &symbol = *pool.getAnyTerminalSymbol();
    REQUIRE(symbol.isTerminal()==true);
    REQUIRE(symbol.isConstant()==false);
    REQUIRE(symbol.matches(Token(Token::T_Ident, "A"))==true);
    REQUIRE(symbol.matches(Token(Token::T_Number, "12.34"))==true);
    REQUIRE(symbol.matches(Token(Token::T_Special, ";"))==true);
    REQUIRE(symbol.matches(Token(Token::T_Eof))==false);
    REQUIRE(symbol.getName() == "any");
    REQUIRE(toString(symbol) == symbol.getName());
  }
  REQUIRE(Symbol::getTotalCount() == 0);
}


TEST_CASE( "Symbols", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  SECTION("body")
  {
    SymbolPool pool;
    const TerminalSymbol *t;
    t = pool.t("hello");
    REQUIRE(t->isTerminal()==true);
    REQUIRE(t->isConstant()==true);
    REQUIRE(t->matches(Token(Token::T_Ident, "hello"))==true);
    REQUIRE(t->matches(Token(Token::T_Number, "12.34"))==false);
    REQUIRE(t->getName() == "'hello'");
    t = pool.eof();
    REQUIRE(t->isTerminal()==true);
    REQUIRE(t->isConstant()==true);
    REQUIRE(t->matches(Token(Token::T_Ident, "hello"))==false);
    REQUIRE(t->matches(Token(Token::T_Number, "12.34"))==false);
    REQUIRE(t->matches(Token(Token::T_Eof))==true);
    REQUIRE(t->getName() == "EOF");
  }
  REQUIRE(Symbol::getTotalCount() == 0);
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
  SECTION("body")
  {
    SymbolPool pool;
    vector<const Symbol*> symbols;
    symbols.push_back(pool.nt("A"));
    symbols.push_back(pool.t("x"));
    Rule rule(pool.nt("A"), symbols, RuleAction ("Group"));
    
    REQUIRE(rule.getId() == -1);
    rule.setId(123); 
    REQUIRE(rule.getId() == 123);
    REQUIRE(rule.getLeftSymbol() == pool.nt("A"));
    REQUIRE(rule.getLength() == 2);
    REQUIRE(rule.getRightSymbol(0) == pool.nt("A"));
    REQUIRE(rule.getRightSymbol(1) == pool.t("x"));
    REQUIRE(rule.getRightSymbol(2) == 0);
    REQUIRE(rule.getAction().getGroupName() == "Group");

    ostringstream ss; ss << rule;
    REQUIRE(ss.str() == "A -> A 'x'");
  }
  REQUIRE(Symbol::getTotalCount() == 0);
}



TEST_CASE( "Rule 2", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  SECTION("body")
  {
    SymbolPool pool;
    vector<const Symbol*> symbols;
    Rule rule(pool.nt("A"), symbols, RuleAction ("Group"));
    REQUIRE(rule.getLength() == 0);
    REQUIRE(rule.getRightSymbol(0) == 0);
    REQUIRE(toString(rule)  == "A ->");    
  }
  REQUIRE(Symbol::getTotalCount() == 0);
}


TEST_CASE( "Grammar 1", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  SECTION("body")
  {    
    Grammar grammar, *g = &grammar;
    REQUIRE(g->getRuleCount()==0);
    vector<const Rule*>rules;
    g->getRulesByLeftSymbol(rules,"A");
    REQUIRE(rules.empty());

    REQUIRE(g->symbols().getNonTerminalSymbol("A", false) == 0);
    REQUIRE(g->symbols().nt("A") != 0);
    REQUIRE(g->symbols().nt("A")->getName() == "A");
    REQUIRE(g->symbols().getNonTerminalSymbol("A", false) != 0);

    const NonTerminalSymbol *A = g->symbols().nt("A");
    vector<const Symbol*> symbols;
    symbols.push_back(g->symbols().t("x"));
    g->addRule(new Rule(A,symbols, RuleAction("")));
    symbols.clear();
    symbols.push_back(g->symbols().nt("A"));
    symbols.push_back(g->symbols().t("x"));
    g->addRule(new Rule(A,symbols, RuleAction("")));
  
    REQUIRE(g->getRuleCount()==3);
    REQUIRE(toString(*g->getRule(0))=="_root -> A EOF");
    REQUIRE(toString(*g->getRule(1))=="A -> 'x'");
    REQUIRE(toString(*g->getRule(2))=="A -> A 'x'");

    g->getRulesByLeftSymbol(rules,"A");
    REQUIRE(rules.size() == 2);
  }
  REQUIRE(Symbol::getTotalCount() == 0);  
}

TEST_CASE( "Grammar 2", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  Grammar *g = new Grammar();
  RuleBuilder(g,"Sum").n("Var").end();
  RuleBuilder(g,"Sum").n("Sum").t("+").n("Var").end();
  RuleBuilder(g,"Var").t("x").end();
  RuleBuilder(g,"Var").t("y").end();
  RuleBuilder(g,"Var").t("z").end();

  REQUIRE(g->symbols().getNonTerminalSymbol("Sum",false) != 0);
  REQUIRE(g->symbols().getNonTerminalSymbol("Var",false) != 0);

  REQUIRE(g->getRuleCount()==6);
  REQUIRE(toString(*g->getRule(0)) == "_root -> Sum EOF");
  REQUIRE(toString(*g->getRule(1)) == "Sum -> Var");
  REQUIRE(toString(*g->getRule(2)) == "Sum -> Sum '+' Var");
  REQUIRE(toString(*g->getRule(3)) == "Var -> 'x'");
  REQUIRE(toString(*g->getRule(4)) == "Var -> 'y'");
  REQUIRE(toString(*g->getRule(5)) == "Var -> 'z'");
  
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


TEST_CASE( "Grammar 3", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  Grammar *g = new Grammar();

  RuleBuilder(g,"A").t("x").end();
  REQUIRE(g->getRuleCount()==2);
  REQUIRE(g->getRule(0)->getLeftSymbol()==g->getRootSymbol());
  REQUIRE(g->getRootSymbol()!=0);
  REQUIRE(g->getRootSymbol()->getName() == "_root");

  g->clear();
  RuleBuilder(g,"A").t("x").end();
  REQUIRE(g->getRuleCount()==2);
  REQUIRE(g->getRule(0)->getLeftSymbol()==g->getRootSymbol());
  REQUIRE(g->getRootSymbol()!=0);
  REQUIRE(g->getRootSymbol()->getName() == "_root");
  

  delete g;
  REQUIRE(Symbol::getTotalCount() == 0);  
}

TEST_CASE( "Nullables 1", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  Grammar *g = new Grammar();

  REQUIRE(g->getNullables().empty());

  RuleBuilder(g,"S").t("a").n("A").end();
  RuleBuilder(g,"S").t("b").n("B").end();
  RuleBuilder(g,"S").t("c").n("C").end();
  RuleBuilder(g,"S").t("d").n("D").end();
  RuleBuilder(g,"S").t("e").n("E").end();
  RuleBuilder(g,"S").t("f").n("F").end();

  RuleBuilder(g,"F").n("B").n("D").n("E").end();
  RuleBuilder(g,"F").t("0").end();

  RuleBuilder(g,"A").t("1").end();
  RuleBuilder(g,"A").t("2").end();

  RuleBuilder(g,"B").t("3").end();
  RuleBuilder(g,"B").end();

  RuleBuilder(g,"C").t("4").end();
  RuleBuilder(g,"C").n("B").t("5").end();

  RuleBuilder(g,"D").t("6").end();
  RuleBuilder(g,"D").n("B").n("B").n("B").end();

  RuleBuilder(g,"E").t("7").end();

  REQUIRE(!g->getNullables().empty());
  REQUIRE(!g->isNullable("A"));
  REQUIRE(g->isNullable("B"));
  REQUIRE(!g->isNullable("C"));
  REQUIRE(g->isNullable("D"));
  REQUIRE(!g->isNullable("E"));
  REQUIRE(!g->isNullable("F"));

  RuleBuilder(g,"E").n("B").n("D").end();
  
  REQUIRE(!g->isNullable("A"));
  REQUIRE(g->isNullable("B"));
  REQUIRE(!g->isNullable("C"));
  REQUIRE(g->isNullable("D"));
  REQUIRE(g->isNullable("E"));
  REQUIRE(g->isNullable("F"));

  g->clear();

  RuleBuilder(g,"A").t("a").end();
  RuleBuilder(g,"A").end();
  RuleBuilder(g,"B").n("B").n("A").end();
  RuleBuilder(g,"B").t("b").end();
  
  REQUIRE(g->isNullable("A"));
  REQUIRE(!g->isNullable("B"));

  delete g;
  REQUIRE(Symbol::getTotalCount() == 0);  
}


TEST_CASE( "First sets 1", "[grammar]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  Grammar *g = new Grammar();


  RuleBuilder(g,"S").t("p").end();
  RuleBuilder(g,"S").n("A").n("B").end();
  RuleBuilder(g,"B").t("x").end();
  RuleBuilder(g,"A").t("y").end();
  RuleBuilder(g,"A").end();

  const set<const TerminalSymbol*> &firstSet = 
    g->getFirstSet("S");

  delete g;
  REQUIRE(Symbol::getTotalCount() == 0);  
}
