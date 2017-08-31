#include "ParserTable.h"
#include <sstream>
#include <assert.h>
#include <iostream>
#include <algorithm>

using namespace std;

ParserState::ParserState(const Grammar *g)
  : m_grammar(g)
{
  assert(g);
  assert(g->getRuleCount()>1);
  set<const TerminalSymbol*> la;
  la.insert(g->symbols().getEofTerminalSymbol());
  m_items[make_pair(0,0)] = la;
  makeClosure();
  computeSignature();
}

ParserState::ParserState(const ParserState &other, const Symbol *symbol)
  : m_grammar(other.m_grammar)
{
  map<RuleIndexAndDot, LookAhaedSet >::const_iterator itemIt;
  for(itemIt = other.m_items.begin(); itemIt != other.m_items.end(); ++itemIt)
  {
    int ruleIndex = itemIt->first.first, dotPos = itemIt->first.second;
    const Rule *rule = m_grammar->getRule(ruleIndex);
    if(rule->getRightSymbol(dotPos) == symbol)
      m_items[make_pair(ruleIndex, dotPos+1)] = itemIt->second;
  }
  makeClosure();
  computeSignature();
}


void ParserState::computeSignature()
{
  ostringstream ss;
  map<pair<int, int>, set<const TerminalSymbol*> >::iterator it;
  for(it=m_items.begin(); it!=m_items.end(); ++it)
  {
    ss << it->first.first << "," << it->first.second << ";";
  }
  m_signature = ss.str();
}

typedef set<const TerminalSymbol*> LookAhaedSet;

void computeNewLookahaedSet(
  LookAhaedSet &newSet,
  const Grammar *g, 
  const pair<int, int> &ruleAndDot,
  const LookAhaedSet &oldSet)
{
  const Rule *rule = g->getRule(ruleAndDot.first);
  int dotPos;
  for(dotPos = ruleAndDot.second + 1; dotPos<rule->getLength(); dotPos++)
  {
    const Symbol *symbol = rule->getRightSymbol(dotPos);
    if(symbol->isTerminal())
    {
      newSet.insert(dynamic_cast<const TerminalSymbol*>(symbol));
      break;
    }
    const LookAhaedSet &ntSet = g->getFirstSet(symbol->getName());
    for(LookAhaedSet::const_iterator it = ntSet.begin(); it != ntSet.end(); ++it)
      newSet.insert(*it);
    if(!g->isNullable(symbol->getName())) break;
  }
  if(dotPos == rule->getLength())
  {
    for(LookAhaedSet::const_iterator it = oldSet.begin(); it != oldSet.end(); ++it)
      newSet.insert(*it);
  }
}



void ParserState::makeClosure()
{
  vector<RuleIndexAndDot> todo;
  map<RuleIndexAndDot, LookAhaedSet >::iterator itemIt;
  for(itemIt = m_items.begin(); itemIt != m_items.end(); ++itemIt)
  {
    int ruleIndex = itemIt->first.first, dotPos = itemIt->first.second;
    const Rule *rule = m_grammar->getRule(ruleIndex);
    if(dotPos>=rule->getLength() || !rule->getRightSymbol(dotPos)->isNonTerminal()) continue;
    todo.push_back(itemIt->first);
  }
  while(!todo.empty())
  {
    RuleIndexAndDot rid = todo.back(); todo.pop_back();
    LookAhaedSet newSet;
    computeNewLookahaedSet(newSet, m_grammar, rid, m_items[rid]);

    string ntName = m_grammar->getRule(rid.first)->getRightSymbol(rid.second)->getName();
    vector<const Rule*> rules;
    m_grammar->getRulesByLeftSymbol(rules, ntName);
    for(int i=0;i<(int)rules.size();i++)
    {
      RuleIndexAndDot newRid(rules[i]->getId(), 0);
      bool isCandidate = rules[i]->getLength()>0 && rules[i]->getRightSymbol(0)->isNonTerminal();
      itemIt = m_items.find(newRid);
      if(itemIt == m_items.end())
      {
        m_items[newRid] = newSet;
        if(isCandidate)
          todo.push_back(newRid);
      }
      else
      {
        bool changed = false;
        LookAhaedSet &oldSet = itemIt->second;
        for(set<const TerminalSymbol*>::iterator j = newSet.begin(); j != newSet.end(); ++j)
        {
          if(oldSet.find(*j)==oldSet.end()) { oldSet.insert(*j); changed = true; }
        }
        if(isCandidate && changed) 
          todo.push_back(newRid);
      }
    }
  }
}


struct GreaterThanTerminal
{
    inline bool operator()(const TerminalSymbol* a, const TerminalSymbol* b)
    {
        return a->getConstantStrength() > b->getConstantStrength();
    }
};


