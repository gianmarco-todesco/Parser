#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <vector>
#include <string>

#include "token.h"

class Grammar;

class Symbol {
  std::string m_name;
  static int m_totalCount;
public:
  Symbol(const std::string &name) : m_name(name) { m_totalCount++; }
  virtual ~Symbol() { m_totalCount--; }
  const std::string &getName() const { return m_name; }
  virtual bool isTerminal() const = 0;
  bool isNonTerminal() const { return !isTerminal(); }
  virtual bool matches(const Token &token) const = 0;
  virtual bool isConstant() const = 0;

  static int getTotalCount() { return m_totalCount; }

protected:
  void setName(const std::string &name) { m_name = name; }
};

std::ostream &operator<<(std::ostream &out, const Symbol &nt);

class NonTerminalSymbol : public Symbol {
public:
  NonTerminalSymbol(const std::string &name) : Symbol(name) {}
  bool isTerminal() const { return false; }
  bool isConstant() const { return false; }
  bool matches(const Token &) const { return false; }
};

class TerminalSymbol : public Symbol {
public:
  TerminalSymbol(const std::string &name) : Symbol(name) {}
  virtual ~TerminalSymbol() {}
  bool isTerminal() const { return true; }
  virtual int getConstantStrength() const = 0;
};


class TextTerminalSymbol : public TerminalSymbol {
  std::string m_text;
public:
  TextTerminalSymbol(const std::string &text) : TerminalSymbol("'"+text+"'"), m_text(text) {}
  TextTerminalSymbol(const std::string &text, const std::string &name) : TerminalSymbol(name), m_text(text) {}
  bool isConstant() const { return true; }
  bool matches(const Token &token) const { return token.getText() == m_text; }
  int getConstantStrength() const { return 10; }
};

class TokenTypeTerminalSymbol : public TerminalSymbol {
  Token::Type m_type;
public:
  TokenTypeTerminalSymbol(const std::string &name, Token::Type type) : TerminalSymbol(name), m_type(type) {}
  bool isConstant() const { return m_type == Token::T_Eol || m_type == Token::T_Eof; }
  bool matches(const Token &token) const { return token.getType() == m_type; }
  int getConstantStrength() const { return 5; }
};

class NumberTerminalSymbol : public TokenTypeTerminalSymbol {
public:
  NumberTerminalSymbol() : TokenTypeTerminalSymbol("number", Token::T_Number) {}
};


class KeywordsTerminalSymbol : public TerminalSymbol {
  std::vector<std::string> m_keywords;
public:
  KeywordsTerminalSymbol(const std::vector<std::string> &keywords);
  KeywordsTerminalSymbol(const std::string &kw1, const std::string &kw2);
  KeywordsTerminalSymbol(const std::string &kw1, const std::string &kw2, const std::string &kw3);
  KeywordsTerminalSymbol(const std::string &kw1, const std::string &kw2, const std::string &kw3, const std::string &kw4);
  bool isConstant() const { return m_keywords.size() == 1; }
  bool matches(const Token &token) const;
  int getConstantStrength() const { return 8; }

private:
  void buildName();
};

class AnyTerminalSymbol : public TerminalSymbol {
public:
  AnyTerminalSymbol() : TerminalSymbol("any") {}
  virtual ~AnyTerminalSymbol() {}
  bool isConstant() const { return false; }
  bool matches(const Token &) const { return true; }
  int getConstantStrength() const { return 1; }
};

class RuleAction {
  std::string m_groupName;
  unsigned long m_mask;
public:
  RuleAction(const std::string &groupName = "", unsigned long mask = 0xFFFFFFFF) 
    : m_groupName(groupName), m_mask(mask) 
  {
  }
  const std::string &getGroupName() const { return m_groupName; }
  unsigned long getMask() const { return m_mask; }
};

class Rule {
private:
  NonTerminalSymbol *m_left;    // not owner
  std::vector<Symbol*> m_right; // not owner
  int m_id;
  RuleAction m_action;
public:
  Rule(
    NonTerminalSymbol *left, 
    const std::vector<Symbol*> &right,
    const RuleAction &action) 
    : m_left(left)
    , m_right(right)
    , m_id(-1)
    , m_action(action) {}
  ~Rule() { }

  int getId() const { return m_id; }
  void setId(int id) { m_id = id; }

  // Grammar *getGrammar() const { return m_left->getGrammar(); }

  const NonTerminalSymbol *getLeftSymbol() const { return m_left; }

  int getLength() const { return m_right.size(); }
  const Symbol *getRightSymbol(int index) const { return index<(int)m_right.size() ? m_right[index] : 0; }

  const RuleAction &getAction() const { return m_action; }
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

  const Rule *addRule(Rule *rule); // takes ownership
  
  int getRuleCount() const { return (int)m_rules.size(); }
  const Rule *getRule(int index) const { return m_rules[index]; }

  void getRulesByLeftSymbol(std::vector<const Rule*> &rules, const std::string &leftSymbolName) const;

private:
  // Symbol *getSymbol(ITokenizer *tokenizer);
};


class RuleBuilder {
  Grammar *m_grammar;
  NonTerminalSymbol *m_leftSymbol;
  std::vector<Symbol*> m_symbols;
public:
  RuleBuilder(Grammar *g, const std::string &leftSymbolName);
  RuleBuilder &t(const std::string &t);
  RuleBuilder &t(TerminalSymbol *terminal);
  // RuleBuilder &t(Token::Type ty);
  RuleBuilder &n(const std::string &ntName);
  const Rule *end(const RuleAction &action = RuleAction());
};


#endif

