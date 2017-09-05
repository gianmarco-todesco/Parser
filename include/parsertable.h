#ifndef PARSER_TABLE_H
#define PARSER_TABLE_H

#include "Grammar.h"
#include <set>
#include <map>

typedef std::pair<int, int> RuleIndexAndDot;
typedef std::set<const TerminalSymbol*> LookAhaedSet;


class ParserState {
  friend class ParserTable;
  const Grammar *m_grammar;
  int m_id;
  std::map<RuleIndexAndDot, LookAhaedSet> m_items; // rule-index,dot-pos => lookahaeds
  std::string m_signature; // m_signature is m_items in printable form (e.g. "1,0;2,0;3,1")
  std::map<const Symbol*, const ParserState*> m_links;

  std::vector<const Symbol*> m_currentSymbols;
  std::vector<const TerminalSymbol*> m_currentTerminals;
  std::vector<std::pair<const Rule*, LookAhaedSet> > m_completeRules;

  bool m_matchesEol;

public:

  // Build the first state
  ParserState(const Grammar *g);

  // Build a next state 
  ParserState(const ParserState &other, const Symbol *symbol);

  const Grammar *getGrammar() const { return m_grammar; }
  std::string getSignature() const { return m_signature; }

  int getId() const { return m_id; }
  void setId(int id) { m_id = id; }

  // const std::vector<LalrItem> &getItems() const { return m_items; }

  void dump(std::ostream &out) const;

  //const std::vector<const Symbol*> &getCurrentSymbols() const { return m_currentSymbols; }
  const std::vector<const TerminalSymbol*> &getCurrentTerminals() const { return m_currentTerminals; }
  const std::vector<std::pair<const Rule*, LookAhaedSet> > &getCompleteRules() const { return m_completeRules; }

  int getTerminals(std::vector<const TerminalSymbol*> &terminals, const Token &token) const;
  int getCompleteRules(std::vector<const Rule*> &rules, const Token &token) const;
  int getExpected(std::vector<const TerminalSymbol*> &expected) const;

  const ParserState *getNextState(const Symbol*symbol) const { 
    std::map<const Symbol*, const ParserState*>::const_iterator it = m_links.find(symbol);
    return it == m_links.end() ? 0 : it->second;
  }
  void setNextState(const Symbol*symbol, const ParserState *nextState) { 
    m_links[symbol] = nextState; 
  }

  bool matchesEol() const { return m_matchesEol; }

  bool isAmbiguous() const;

private:
  void makeClosure();
  void computeSignature();
  void postBuild();
  void computeMatchesEol();
};


class ParserTable {
  const Grammar *m_grammar;
  std::vector<ParserState*> m_states; 
public:
  ParserTable(const Grammar *g);
  ~ParserTable();

  int getStateCount() const { return m_states.size(); }
  const ParserState*getState(int index) const { return m_states[index]; }
  const ParserState*getFirstState() const { return m_states[0]; }
  
  void dump(std::ostream &out) const;
  bool isAmbiguous() const;

private:
  void build();
  bool merge(ParserState *oldState, ParserState *newState);
};


#endif
