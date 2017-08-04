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
  m_rootSymbol = new NonTerminalSymbol("_root");
  m_ntSymbols[m_rootSymbol->getName()] = m_rootSymbol;
  m_eofSymbol = new TokenTypeTerminalSymbol("EOF", Token::T_Eof);
  m_tSymbols.push_back(m_eofSymbol);
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
  m_rootSymbol = 0;
  m_eofSymbol = 0;
}

void Grammar::updateRootRule(const NonTerminalSymbol *startSymbol)
{
  vector<const Symbol*> right; right.push_back(startSymbol); right.push_back(m_eofSymbol);
  Rule *rule = new Rule(m_rootSymbol, right, RuleAction("pass",1));
  rule->setId(0);
  if(m_rules.empty()) m_rules.push_back(rule);
  else 
  {
    delete m_rules[0];
    m_rules[0] = rule;
  }
}


const NonTerminalSymbol* Grammar::getNonTerminal(const std::string &name, bool createIfNeeded)
{
  using namespace std;
  map<string, NonTerminalSymbol*>::iterator it=m_ntSymbols.find(name);
  if(it != m_ntSymbols.end()) return it->second;
  else if(!createIfNeeded) return 0;
  NonTerminalSymbol*nt = new NonTerminalSymbol(name);
  m_ntSymbols[name] = nt;
  return nt;
}

const TerminalSymbol* Grammar::addTerminal(TerminalSymbol*t)
{
  m_tSymbols.push_back(t);
  return t;
}

const Rule *Grammar::addRule(Rule *rule)
{
  if(m_rules.empty()) updateRootRule(rule->getLeftSymbol());
  rule->setId(m_rules.size());
  m_rules.push_back(rule);
  return rule;
}

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


const NonTerminalSymbol *Grammar::getStartSymbol() const
{
  if(m_rules.empty()) return 0;
  else return static_cast<const NonTerminalSymbol *>(m_rules[0]->getRightSymbol(0));
}

void Grammar::setStartSymbol(const NonTerminalSymbol*nt)
{
  updateRootRule(nt);
}


void Grammar::dump(ostream &out) const
{
  for(int i=0;i<getRuleCount();i++)
  {
    const Rule *rule = getRule(i);
    out << *rule << endl;
  }
}

//=============================================================================


RuleBuilder::RuleBuilder(Grammar *g, const std::string &leftSymbolName)
  : m_grammar(g)
  , m_leftSymbol(g->getNonTerminal(leftSymbolName))
{
}

RuleBuilder &RuleBuilder::t(const std::string &t)
{
  const Symbol *symbol = m_grammar->addTerminal(new TextTerminalSymbol(t));
  m_symbols.push_back(symbol);
  return *this;
}

RuleBuilder &RuleBuilder::t(const TerminalSymbol *terminal)
{
  m_symbols.push_back(terminal);
  return *this;
}

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


