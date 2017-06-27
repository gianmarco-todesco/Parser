#include "parser.h"
#include <set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <assert.h>


using namespace std;


class ParseStateBuilder {
  const Grammar *m_grammar;
  set<pair<int, int> > m_items;
public:
  ParseStateBuilder(const Grammar *g) : m_grammar(g) {}
  void build(const string &ntName);
  void build(const ParseState &other, const Symbol *symbol);

  void makeClosure(const string &ntName = "");
  
  void getItems(vector<pair<int, int> > &items);
};


void ParseStateBuilder::build(const string &ntName)
{
  vector<const Rule*> rules;
  m_grammar->getRulesByLeftSymbol(rules, ntName);
  for(vector<const Rule*>::iterator it = rules.begin(); it != rules.end(); ++it)
  {
    m_items.insert(make_pair((*it)->getId(), 0));
  }
  makeClosure(ntName);
}

void ParseStateBuilder::build(const ParseState &other, const Symbol *symbol)
{
  const vector<pair<int, int> > &otherItems = other.getItems();

  for(vector<pair<int, int> >::const_iterator it = otherItems.begin(); it != otherItems.end(); ++it)
  {
    const Rule *rule = m_grammar->getRule(it->first);
    const Symbol *dotSymbol = rule->getRightSymbol(it->second);
    if(!dotSymbol) continue;
    if(dotSymbol->isTerminal() != symbol->isTerminal()) continue;
    if(dotSymbol->getName() == symbol->getName())
    {
      m_items.insert(make_pair(it->first, it->second+1));
    }
  }
  makeClosure();
}



void ParseStateBuilder::makeClosure(const string &ntName)
{
  set<string> done;
  if(ntName != "") done.insert(ntName);
  vector<string> todo;
  for(set<pair<int, int> >::const_iterator it = m_items.begin(); it != m_items.end(); ++it)
  {
    const Symbol *symbol = m_grammar->getRule(it->first)->getRightSymbol(it->second);
    if(!!symbol && symbol->isNonTerminal()) { 
      string name = symbol->getName();
      if(done.find(name) == done.end()) { done.insert(name); todo.push_back(name); }
    }
  }
  while(!todo.empty())
  {
    string name = todo.back();
    todo.pop_back();
    vector<const Rule*> rules;
    m_grammar->getRulesByLeftSymbol(rules, name);
    for(vector<const Rule*>::iterator it = rules.begin(); it != rules.end(); ++it)
    {
      m_items.insert(make_pair((*it)->getId(), 0));
      const Symbol *symbol = (*it)->getRightSymbol(0); 
      if(!!symbol && symbol->isNonTerminal()) { 
        name = symbol->getName();
        if(done.find(name) == done.end()) { done.insert(name); todo.push_back(name); }
      }
    }
  }
}


void ParseStateBuilder::getItems(vector<pair<int, int> > &items)
{
  items.assign(m_items.begin(), m_items.end());
  sort(items.begin(), items.end());
}



//=============================================================================


ParseState::ParseState(const Grammar *g, const string &ntName)
  : m_grammar(g)
  , m_id(-1)
{
  ParseStateBuilder builder(g);
  builder.build(ntName);
  builder.getItems(m_items);
  postBuild();
}

ParseState::ParseState(const ParseState &other, const Symbol *symbol)
  : m_grammar(other.getGrammar())
{
  ParseStateBuilder builder(m_grammar);
  builder.build(other, symbol);
  builder.getItems(m_items);
  postBuild();
}


void ParseState::computeSignature()
{
  ostringstream ss;
  for(vector<pair<int,int> >::const_iterator it=m_items.begin(); it!=m_items.end(); ++it)
  {
    ss << it->first << "," << it->second << ";";
  }
  m_signature = ss.str();
}


struct GreaterThanTerminal
{
    inline bool operator()(const TerminalSymbol* a, const TerminalSymbol* b)
    {
        return a->getConstantStrength() > b->getConstantStrength();
    }
};

