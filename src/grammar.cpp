#include "grammar.h"
#include <sstream>


using namespace std;


int Symbol::m_totalCount = 0;

//=============================================================================

std::ostream &operator<<(std::ostream &out, const Symbol &nt) 
{ 
  return out << nt.getName().c_str(); 
}

std::ostream &operator<<(std::ostream &out, const Rule &rule) { 
  out << *rule.getLeftSymbol() << " ->";
  for(int i=0; i<rule.getLength(); i++) out << " " << *rule.getRightSymbol(i); 
  return out;
}

//=============================================================================

KeywordsTerminalSymbol::KeywordsTerminalSymbol(const std::vector<std::string> &keywords) 
  : TerminalSymbol("")
  , m_keywords(keywords)
{
  buildName();
}

/*
KeywordsTerminalSymbol(Grammar *grammar, const std::string &kw1, const std::string &kw2);
KeywordsTerminalSymbol(Grammar *grammar, const std::string &kw1, const std::string &kw2, const std::string &kw3);
KeywordsTerminalSymbol(Grammar *grammar, const std::string &kw1, const std::string &kw2, const std::string &kw3, const std::string &kw4);

*/

KeywordsTerminalSymbol::KeywordsTerminalSymbol(const std::string &kw1, const std::string &kw2)
  : TerminalSymbol("")
{
  m_keywords.push_back(kw1);
  m_keywords.push_back(kw2);
  buildName();
}

KeywordsTerminalSymbol::KeywordsTerminalSymbol(const std::string &kw1, const std::string &kw2, const std::string &kw3)
  : TerminalSymbol("")
{
  m_keywords.push_back(kw1);
  m_keywords.push_back(kw2);
  m_keywords.push_back(kw3);
  buildName();
}

KeywordsTerminalSymbol::KeywordsTerminalSymbol(const std::string &kw1, const std::string &kw2, 
                                               const std::string &kw3, const std::string &kw4)
  : TerminalSymbol("")
{
  m_keywords.push_back(kw1);
  m_keywords.push_back(kw2);
  m_keywords.push_back(kw3);
  m_keywords.push_back(kw4);
  buildName();
}


void KeywordsTerminalSymbol::buildName()
{
  ostringstream os;
  os << "[";
  for(vector<string>::iterator it = m_keywords.begin(); it != m_keywords.end(); ++it)
  {
    if(it != m_keywords.begin()) os << ", ";
    os << "'" << *it << "'";
  }
  os << "]";
  setName(os.str());
}



bool KeywordsTerminalSymbol::matches(const Token &token) const
{
  for(vector<string>::const_iterator it = m_keywords.begin(); it != m_keywords.end(); ++it)
  {
    if(*it == token.getText()) return true;
  }
  return false;
}


//=============================================================================

Grammar::Grammar()
{
}

Grammar::~Grammar()
{
  clear();
}

void Grammar::clear()
{
  using namespace std;
  for(map<string, NonTerminalSymbol*>::iterator it=m_ntSymbols.begin(); it != m_ntSymbols.end(); ++it) delete it->second;
  m_ntSymbols.clear();
  for(vector<TerminalSymbol*>::iterator it=m_tSymbols.begin(); it != m_tSymbols.end(); ++it) delete *it;
  m_tSymbols.clear();
  for(vector<Rule*>::iterator it=m_rules.begin(); it != m_rules.end(); ++it) delete *it;
  m_rules.clear();
}


NonTerminalSymbol* Grammar::getNonTerminal(const std::string &name, bool createIfNeeded)
{
  using namespace std;
  map<string, NonTerminalSymbol*>::iterator it=m_ntSymbols.find(name);
  if(it != m_ntSymbols.end()) return it->second;
  else if(!createIfNeeded) return 0;
  NonTerminalSymbol*nt = new NonTerminalSymbol(name);
  m_ntSymbols[name] = nt;
  return nt;
}

TerminalSymbol* Grammar::addTerminal(TerminalSymbol*t)
{
  m_tSymbols.push_back(t);
  return t;
}


