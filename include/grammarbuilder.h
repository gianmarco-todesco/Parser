#ifndef GRAMMARBUILDER_H
#define GRAMMARBUILDER_H

#include "token.h"
#include "grammar.h"
#include "parser.h"

class GrammarDefinitionParser : public Parser {
  Grammar *m_theGrammar;
public:
  GrammarDefinitionParser();
  ~GrammarDefinitionParser();
};

class GrammarBuilder {
public:
  Grammar *build(BaseTokenizer *tokenizer);

};

#endif
