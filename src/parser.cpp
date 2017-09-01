#include "parser.h"
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <assert.h>


using namespace std;


Parser::Parser(const Grammar *grammar)
  : m_grammar(grammar)
  , m_parseTable(grammar)
  , m_parseTree(0)
  , m_tokenizer(0)
  , m_skipNewLines(false)
  , m_trace(false)
{
}

std::string p(const Token &token)
{
  if(token.isEof()) return "EOF";
  else if(token.isEol()) return "EOL";
  else if(token.getType() == Token::T_Ident || token.getType() == Token::T_Special || token.getType() == Token::T_Number) return "'" + token.getText() + "'";
  else if(token.getType() == Token::T_QuotedString) return "q" + token.getText();
  else return "?" + token.getText() + "?";
}

bool Parser::parse(const BaseTokenizer *tokenizer)
{
  const vector<Token> &tokens = tokenizer->getTokens();

  delete m_parseTree;
  m_parseTree = new ParseTree(tokenizer);

  m_stack.clear();
  m_stack.push_back(m_parseTable.getFirstState());
  int pos = 0;
  bool ret = false;
  for(;;)
  {
    const ParserState*state = m_stack.back();

    if(m_skipNewLines)
      skipNewLinesAndComments(tokens, pos, state->matchesEol() ? 1 : 0);

    const Token &token = tokens[pos] ;

    vector<const Rule*> completedRules;
    vector<const TerminalSymbol*> terminals;
    state->getCompleteRules(completedRules, token);

    
    state->getTerminals(terminals, token);
    
    if(terminals.size()>0)
    {
      //if(matchedTerminals.size()>1) 
      //  cout << "Uh oh 1" << endl;
      
      if(completedRules.size()>0) 
      {
        //cout << "Shift-reduce conflict" << endl;
        //cout << "  State S" << state->getId() << endl;
        //cout << "  shift : " << *matchedTerminals[0] << endl;
        //cout << "  reduce : " << *completedRules[0] << endl;
        // tokenizer->dumpPosition(cout, pos);
      }
      const ParserState*nextState = state->getNextState(terminals[0]);
      assert(nextState);
      m_stack.push_back(nextState);
      m_parseTree->addLeaf(pos);
      if(m_trace) 
      {
        cout << "Shift " << p(tokens[pos]);
        if(!terminals[0]->isConstant()) 
          cout << " (" << terminals[0]->getName() << ")" ;
        cout << endl;
      }
      if(!tokens[pos].isEof()) pos++;
    }
    else if(completedRules.size()>0)
    {
      if(completedRules.size()>1) 
      {
        cout << "Reduce-reduce conflict" << endl;
        for(int i=0;i<(int)completedRules.size();i++)
          cout << "Reduce " << *completedRules[i] << endl;
      }

      const Rule *rule;      
      rule = completedRules[0];

      if(m_trace) cout << "Reduce " << *rule << endl;
      doSemanticAction(rule);

      for(int i=0;i<rule->getLength();i++) m_stack.pop_back();
      if(!m_stack.empty())
      {
        state = m_stack.back();
        const ParserState*nextState = state->getNextState(rule->getLeftSymbol());
        assert(nextState || state->getId()==0);
        if(nextState) m_stack.push_back(nextState);
        else { ret = true; break; }
      }
    }
    else
    {
      cout << "Syntax error" << endl;
      cout << "expected one of:";
      vector<const TerminalSymbol*> expected;
      state->getExpected(expected);
      for(int i=0;i<(int)expected.size();i++) cout << expected[i]->getName() << " ";
      cout << endl;
      cout << "Found " << tokenizer->getTokens()[pos] << endl;
      tokenizer->dumpPosition(cout, pos);

      state->dump();

      break;
    }    
  }

  return ret;
}


void Parser::doSemanticAction(const Rule *rule)
{
  ostringstream ss;
  ss<<*rule;
  string text = ss.str();

  if(rule->getAction().getGroupName() == "")
  {
    int count = rule->getLength();
    int i;
    for(i=0;i<count && rule->getRightSymbol(i)->isConstant();i++) {}
    if(i>=count)
    {
      // no non-terminals in right-side. return "null"
      m_parseTree->makeNode("_nil", count, 0);
    }
    else
      m_parseTree->takeOne(count, i);
  }
  else if(rule->getAction().getGroupName() == "pass")
  {
    int mask = rule->getAction().getMask();
    if(mask==0 && rule->getLength()==1) mask = 1;
    vector<int> fields;
    int i=0;
    while(mask!=0) 
    {
      if(mask&1) fields.push_back(i);
      mask>>=1;
      i++;
    }
    if(fields.size()>0)
      m_parseTree->takeOne(rule->getLength(), fields[0]);
    else
      m_parseTree->makeNode("_nil", rule->getLength(), 0);
  }
  else if(rule->getAction().getGroupName() == "null")
  {
    m_parseTree->makeNode("_nil", rule->getLength(), 0);
  }
  else
  {
    const RuleAction &action = rule->getAction();
    m_parseTree->makeNode(action.getGroupName(), rule->getLength(),action.getMask());
  }
}


void Parser::skipNewLinesAndComments(const std::vector<Token> &tokens, int &pos, int flags)
{
  const Token slash(Token::T_Special, "/");
  const Token star(Token::T_Special, "*");
  int n = (int)tokens.size();

  while(pos < n)
  {
    if(flags == 0 && tokens[pos].isEol()) pos++;
    else if(pos+1<n && tokens[pos] == slash)
    {
      if(tokens[pos+1] == slash) 
      {
        // line comment
        pos += 2;
        while(pos<n && !tokens[pos].isEol()) pos++;
        if(pos<n) pos++;
      }
      else if(tokens[pos+1] == star) 
      {
        // block comment
        pos += 2;
        while(pos+1<n && !(tokens[pos] == star && tokens[pos+1] == slash)) pos++;
        if(pos+1<n) pos+=2;
      }
      else
        break;
    }
    else break;
   
  }
}


