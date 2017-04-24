#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <vector>
#include <string>

#include "token.h"

class Grammar;


class Symbol {
  Grammar *m_grammar;
  std::string m_name;
public:
  Symbol(Grammar *grammar, const std::string &name) : m_grammar(grammar), m_name(name) {}
  virtual ~Symbol() {}
  Grammar *getGrammar() const { return m_grammar; }
  const std::string &getName() const { return m_name; }
  virtual bool isTerminal() const = 0;
  bool isNonTerminal() const { return !isTerminal(); }
  virtual bool matches(const Token &token) const = 0;
  virtual bool isConstant() const = 0;
protected:
  void setName(const std::string &name) { m_name = name; }
};

std::ostream &operator<<(std::ostream &out, const Symbol &nt);

class NonTerminalSymbol : public Symbol {
public:
  NonTerminalSymbol(Grammar *grammar, const std::string &name) : Symbol(grammar, name) {}
  bool isTerminal() const { return false; }
  bool isConstant() const { return false; }
  bool matches(const Token &) const { return false; }
};

class TerminalSymbol : public Symbol {
public:
  TerminalSymbol(Grammar *grammar, const std::string &name) : Symbol(grammar, name) {}
  virtual ~TerminalSymbol() {}
  bool isTerminal() const { return true; }
  virtual int getConstantStrength() const = 0;
};


class TextTerminalSymbol : public TerminalSymbol {
  std::string m_text;
public:
  TextTerminalSymbol(Grammar *grammar, const std::string &text) : TerminalSymbol(grammar, "'"+text+"'"), m_text(text) {}
  TextTerminalSymbol(Grammar *grammar, const std::string &text, const std::string &name) : TerminalSymbol(grammar, name), m_text(text) {}
  bool isConstant() const { return true; }
  bool matches(const Token &token) const { return token.getText() == m_text; }
  int getConstantStrength() const { return 10; }
};

class TokenTypeTerminalSymbol : public TerminalSymbol {
  Token::Type m_type;
public:
  TokenTypeTerminalSymbol(Grammar *grammar, const std::string &name, Token::Type type) : TerminalSymbol(grammar, name), m_type(type) {}
  bool isConstant() const { return m_type == Token::T_Eol || m_type == Token::T_Eof; }
  bool matches(const Token &token) const { return token.getType() == m_type; }
  int getConstantStrength() const { return 5; }
};

class NumberTerminalSymbol : public TokenTypeTerminalSymbol {
public:
  NumberTerminalSymbol(Grammar *grammar) : TokenTypeTerminalSymbol(grammar, "number", Token::T_Number) {}
};


class KeywordsTerminalSymbol : public TerminalSymbol {
  std::vector<std::string> m_keywords;
public:
  KeywordsTerminalSymbol(Grammar *grammar, const std::vector<std::string> &keywords);
  KeywordsTerminalSymbol(Grammar *grammar, const std::string &kw1, const std::string &kw2);
  KeywordsTerminalSymbol(Grammar *grammar, const std::string &kw1, const std::string &kw2, const std::string &kw3);
  KeywordsTerminalSymbol(Grammar *grammar, const std::string &kw1, const std::string &kw2, const std::string &kw3, const std::string &kw4);
  bool isConstant() const { return m_keywords.size() == 1; }
  bool matches(const Token &token) const;
  int getConstantStrength() const { return 8; }

private:
  void buildName();
};

class AnyTerminalSymbol : public TerminalSymbol {
public:
  AnyTerminalSymbol(Grammar *grammar) : TerminalSymbol(grammar, "any") {}
  virtual ~AnyTerminalSymbol() {}
  bool isConstant() const { return false; }
  bool matches(const Token &) const { return true; }
  int getConstantStrength() const { return 1; }
};



class Rule {
  NonTerminalSymbol *m_left;    // not owner
  std::vector<Symbol*> m_right; // not owner
  int m_id;
  std::string m_action;
public:
  Rule(NonTerminalSymbol *left, const std::vector<Symbol*> &right) : m_left(left), m_right(right), m_id(-1), m_action("") {}
  ~Rule() { }

  int getId() const { return m_id; }
  void setId(int id) { m_id = id; }

  Grammar *getGrammar() const { return m_left->getGrammar(); }

  const NonTerminalSymbol *getLeftSymbol() const { return m_left; }

  int getLength() const { return m_right.size(); }
  const Symbol *getRightSymbol(int index) const { return index<(int)m_right.size() ? m_right[index] : 0; }

  const std::string getAction() const { return m_action; }
  void setAction(const std::string &action) { m_action = action; }
};

std::ostream &operator<<(std::ostream &out, const Rule &rule);


class Grammar {
  std::map<std::string, NonTerminalSymbol*> m_ntSymbols;
  std::vector<TerminalSymbol*> m_tSymbols;
  std::vector<Rule*> m_rules;

public:
  Grammar();
  ~Grammar();

  void clear();

  NonTerminalSymbol*getNonTerminal(const std::string &name, bool createIfNeeded = true);
  TerminalSymbol* addTerminal(TerminalSymbol*t);

  const Rule *createRule(const std::string &left, const std::string &right, const std::string &action);

  int getRuleCount() const { return (int)m_rules.size(); }
  const Rule *getRule(int index) const { return m_rules[index]; }

  void getRulesByLeftSymbol(std::vector<Rule*> &rules, const std::string &leftSymbolName) const;

private:
  // Symbol *getSymbol(ITokenizer *tokenizer);
};



#endif

