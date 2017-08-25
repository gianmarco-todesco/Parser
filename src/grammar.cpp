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
/*
KeywordsTerminalSymbol::KeywordsTerminalSymbol(const std::vector<std::string> &keywords) 
  : TerminalSymbol("")
  , m_keywords(keywords)
{
  buildName();
}


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

*/
//=============================================================================


SymbolPool::SymbolPool()
{
}

SymbolPool::~SymbolPool()
{
  for(map<std::string, NonTerminalSymbol*>::iterator it = m_ntSymbols.begin(); it != m_ntSymbols.end(); ++it)
    delete it->second;
  for(map<std::string, TerminalSymbol*>::iterator it = m_tSymbols.begin(); it != m_tSymbols.end(); ++it)
    delete it->second;
  m_ntSymbols.clear();
  m_tSymbols.clear();  
}

const NonTerminalSymbol* SymbolPool::getNonTerminalSymbol(const std::string &name, bool createIfNeeded)
{
  map<std::string, NonTerminalSymbol*>::iterator it = m_ntSymbols.find(name);
  if(it != m_ntSymbols.end()) return it->second;
  if(createIfNeeded)
  {
    NonTerminalSymbol*nt = new NonTerminalSymbol(name);
    m_ntSymbols[name] = nt;
    return nt;
  }
  else
  {
    return 0;
  }
}

TerminalSymbol* SymbolPool::getTerminalSymbol(const std::string &name)
{
  map<std::string, TerminalSymbol*>::iterator it = m_tSymbols.find(name);
  if(it != m_tSymbols.end()) return it->second;
  else return 0;
}

TerminalSymbol* SymbolPool::getTokenTypeTerminalSymbol(const std::string &name, Token::Type type)
{
  map<std::string, TerminalSymbol*>::iterator it = m_tSymbols.find(name);
  if(it != m_tSymbols.end()) return it->second;
  TokenTypeTerminalSymbol *t = new TokenTypeTerminalSymbol(name, type);
  m_tSymbols[name] = t;
  return t;
}

const TerminalSymbol* SymbolPool::getTextTerminalSymbol(const std::string &text, bool createIfNeeded)
{
  string name = "'" + text + "'";
  TerminalSymbol *t = getTerminalSymbol(name);
  if(!t && createIfNeeded)
  {
    t = new TextTerminalSymbol(text, name);
    m_tSymbols[name] = t;
  }
  return t;
}

const TerminalSymbol* SymbolPool::getAnyTerminalSymbol()
{
  string name = "any";
  TerminalSymbol *t = getTerminalSymbol(name);
  if(t) return t;
  t = new AnyTerminalSymbol();
  m_tSymbols[name] = t;
  return t;
}

void SymbolPool::getNonTerminalSymbols(std::vector<const NonTerminalSymbol*> &nts) const
{
  map<string,NonTerminalSymbol*>::const_iterator it;
  for(it = m_ntSymbols.begin(); it != m_ntSymbols.end(); ++it)
    nts.push_back(it->second);
}

void SymbolPool::getNonTerminalSymbolNames(std::vector<std::string> &nts) const
{
  map<string,NonTerminalSymbol*>::const_iterator it;
  for(it = m_ntSymbols.begin(); it != m_ntSymbols.end(); ++it)
    nts.push_back(it->second->getName());
}

//=============================================================================

class NullablesBuilder {
  const Grammar *m_grammar;
  set<string> &m_nullables;
  vector<const Rule*> m_candidates;
public:
  NullablesBuilder(const Grammar *grammar, set<string> &nullables) 
    : m_grammar(grammar)
    , m_nullables(nullables)
  {
    m_nullables.clear();
    initialize();
    build();
  }
private:
  void initialize();
  bool hasTerminals(const Rule *rule) const {
    for(int i=0;i<rule->getLength();i++)
      if(rule->getRightSymbol(i)->isTerminal()) return true;
    return false;
  }
  bool isNullable(const string &name) const { 
    return m_nullables.find(name) != m_nullables.end(); 
  }
  bool isNullable(const Rule *rule) const { 
    for(int i=0;i<rule->getLength();i++)
    {
      const Symbol *symbol = rule->getRightSymbol(i);
      if(symbol->isTerminal() || !isNullable(symbol->getName()))
        return false;
    }
    return true;
  }
  void build();
};