/*
class RuleBuilder {
  Grammar *m_grammar;
public:
  RuleBuilder(Grammar *grammar) : m_grammar(grammar) {}

  Symbol *getSymbol(const vector<Token> &tokens, int &pos);



};

Symbol *RuleBuilder::getSymbol(const vector<Token> &tokens, int &pos)
{
  Token token = tokens[pos++];
  if(token.getType() == Token::T_Ident)
  {
    if(token.getText() == "number")
      return m_grammar->addTerminal(new NumberTerminalSymbol(m_grammar));
    else if(token.getText() == "ident")
      return m_grammar->addTerminal(new TokenTypeTerminalSymbol(m_grammar, "ident",Token::T_Ident));
    else if(token.getText() == "any")
      return m_grammar->addTerminal(new AnyTerminalSymbol(m_grammar));
    else if(token.getText() == "EOF")
      return m_grammar->addTerminal(new TokenTypeTerminalSymbol(m_grammar, "EOF", Token::T_Eof));
    else
      return m_grammar->getNonTerminal(token.getText());
  }
  else if(token.getType() == Token::T_Special)
  {
    if(token.getText() == "[")
    {
      vector<string> keywords;
      while(pos<(int)tokens.size())
      {
        token = tokens[pos++];
        if(token.getText() == "]") break;
        string text = token.getText();
        if(token.getType() == Token::T_QuotedString)
          text = text.substr(1,text.length()-2);
        keywords.push_back(text);
      }
      if(keywords.empty()) return 0;
      return m_grammar->addTerminal(new KeywordsTerminalSymbol(m_grammar, keywords));
    }
    else
      return m_grammar->addTerminal(new TextTerminalSymbol(m_grammar, token.getText()));
  }
  else if(token.getType() == Token::T_QuotedString)
  {
    string text = token.getText();
    text = text.substr(1,text.length()-2);
    return m_grammar->addTerminal(new TextTerminalSymbol(m_grammar, text));
  }
  else
    return 0;
}

*/
/*

Symbol *Grammar::getSymbol(ITokenizer *tokenizer)
{

}
*/

const Rule *Grammar::addRule(Rule *rule)
{
  rule->setId(m_rules.size());
  m_rules.push_back(rule);
  return rule;
}


/*
const Rule *Grammar::createRule(const std::string &left, const std::string &right, const std::string &action)
{
  using namespace std;
  vector<Symbol*> rightSymbols;
  StringTokenizer st(right);
  const vector<Token> &tokens = st.getTokens();
  RuleBuilder rb(this);
  int pos = 0;
  while(pos<(int)tokens.size() && !tokens[pos].isEof())
  {
    Symbol*symbol = rb.getSymbol(tokens, pos);
    if(!symbol) return 0;
    rightSymbols.push_back(symbol);
  }
  Rule *rule = new Rule(getNonTerminal(left), rightSymbols);
  rule->setId(m_rules.size());
  m_rules.push_back(rule);
  rule->setAction(action);
  return rule;
}
*/

void Grammar::getRulesByLeftSymbol(std::vector<const Rule*> &rules, const std::string &leftSymbolName) const
{
  rules.clear();
  for(vector<Rule*>::const_iterator it=m_rules.begin(); it != m_rules.end(); ++it) 
  {
    Rule *rule = *it;
    if(rule->getLeftSymbol()->getName() == leftSymbolName) 
      rules.push_back(rule);
  }
}


RuleBuilder::RuleBuilder(Grammar *g, const std::string &leftSymbolName)
  : m_grammar(g)
  , m_leftSymbol(g->getNonTerminal(leftSymbolName))
{
}

RuleBuilder &RuleBuilder::t(const std::string &t)
{
  Symbol *symbol = m_grammar->addTerminal(new TextTerminalSymbol(t));
  m_symbols.push_back(symbol);
  return *this;
}

RuleBuilder &RuleBuilder::t(TerminalSymbol *terminal)
{
  m_symbols.push_back(terminal);
  return *this;
}


/*
RuleBuilder &RuleBuilder::t(Token::Type ty)
{
  Symbol *symbol = m_grammar->addTerminal(new TokenTypeTerminalSymbol(m_grammar, ty));
  m_symbols.push_back(symbol);
  return *this;
}
*/

RuleBuilder &RuleBuilder::n(const std::string &ntName)
{
  m_symbols.push_back(m_grammar->getNonTerminal(ntName));
  return *this;
}

const Rule *RuleBuilder::end(const RuleAction &action)
{
  Rule *rule = new Rule(m_leftSymbol, m_symbols, action);
  return m_grammar->addRule(rule);
}


