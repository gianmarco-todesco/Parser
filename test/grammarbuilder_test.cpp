#include "grammarbuilder.h"
#include "catch.hpp"
#include <sstream>
#include <iostream>

using namespace std;



TEST_CASE( "GrammarDefinitionGrammar", "[ugh]") {
  /*
  REQUIRE(Symbol::getTotalCount()==0);
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

    TerminalSymbol *eof = g->addTerminal(
      new TokenTypeTerminalSymbol("EOF", Token::T_Eof));
    RuleBuilder(g,"S").n("A").t(eof).end();

    for(int i=0;i<g->getRuleCount();i++)
    {
      const Rule *rule = g->getRule(i);
      cout << *rule << endl;
    }

    StringTokenizer st2("x y z z");
    Parser parser(g, "S");
    bool ret = parser.parse(&st2);
    if(ret) 
    {
      parser.getParseTree()->dump(cout);
    }

    delete g;
  }
  REQUIRE(Symbol::getTotalCount()==0);
  */


  /*
  GrammarDefinitionParser parser;
  const Grammar *grammar = parser.getGrammar();
  for(int i=0;i<grammar->getRuleCount(); i++)
  {
    const Rule *rule = grammar->getRule(i);
    cout << *rule << endl;
  }

  bool ret = parser.parse(&st);
  if(ret) 
    cout << parser.getOutput()->toString().c_str() << endl;
    */

}