void ParserState::postBuild()
{
  map<RuleIndexAndDot, LookAhaedSet >::const_iterator itemIt;
  for(itemIt = m_items.begin(); itemIt != m_items.end(); ++itemIt)
  {
    int ruleIndex = itemIt->first.first, dotPos = itemIt->first.second;
    const Rule *rule = m_grammar->getRule(ruleIndex);
    if(dotPos >= rule->getLength())
    {
      m_completeRules.push_back(make_pair(rule, itemIt->second));
    }
    else if(rule->getRightSymbol(dotPos)->isTerminal()) 
    {
      const TerminalSymbol *t = dynamic_cast<const TerminalSymbol *>(rule->getRightSymbol(dotPos));
      m_currentTerminals.push_back(t);
    }
  }
  sort(m_currentTerminals.begin(), m_currentTerminals.end(), GreaterThanTerminal());

  map<const TerminalSymbol*, int> tb;
  for(int i=0;i<(int)m_currentTerminals.size();i++)
  {
    
  }
}

void ParserState::dump() const
{
  cout << "S" << getId() << endl;
  map<RuleIndexAndDot, LookAhaedSet >::const_iterator itemIt;
  for(itemIt = m_items.begin(); itemIt != m_items.end(); ++itemIt)
  {
    cout 
      << "  " 
      << make_pair(*m_grammar->getRule(itemIt->first.first), itemIt->first.second) 
      << ", {";
    for(LookAhaedSet::const_iterator it = itemIt->second.begin(); it != itemIt->second.end(); ++it)
    {
      if(it != itemIt->second.begin()) cout << ", ";
      cout << " " << **it ;
    }
    cout << " }" << endl;
  }
  cout << endl;
}

int ParserState::getTerminals(vector<const TerminalSymbol*> &terminals, const Token &token) const
{
  for(vector<const TerminalSymbol*>::const_iterator it = m_currentTerminals.begin();
    it != m_currentTerminals.end(); ++it)
  {
    if((*it)->matches(token)) terminals.push_back(*it); 
  }
  return (int)terminals.size();
}

int ParserState::getCompleteRules(vector<const Rule*> &rules, const Token &token) const
{
  for(vector<pair<const Rule*, LookAhaedSet> >::const_iterator it = m_completeRules.begin();
      it != m_completeRules.end(); ++it)
  {
    for(LookAhaedSet::const_iterator j = it->second.begin(); j != it->second.end(); ++j)
    {
      if((*j)->matches(token)) {rules.push_back(it->first); break; }
    }
  }
  return (int)rules.size();
}



//=============================================================================

ParserTable::ParserTable(const Grammar *g)
  : m_grammar(g)
{
  build();
}

ParserTable::~ParserTable()
{
  for(vector<ParserState*>::iterator it = m_states.begin(); it != m_states.end(); ++it)
    delete *it;
  m_states.clear();
}


bool ParserTable::merge(ParserState *oldState, ParserState *newState)
{
  bool changed = false;
  map<RuleIndexAndDot, LookAhaedSet >::const_iterator itemIt;
  for(itemIt = newState->m_items.begin(); itemIt != newState->m_items.end(); ++itemIt)
  {
    const LookAhaedSet &newLa = itemIt->second;
    LookAhaedSet &oldLa = oldState->m_items[itemIt->first];
    for(LookAhaedSet::const_iterator it = newLa.begin(); it != newLa.end(); ++it)
    {
      if(oldLa.find(*it)==oldLa.end()) { changed=true; oldLa.insert(*it); }
    }
  }
  return changed;
}

void ParserTable::build()
{
  ParserState *state = new ParserState(m_grammar);
  state->setId(0);
  m_states.push_back(state);
  vector<int> todo;
  todo.push_back(0);
  map<string, int> table;
  table[state->getSignature()] = state->getId();
  while(!todo.empty())
  {
    ParserState *oldState = m_states[todo.back()]; todo.pop_back();
    set<const Symbol*> symbols;
    map<RuleIndexAndDot, LookAhaedSet >::const_iterator itemIt;
    for(itemIt = oldState->m_items.begin(); itemIt != oldState->m_items.end(); ++itemIt)
    {
      const Rule*rule = m_grammar->getRule(itemIt->first.first);
      if(rule->getLength() <= itemIt->first.second) continue;
      const Symbol *symbol = rule->getRightSymbol(itemIt->first.second);
      symbols.insert(symbol);
    }
    for(set<const Symbol*>::iterator it = symbols.begin(); it != symbols.end(); ++it)
    {
      state = new ParserState(*oldState, *it);
      map<string, int>::iterator tableIt = table.find(state->getSignature());
      if(tableIt == table.end())
      {
        state->setId(m_states.size());
        m_states.push_back(state);
        table[state->getSignature()] = state->getId();
        todo.push_back(state->getId());
        oldState->setNextState(*it, state);
      }
      else
      {
        bool changed = merge(m_states[tableIt->second], state);
        delete state;
        if(changed) todo.push_back(tableIt->second);
        oldState->setNextState(*it, m_states[tableIt->second]);
      }
    }
  }
  for(int i=0;i<(int)m_states.size();i++) m_states[i]->postBuild();
}


