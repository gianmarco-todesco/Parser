#ifndef PARSER_H
#define PARSER_H

#include "grammar.h"
#include <ostream>

class ParseState {
  const Grammar *m_grammar;
  int m_id;
  std::vector<std::pair<int, int> > m_items; // m_items = [<rule-index, dot-pos>, ... ]
  std::string m_signature; // m_signature is m_items in printable form (e.g. "1,0;2,0;3,1")
  std::map<const Symbol*, const ParseState*> m_links;

  std::vector<const Symbol*> m_currentSymbols;
  std::vector<const TerminalSymbol*> m_currentTerminals;
  std::vector<const Rule*> m_completeRules;
public:
  // Build a state with all the rule 'ntName -> @ ....'
  ParseState(const Grammar *g, const std::string &ntName);
  // Build a next state 
  ParseState(const ParseState &other, const Symbol *symbol);

  const Grammar *getGrammar() const { return m_grammar; }
  std::string getSignature() const { return m_signature; }

  int getId() const { return m_id; }
  void setId(int id) { m_id = id; }

  const std::vector<std::pair<int, int> > &getItems() const { return m_items; }

  void dump() const;

  const std::vector<const Symbol*> &getCurrentSymbols() const { return m_currentSymbols; }
  const std::vector<const TerminalSymbol*> &getCurrentTerminals() const { return m_currentTerminals; }
  const std::vector<const Rule*> &getCompleteRules() const { return m_completeRules; }

  const ParseState *getNextState(const Symbol*symbol) const { 
    std::map<const Symbol*, const ParseState*>::const_iterator it = m_links.find(symbol);
    return it == m_links.end() ? 0 : it->second;
  }
  void setNextState(const Symbol*symbol, const ParseState *nextState) { 
    m_links[symbol] = nextState; 
  }

private:
  void postBuild();
  void computeSignature();
};


class ParseTable {
  const Grammar *m_grammar;
  std::vector<ParseState*> m_states; 
public:
  ParseTable(const Grammar *g);
  ~ParseTable();

  int getStateCount() const { return m_states.size(); }
  const ParseState*getState(int index) const { return m_states[index]; }
  const ParseState*getFirstState() const { return m_states[0]; }
  
  void dump() const;

private:
  void build();
};

class ParseNode;

class ParseTree {
  const BaseTokenizer *m_tokenizer;
  std::vector<std::string> m_nodeTags;
  std::map<std::string, int> m_nodeTagTable;
  std::vector<int> m_buffer;
  std::vector<int> m_stack;
public:
  ParseTree(const BaseTokenizer *tokenizer);
  ~ParseTree();

  void addLeaf(int tokenIndex);
  void removeItems(int count);
  void makeNode(const std::string &tag, int count, unsigned long mask = 0xffffffffL);
  void takeOne(int count, int index);
  
  int getStackSize() const { return (int)m_stack.size(); }

  // get the index-th last node on the stack
  ParseNode getNode(int index) const;

  ParseNode getChildNode(const ParseNode &parent, int childIndex) const;
  std::string getTag(const ParseNode &node) const;
  bool isLeaf(const ParseNode &node) const;
  int getChildCount(const ParseNode &node) const;
  Token getToken(const ParseNode &node) const;

  /*
  ParseOutputNode getChildNode(const ParseOutputNode &node, int childIndex) const;
  */

  std::string toString() const; // used mainly for debugging
  void dump(std::ostream &out) const;
};

class ParseNode {
  const ParseTree *m_parseTree;
  int m_position; // position refers to the tree
public:

  ParseNode() : m_parseTree(0), m_position(-1) {} // a null node
  ParseNode(const ParseTree *tree, int position) : m_parseTree(tree), m_position(position) {}
  ~ParseNode() {}

  int getPosition() const { return m_position; }

  bool isNull() const { return !m_parseTree; }

  Token getToken() const { return m_parseTree->getToken(*this); }
  ParseNode getChild(int childIndex) const { return m_parseTree->getChildNode(*this, childIndex); }
  std::string getTag() const { return m_parseTree->getTag(*this); }
  bool isLeaf() const { return m_parseTree->isLeaf(*this); }
  int getChildCount() const { return m_parseTree->getChildCount(*this); }
};

class Parser {
  const Grammar *m_grammar;
  ParseTable m_parseTable;
  std::vector<const ParseState*> m_stack;
  ParseTree *m_parseTree;
  const BaseTokenizer *m_tokenizer;
  bool m_skipNewLines;

public:

  Parser(const Grammar *grammar);
  virtual ~Parser() { delete m_parseTree; }

  const ParseTable &getParseTable() const { return m_parseTable; }
  const Grammar *getGrammar() const { return m_grammar;}
 
  bool parse(const BaseTokenizer *tokenizer);

  void setSkipNewLines(bool skipNewLines) { m_skipNewLines = skipNewLines; }

  // warning: parseTree is owned by the Parser (and it is destroyed by the parser dtor)
  const ParseTree *getParseTree() const { return m_parseTree; }

  virtual void skipNewLinesAndComments(const std::vector<Token> &tokens, int &pos);
  

private:

  void doSemanticAction(const Rule *rule);

};


#endif
