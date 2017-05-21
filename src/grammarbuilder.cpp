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
    TerminalSymbol *number = g->addTerminal(
      new TokenTypeTerminalSymbol("number", Token::T_Number));
    TerminalSymbol *qstring = g->addTerminal(
      new TokenTypeTerminalSymbol("qstring", Token::T_QuotedString));
    TerminalSymbol *eol = g->addTerminal(
      new TokenTypeTerminalSymbol("EOL", Token::T_Eol));
    TerminalSymbol *eof = g->addTerminal(
      new TokenTypeTerminalSymbol("EOF", Token::T_Eof));

    RuleBuilder(g,"Stm")
      .t(ident).t("-").t(">").n("right").n("action").t(eol).end(RuleAction("Rule", 0x19));

    RuleBuilder(g,"StmLst").n("Stm").end(RuleAction("List"));
    RuleBuilder(g,"StmLst").n("StmLst").n("Stm").end(RuleAction("List"));
    
    RuleBuilder(g,"right").end(RuleAction("Right"));
    RuleBuilder(g,"right").n("right").t(ident).end(RuleAction("Right"));
    RuleBuilder(g,"right").n("right").t(qstring).end(RuleAction("Right"));
    
    RuleBuilder(g,"action").end();
    RuleBuilder(g,"action").t(":").t(ident).end(RuleAction("Action",0x2));
    RuleBuilder(g,"action").t(":").t(ident).t("(").n("IntLst").t(")").end(RuleAction("Action",0xA));

    RuleBuilder(g,"IntLst").t(number).end(RuleAction("IntLst"));
    RuleBuilder(g,"IntLst").n("IntLst").t(",").t(number).end(RuleAction("IntLst",0x5));

    RuleBuilder(g,"S").n("StmLst").t(eof).end();


    return g;
  }


} // namespace


GrammarDefinitionParser::GrammarDefinitionParser()
  : Parser(makeGrammarDefinitionGrammar(), "S")
{
  getParseTable().dump();
}


GrammarDefinitionParser::~GrammarDefinitionParser()
{
  // orrendo!!
  delete const_cast<Grammar*>(getGrammar());
}


Grammar *GrammarBuilder::build(BaseTokenizer *tokenizer)
{
  Grammar *grammar  = new Grammar();
  GrammarDefinitionParser parser;

  bool ret = parser.parse(tokenizer);
  if(!ret) return 0;

  const ParseTree *ptree = parser.getParseTree();

  assert(ptree->getStackSize()==1);
  ParseNode rulesNode = ptree->getNode(0);
  assert(rulesNode.getTag()=="List");
  for(int i=0;i<rulesNode.getChildCount();i++)
  {
    ParseNode ruleNode = rulesNode.getChild(i);
    cout << ruleNode.getTag() << endl;
    string ntName = ruleNode.getChild(0).getToken().getText();
    vector<Symbol*> symbols;

    ParseNode right = ruleNode.getChild(1);
    for(int j=0; j<right.getChildCount(); j++) 
    {      
      ParseNode item = right.getChild(j);
      if(item.getToken().getType() == Token::T_Ident) 
      {
        symbols.push_back(grammar->getNonTerminal(item.getToken().getText()));
      }
      else
      {
        string text = item.getToken().getText();
        text = text.substr(1,text.length()-2);
        symbols.push_back(grammar->addTerminal(new TextTerminalSymbol(text)));
      }
    }

    ParseNode action = ruleNode.getChild(2);


    Rule *newRule = new Rule(grammar->getNonTerminal(ntName), symbols, RuleAction("uffa"));
    grammar->addRule(newRule);

  }

  return grammar;

}