void ParseState::postBuild()
{
  computeSignature();
  set<const Symbol*> table;
  for(vector<pair<int, int> >::const_iterator it = m_items.begin(); it != m_items.end(); ++it)
  {
    const Symbol *symbol = m_grammar->getRule(it->first)->getRightSymbol(it->second);
    if(symbol == 0)
    {
      m_completeRules.push_back(m_grammar->getRule(it->first));
    }
    else if(table.find(symbol) == table.end()) 
    { 
      table.insert(symbol); 
      m_currentSymbols.push_back(symbol); 
      if(symbol->isTerminal()) 
        m_currentTerminals.push_back(dynamic_cast<const TerminalSymbol*>(symbol));
    }
  }
  sort(m_currentTerminals.begin(), m_currentTerminals.end(), GreaterThanTerminal());
}


void ParseState::dump()
{
  for(vector<pair<int, int> >::const_iterator it = m_items.begin(); it != m_items.end(); ++it)
  {
    const Rule *rule = m_grammar->getRule(it->first);
    int dot = it->second;
    cout << "  "  << rule->getLeftSymbol()->getName().c_str() << " ->";
    for(int i=0;i<rule->getLength();i++)
    {
      if(i==dot) cout << " @";
      cout << " " << rule->getRightSymbol(i)->getName().c_str();
    }
    if(rule->getLength()==dot) cout << " @";
    cout << endl;
  }
}


//=============================================================================

ParseTable::ParseTable(const Grammar *g, const string &startNtName)
  : m_grammar(g)
{
  build(startNtName);
}

ParseTable::~ParseTable()
{
  for(int i=0;i<(int)m_states.size();i++) delete m_states[i];
  m_states.clear();
}


void ParseTable::dump() const
{
  for(int i=0;i<(int)m_states.size();i++)
  {
    cout << "S" << i << endl;
    m_states[i]->dump();
  }
}


void ParseTable::build(const string &startNtName)
{
  ParseState *state = new ParseState(m_grammar, startNtName);
  state->setId(0);
  m_states.push_back(state);
  map<string, ParseState *> table;
  table[state->getSignature()] = state;
  vector<ParseState *> todo;
  todo.push_back(state);
  while(!todo.empty())
  {
    state = todo.back(); todo.pop_back();
    const vector<const Symbol*> symbols = state->getCurrentSymbols();    
    for(vector<const Symbol*>::const_iterator it = symbols.begin(); it != symbols.end(); ++it)
    {
      ParseState *nextState = new ParseState(*state, *it);
      map<string, ParseState *>::iterator nextStateIt = table.find(nextState->getSignature());
      if(nextStateIt != table.end()) 
      {
        delete nextState;
        state->setNextState(*it, nextStateIt->second);
      }
      else
      {
        nextState->setId((int)m_states.size());
        m_states.push_back(nextState);
        table[nextState->getSignature()] = nextState;
        todo.push_back(nextState);
        state->setNextState(*it, nextState);
      }
    }
  }
}



//=============================================================================

ParseTree::ParseTree(const BaseTokenizer *tokenizer)
  : m_tokenizer(tokenizer)
{
}

ParseTree::~ParseTree()
{
}

void ParseTree::addLeaf(int tokenIndex)
{
  m_stack.push_back((int)m_buffer.size());
  m_buffer.push_back(3); // size
  m_buffer.push_back(-1); // is a token
  m_buffer.push_back(tokenIndex);
}

// remove count elements from the stack; remove them also from the buffer
void ParseTree::removeItems(int count)
{
  assert((int)m_stack.size()>=count);
  int k = m_stack[m_stack.size()-count];
  m_stack.erase(m_stack.end()-count,m_stack.end());
  m_buffer.erase(m_buffer.begin()+k, m_buffer.end());
}

