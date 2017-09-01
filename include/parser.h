#ifndef PARSER_H
#define PARSER_H

#include "grammar.h"
#include "parsertable.h"
#include "parsetree.h"
#include <ostream>

class Parser {
  const Grammar *m_grammar;
  ParserTable m_parseTable;
  std::vector<const ParserState*> m_stack;
  ParseTree *m_parseTree;
  const BaseTokenizer *m_tokenizer;
  bool m_skipNewLines;
  bool m_trace;

public:

  Parser(const Grammar *grammar);
  virtual ~Parser() { delete m_parseTree; }

  const ParserTable &getParseTable() const { return m_parseTable; }
  const Grammar *getGrammar() const { return m_grammar;}
 
  bool parse(const BaseTokenizer *tokenizer);

  void setSkipNewLines(bool skipNewLines) { m_skipNewLines = skipNewLines; }

  // warning: parseTree is owned by the Parser (and it is destroyed by the parser dtor)
  const ParseTree *getParseTree() const { return m_parseTree; }

  virtual void skipNewLinesAndComments(const std::vector<Token> &tokens, int &pos, int flags=0);
  
  void traceOn() { m_trace = true; }

private:

  void doSemanticAction(const Rule *rule);
};


#endif
