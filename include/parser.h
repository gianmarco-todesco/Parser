#ifndef PARSER_H
#define PARSER_H

#include "grammar.h"

class ParseState {
  const Grammar *m_grammar;
  int m_id;
  std::vector<std::pair<int, int> > m_items;
  std::string m_signature;
  std::map<const Symbol*, const ParseState*> m_links;

  std::vector<const Symbol*> m_currentSymbols;
  std::vector<const TerminalSymbol*> m_currentTerminals;
  std::vector<const Rule*> m_completeRules;
public:
  ParseState(const Grammar *g, const std::string &ntName);
  ParseState(const ParseState &other, const Symbol *symbol);

  const Grammar *getGrammar() const { return m_grammar; }
  std::string getSignature() const { return m_signature; }

  int getId() const { return m_id; }
  void setId(int id) { m_id = id; }

  const std::vector<std::pair<int, int> > &getItems() const { return m_items; }

  void dump();

  const std::vector<const Symbol*> &getCurrentSymbols() const { return m_currentSymbols; }
  const std::vector<const TerminalSymbol*> &getCurrentTerminals() const { return m_currentTerminals; }
  const std::vector<const Rule*> &getCompleteRules() const { return m_completeRules; }

  const ParseState *getNextState(const Symbol*symbol) const { 
    std::map<const Symbol*, const ParseState*>::const_iterator it = m_links.find(symbol);
    return it == m_links.end() ? 0 : it->second;
  }
  void setNextState(const Symbol*symbol, const ParseState *nextState) { m_links[symbol] = nextState; }

  void postBuild();
  void computeSignature();
};


class ParseTable {
  const Grammar *m_grammar;
  std::vector<ParseState*> m_states; 
public:
  ParseTable(const Grammar *g, const std::string &startNtName);
  void dump();

  void build(const std::string &startNtName);
  const ParseState*getFirstState() const { return m_states[0]; }
};

class ParseOutputNode {
  Token m_token;
  int m_childCount, m_position;
public:
  ParseOutputNode(int position, const Token &token) : m_token(token), m_childCount(0), m_position(position) {}
  ParseOutputNode(int position, const std::string &groupName, int childCount) : m_token(Token::T_None, groupName), m_childCount(childCount), m_position(position) {}

  bool isToken() const { return m_token.getType() != Token::T_None; }
  bool isGroup() const { return m_token.getType() == Token::T_None; }
  const Token &getToken() const { return m_token; }
  std::string getGroupName() const { return m_token.getText(); }
  int getChildCount() const { return m_childCount; }
  int getPosition() const { return m_position; }
};

class ParseOutput {
  BaseTokenizer *m_tokenizer;
  std::vector<std::string> m_nodeNames;
  std::map<std::string, int> m_nodeNameTable;
  std::vector<int> m_buffer;
  std::vector<int> m_stack;
public:
  ParseOutput(BaseTokenizer *tokenizer);
  ~ParseOutput();

  void addToken(int tokenIndex);
  void removeItems(int count);
  void makeGroup(const std::string &groupName, int count, unsigned long mask = 0xffffffffL);
  void takeOne(int index, int count);
  
  int getCount() const { return (int)m_stack.size(); }
  ParseOutputNode getNode(int index) const;
  ParseOutputNode getNodeByPosition(int position) const;
  ParseOutputNode getChildNode(const ParseOutputNode &node, int childIndex) const;

};


class Parser {
  Grammar *m_grammar;
  ParseTable m_parseTable;

  std::vector<const ParseState*> m_stack;
  ParseOutput *m_output;

public:
  Parser(Grammar *grammar, const std::string &startNtName);
  ~Parser() { delete m_output; }

  bool parse(BaseTokenizer *tokenizer);

  const ParseOutput *getOutput() const { return m_output; }

private:
  void doSemanticAction(const Rule *rule);
  void skipNewLinesAndComments(const std::vector<Token> &tokens, int &pos);
};

#endif