// replace count elements from the stack with the index-th last element; update the buffer
void ParseTree::takeOne(int count, int index)
{
  assert(0<=index && index<count);
  int h = (int)m_stack.size() - count; 
  assert(h>=0);
  int k = m_stack[h];
  int k1 = m_stack[h+index];
  if(k1>k) m_buffer.erase(m_buffer.begin()+k, m_buffer.begin()+k1);
  k1 = k + m_buffer[k];
  if(k1<(int)m_buffer.size()) m_buffer.erase(m_buffer.begin()+k1, m_buffer.end());
  m_stack.erase(m_stack.begin()+h, m_stack.end());
  m_stack.push_back(k);
}

// remove count elements from the stack, push a node with the given tag to the stack; 
// the node contains a number of old elements according to mask
// update the buffer
// update the tag table (tags are stored with their ids)
void ParseTree::makeNode(const std::string &tag, int count, unsigned long mask)
{
  // add the group to the 
  pair<map<string, int>::iterator, bool> res = m_nodeTagTable.insert(make_pair(tag,  m_nodeTags.size()));
  if(res.second) m_nodeTags.push_back(tag);
  int tagIndex = res.first->second;
    
  vector<int> tmpBuffer;
  tmpBuffer.push_back(0);
  tmpBuffer.push_back(tagIndex);
  tmpBuffer.push_back(0); // child count

  int h = (int)m_stack.size() - count; 
  assert(h>=0);
  for(int i=0; i<count; i++)
  {
    if(((mask>>i)&1)==0) continue;
    int a = m_stack[h+i];
    int b = a + m_buffer[a];
    if(m_buffer[a+1]==tagIndex) // left recursion
    {
      tmpBuffer[2] += m_buffer[a+2];
      a+=3;
    }
    else
    {
      tmpBuffer[2]++;
    }
    tmpBuffer.insert(tmpBuffer.end(), m_buffer.begin()+a, m_buffer.begin()+b);
  }

  tmpBuffer[0] = (int)tmpBuffer.size();
  int k;
  if(count>0)
  {
    k = m_stack[m_stack.size()-count];
    m_buffer.erase(m_buffer.begin()+k, m_buffer.end());
  }
  else
  {
    k = m_buffer.size();
  }
  m_buffer.insert(m_buffer.end(), tmpBuffer.begin(), tmpBuffer.end());

  m_stack.erase(m_stack.begin()+h, m_stack.end());
  m_stack.push_back(k);
}

ParseNode ParseTree::getNode(int index) const
{
  assert(0<=index && index<(int)m_stack.size());
  int k = m_stack[index];
  return ParseNode(this,k);
}

ParseNode ParseTree::getChildNode(const ParseNode &parent, int childIndex) const
{
  int k = parent.getPosition();
  if(0<=childIndex && childIndex<m_buffer[k+2])
  {
    k += 3;
    for(int i=0;i<childIndex;i++)
    {
      k += m_buffer[k];
    }
    return ParseNode(this, k);
  }
  else return ParseNode();
}

std::string ParseTree::getTag(const ParseNode &node) const
{
  int k = node.getPosition();
  if(m_buffer[k+1]>=0) return m_nodeTags[m_buffer[k+1]];
  else return "";
}

bool ParseTree::isLeaf(const ParseNode &node) const
{
  int k = node.getPosition();
  return m_buffer[k+1]<0;
}

Token ParseTree::getToken(const ParseNode &node) const
{
  int k = node.getPosition();
  if(m_buffer[k+1]<0)
    return m_tokenizer->getTokens()[m_buffer[k+2]];
  else
    return Token();
}

int ParseTree::getChildCount(const ParseNode &node) const
{
  int k = node.getPosition();
  if(m_buffer[k+1]<0) return 0;
  else return m_buffer[k+2];
}


namespace {

