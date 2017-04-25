#include "grammarbuilder.h"
#include <iostream>
#include <assert.h>

using namespace std;

namespace {

  Grammar *makeGrammarDefinitionGrammar()
  {
    Grammar *g = new Grammar();
    TerminalSymbol *ident = g->addTerminal(
      new TokenTypeTerminalSymbol("ident", Token::T_Ident));
    TerminalSymbol *qstring = g->addTerminal(
      new TokenTypeTerminalSymbol("qstring", Token::T_QuotedString));
    TerminalSymbol *eol = g->addTerminal(
      new TokenTypeTerminalSymbol("EOL", Token::T_Eol));
    TerminalSymbol *eof = g->addTerminal(
      new TokenTypeTerminalSymbol("EOF", Token::T_Eof));

    RuleBuilder(g,"Stm")
      .t(ident)
      .t("-").t(">")
      .n("right")
      .n("action")
      .t(eol).end(RuleAction("Rule"));

    RuleBuilder(g,"StmLst").n("Stm").end(RuleAction("List"));
    RuleBuilder(g,"StmLst").n("StmLst").n("Stm").end(RuleAction("List"));
    
    RuleBuilder(g,"right").end(RuleAction("Right"));
    RuleBuilder(g,"right").n("right").t(ident).end(RuleAction("Right"));
    RuleBuilder(g,"right").n("right").t(qstring).end(RuleAction("Right"));
    
    RuleBuilder(g,"action").end();
    
    RuleBuilder(g,"S").n("StmLst").t(eof).end();


    return g;
  }


} // namespace


GrammarDefinitionParser::GrammarDefinitionParser()
  : Parser(makeGrammarDefinitionGrammar(), "S")
{

}


GrammarDefinitionParser::~GrammarDefinitionParser()
{
  // orrendo!!
  delete const_cast<Grammar*>(getGrammar());
}


Grammar *GrammarBuilder::build(BaseTokenizer *tokenizer)
{
  GrammarDefinitionParser parser;

  bool ret = parser.parse(tokenizer);
  if(!ret) return 0;

  const ParseOutput *po = parser.getOutput();
  assert(po->getCount()==1);
  ParseOutputNode rules = po->getNode(0);
  assert(rules.getGroupName()=="List");
  for(int i=0;i<rules.getChildCount();i++)
  {
    cout << "rule" << endl;

  }

  return 0;

}
