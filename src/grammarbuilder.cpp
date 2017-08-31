#include "grammarbuilder.h"
#include <iostream>
#include <assert.h>

using namespace std;

namespace {

  Grammar *makeGrammarDefinitionGrammar()
  {
    Grammar *g = new Grammar();
    RuleBuilder(g,"StmLst").end(RuleAction("List"));
    RuleBuilder(g,"StmLst").n("StmLst").eol().end(RuleAction("pass",1));
    RuleBuilder(g,"StmLst").n("StmLst").n("Stm").eol().end(RuleAction("List",1+2));

    RuleBuilder(g,"Stm")
      .id().t("-").t(">").n("right").n("action").end(RuleAction("Rule", 0x19));
    
    RuleBuilder(g,"right").end(RuleAction("Right"));
    RuleBuilder(g,"right").n("right").id().end(RuleAction("Right"));
    RuleBuilder(g,"right").n("right").qstring().end(RuleAction("Right"));
    
    RuleBuilder(g,"action").end();
    RuleBuilder(g,"action").t(":").t("pass").end(RuleAction("Action",0x2));
    RuleBuilder(g,"action").t(":").t("pass").t("(").number().t(")").end(RuleAction("Action",0xA));
    RuleBuilder(g,"action").t(":").t("null").end(RuleAction("Action",0x2));
    RuleBuilder(g,"action").t(":").id().t("(").n("IntLst").t(")").end(RuleAction("Action",0xA));
    RuleBuilder(g,"action").t(":").id().end(RuleAction("Action",0x2));

    RuleBuilder(g,"IntLst").number().end(RuleAction("IntLst"));
    RuleBuilder(g,"IntLst").n("IntLst").t(",").number().end(RuleAction("IntLst",0x5));

    return g;
  }


} // namespace


GrammarDefinitionParser::GrammarDefinitionParser()
  : Parser(makeGrammarDefinitionGrammar())
{
  // getParseTable().dump();
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

  // ptree->dump(cout);

  assert(ptree->getStackSize()==1);
  ParseNode rulesNode = ptree->getNode(0);
  assert(rulesNode.getTag()=="List");
  for(int i=0;i<rulesNode.getChildCount();i++)
  {
    ParseNode ruleNode = rulesNode.getChild(i);
    // cout << ruleNode.getTag() << endl;
    string ntName = ruleNode.getChild(0).getToken().getText();
    if(ntName == "") continue;
    vector<const Symbol*> symbols;

    ParseNode right = ruleNode.getChild(1);
    for(int j=0; j<right.getChildCount(); j++) 
    {      
      ParseNode item = right.getChild(j);
      if(item.getToken().getType() == Token::T_Ident) 
      {
        if(item.getToken().getText() == "ident")
        {
          symbols.push_back(grammar->symbols().getIdentTerminalSymbol());
        }
        else if(item.getToken().getText() == "int")
        {
          symbols.push_back(grammar->symbols().getNumberTerminalSymbol());
        }
        else if(item.getToken().getText() == "qstring")
        {
          symbols.push_back(grammar->symbols().getQuotedStringTerminalSymbol());
        }
        else if(item.getToken().getText() == "any")
        {
          symbols.push_back(grammar->symbols().getAnyTerminalSymbol());
        }
        else
          symbols.push_back(grammar->symbols().nt(item.getToken().getText()));
      }
      else
      {
        string text = item.getToken().getText();
        text = text.substr(1,text.length()-2);
        symbols.push_back(grammar->symbols().t(text));
      }
    }

    RuleAction ruleAction;

    ParseNode action = ruleNode.getChild(2);
    if(action.getChildCount() > 0)
    {
      string actionName = action.getChild(0).getToken().getText();
      int mask = 0;
      if(action.getChildCount()==2)
      {
        ParseNode argLst = action.getChild(1);
        for(int i=0;i<argLst.getChildCount();i++)
        {
          ParseNode arg = argLst.getChild(i);
          string v = arg.getToken().getText();
          int k = stoi(v, 0) - 1;
          mask |= 1<<k;
        }
      }
      ruleAction = RuleAction(actionName, mask);
    }
    else
    {
      ruleAction = RuleAction(ntName, (1<<symbols.size())-1);
    }

    Rule *newRule = new Rule(grammar->symbols().nt(ntName), symbols, ruleAction);
    grammar->addRule(newRule);
  }

  return grammar;
}