  void serialize(
    ostream &ss, 
    const ParseNode &node)
  {
    if(node.isLeaf())
    {
      Token token = node.getToken();
      ss << "'" << token.getText() << "'";
    }
    else
    {
      ss << node.getTag().c_str() << "(";
      for(int i=0;i<node.getChildCount();i++)
      {
        if(i>0) ss<<",";
        serialize(ss, node.getChild(i));      
      }
      ss << ")";
    }
  }

  void dump(
    ostream &out,
    int level,
    const ParseNode &node)
  {
    for(int i=0;i<level;i++) out << "    ";
    if(node.isLeaf())
    {
      out << "'"<<node.getToken().getText()<<"'\n";
    }
    else
    {
      out << node.getTag().c_str() << "\n";
      for(int i=0;i<node.getChildCount();i++)
      {
        dump(out, level+1, node.getChild(i));  
      }
    }
  }

}// namespace 

std::string ParseTree::toString() const
{
  ostringstream ss;
  for(int i=0;i<getStackSize();i++)
  {
    if(i>0) ss<<",";
    serialize(ss, getNode(i));
  }
  return ss.str();
}


void ParseTree::dump(ostream &out) const 
{
  for(int i=0;i<(int)m_stack.size();i++)
  {
    ::dump(out, 0, getNode(i));
  }
}


//=============================================================================


Parser::Parser(const Grammar *grammar, const string &startNtName)
  : m_grammar(grammar)
  , m_parseTable(grammar, startNtName)
  , m_parseTree(0)
  , m_tokenizer(0)
{
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
    const ParseState*state = m_stack.back();
    const vector<const Rule*> &completedRules = state->getCompleteRules();
    const vector<const TerminalSymbol*> &terminals = state->getCurrentTerminals();
    vector<const TerminalSymbol*> matchedTerminals;

    skipNewLinesAndComments(tokens, pos);

    // cout << tokens[pos] << endl;

    for(vector<const TerminalSymbol*>::const_iterator it = terminals.begin(); it != terminals.end(); ++it)
    {
      if((*it)->matches(tokens[pos])) matchedTerminals.push_back(*it);
    }
    if(matchedTerminals.size()>0)
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
      const ParseState*nextState = state->getNextState(matchedTerminals[0]);
      assert(nextState);
      m_stack.push_back(nextState);
      m_parseTree->addLeaf(pos);
      //cout << "Shift " << tokens[pos] << endl;
      if(!tokens[pos].isEof()) pos++;
    }
    else if(completedRules.size()>0)
    {
      if(completedRules.size()>1) cout << "Reduce-reduce conflict" << endl;
      const Rule *rule = completedRules[0];
      //cout << "Reduce " << *rule << endl;
      doSemanticAction(rule);

      for(int i=0;i<rule->getLength();i++) m_stack.pop_back();
      if(!m_stack.empty())
      {
        state = m_stack.back();
        const ParseState*nextState = state->getNextState(rule->getLeftSymbol());
        assert(nextState || state->getId()==0);
        if(nextState) m_stack.push_back(nextState);
        else { ret = true; break; }
      }
    }
    else
    {
      cout << "Syntax error" << endl;
      cout << "expected one of:";
      for(int i=0;i<(int)terminals.size();i++) cout << terminals[i]->getName() << " ";
      cout << endl;
      cout << "Found " << tokenizer->getTokens()[pos] << endl;
      tokenizer->dumpPosition(cout, pos);
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
  else
  {
    const RuleAction &action = rule->getAction();
    m_parseTree->makeNode(action.getGroupName(), rule->getLength(),action.getMask());
  }
}

/*
void Parser::skipNewLinesAndComments(const std::vector<Token> &tokens, int &pos)
{
  const Token slash(Token::T_Special, "/");
  const Token star(Token::T_Special, "*");
  int n = (int)tokens.size();

  while(pos < n)
  {
    if(tokens[pos].isEol()) pos++;
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

*/