void NullablesBuilder::initialize()
{
  for(int i=0;i<m_grammar->getRuleCount();i++)
  {
    const Rule *rule = m_grammar->getRule(i);
    if(rule->getLength()==0) 
      m_nullables.insert(rule->getLeftSymbol()->getName());
    else if(!hasTerminals(rule))
      m_candidates.push_back(rule);
  }
}

void NullablesBuilder::build()
{
  bool found = true;
  while(found)
  {
    found = false;
    vector<const Rule*>::iterator it1,it2;
    for(it1=it2=m_candidates.begin(); it1 != m_candidates.end(); ++it1)
    {
      const Rule *rule = *it1;
      string leftSymbolName = rule->getLeftSymbol()->getName();
      if(isNullable(leftSymbolName))
      {
        // left symbol is nullable: rule can be removed from the candidate list
      }
      else if(isNullable(rule)) 
      {
        // new nullable symbol found!
        found=true;
        m_nullables.insert(leftSymbolName);
      }
      else
      {
        // I must keep this one
        if(it1!=it2) *it2 = *it1;
        it2++;
      }
    }
    m_candidates.erase(it2, m_candidates.end());
  }
}

//=============================================================================

class FirstSetBuilder {
  const Grammar *m_grammar;
  map<string, set<const TerminalSymbol*> > &m_firsts;
public:
  FirstSetBuilder(
    const Grammar *grammar, 
    map<string, set<const TerminalSymbol*> > &firsts)
    : m_grammar(grammar)
    , m_firsts(firsts)
  {
    initialize();
    build();
  }

private:
  void initialize();
  void build();
};

void FirstSetBuilder::initialize()
{
  for(int i=0;i<m_grammar->getRuleCount();i++)
  {
    /*
    const Rule *rule = m_grammar->getRule(i);
    int j = 0;
    while(j<rule->getLength() 
      && rule->getRightSymbol(i)->isNonTerminal() 
      && rule
    if(rule->getLength()>0 && rule->get) 
      m_nullables.insert(rule->getLeftSymbol()->getName());
    else if(!hasTerminals(rule))
      m_candidates.push_back(rule);
      */
  }

}

void FirstSetBuilder::build()
{
}




//=============================================================================


Grammar::Grammar()
  : m_dirty(true)
{
  m_rootSymbol = m_symbols.nt("_root");
}

Grammar::~Grammar()
{
  clear();
}

void Grammar::clear()
{
  m_dirty = true;
  for(vector<Rule*>::iterator it=m_rules.begin(); it != m_rules.end(); ++it) delete *it;
  m_rules.clear();
  m_rootSymbol = 0;
}

void Grammar::updateRootRule(const NonTerminalSymbol *startSymbol)
{
  vector<const Symbol*> right; right.push_back(startSymbol); right.push_back(m_symbols.eof());
  Rule *rule = new Rule(m_rootSymbol, right, RuleAction("pass",1));
  rule->setId(0);
  if(m_rules.empty()) m_rules.push_back(rule);
  else 
  {
    delete m_rules[0];
    m_rules[0] = rule;
  }
}

const Rule *Grammar::addRule(Rule *rule)
{
  m_dirty = true;
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
  m_dirty = true;
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

const set<string> &Grammar::getNullables() const
{
  if(m_dirty) computeAll();
  return m_nullables;
}

bool Grammar::isNullable(std::string ntName) const
{
  if(m_dirty) computeAll();
  return m_nullables.find(ntName) != m_nullables.end();
}


void Grammar::computeAll() const
{
  NullablesBuilder nullablesBuilder(this, m_nullables);
  m_dirty = false;
}




//=============================================================================


RuleBuilder::RuleBuilder(Grammar *g, const std::string &leftSymbolName)
  : m_grammar(g)
  , m_leftSymbol(g->symbols().nt(leftSymbolName))
{
}

RuleBuilder &RuleBuilder::t(const std::string &text)
{
  const Symbol *symbol = m_grammar->symbols().t(text);
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
  m_symbols.push_back(m_grammar->symbols().nt(ntName));
  return *this;
}

const Rule *RuleBuilder::end(const RuleAction &action)
{
  Rule *rule = new Rule(m_leftSymbol, m_symbols, action);
  return m_grammar->addRule(rule);
}

