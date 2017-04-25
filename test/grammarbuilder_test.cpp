#include "grammarbuilder.h"
#include "catch.hpp"
#include <sstream>
#include <iostream>

using namespace std;



TEST_CASE( "GrammarDefinitionGrammar", "[ugh]") {

  REQUIRE(Symbol::getTotalCount()==0);
  {
    StringTokenizer st("A -> 'x'\nA -> A 'x'\n");

    GrammarBuilder gb;
    gb.build(&st);
  }
  REQUIRE(Symbol::getTotalCount()==0);
  
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
