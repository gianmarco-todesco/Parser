#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <vector>
#include <set>
#include <map>
#include <string>

#include "token.h"

class Grammar;
class SymbolPool;

//
// Symbol
// 
class Symbol {
  std::string m_name;
  static int m_totalCount;
  friend class SymbolPool;
public:
  virtual ~Symbol() { m_totalCount--; }

  const std::string &getName() const { return m_name; }
  virtual bool isTerminal() const = 0;
  bool isNonTerminal() const { return !isTerminal(); }
  virtual bool matches(const Token &token) const = 0;
  virtual bool isConstant() const = 0;

  static int getTotalCount() { return m_totalCount; }

protected:
  Symbol(const std::string &name) : m_name(name) { m_totalCount++; }
  void setName(const std::string &name) { m_name = name; }
};

std::ostream &operator<<(std::ostream &out, const Symbol &nt);


//
// NonTerminalSymbol
// 
class NonTerminalSymbol : public Symbol {
  friend class SymbolPool;
public:
  bool isTerminal() const { return false; }
  bool isConstant() const { return false; }
  bool matches(const Token &) const { return false; }
protected:
  NonTerminalSymbol(const std::string &name) : Symbol(name) {}
};

//
// TerminalSymbol
// 
class TerminalSymbol : public Symbol {
  friend class SymbolPool;
public:
  virtual ~TerminalSymbol() {}
  bool isTerminal() const { return true; }
  virtual int getConstantStrength() const = 0;
protected:
  TerminalSymbol(const std::string &name) : Symbol(name) {}
};


//
// TextTerminalSymbol
// 
class TextTerminalSymbol : public TerminalSymbol {
  std::string m_text;
  friend class SymbolPool;
public:
  bool isConstant() const { return true; }
  bool matches(const Token &token) const { return token.getText() == m_text; }
  int getConstantStrength() const { return 10; }
protected:
  TextTerminalSymbol(const std::string &text) : TerminalSymbol("'"+text+"'"), m_text(text) {}
  TextTerminalSymbol(const std::string &text, const std::string &name) : TerminalSymbol(name), m_text(text) {}
};

//
// TokenTypeTerminalSymbol
// 
class TokenTypeTerminalSymbol : public TerminalSymbol {
  Token::Type m_type;
  friend class SymbolPool;
public:
  bool isConstant() const { return m_type == Token::T_Eol || m_type == Token::T_Eof; }
  bool matches(const Token &token) const { return token.getType() == m_type; }
  int getConstantStrength() const { return 5; }
protected:
  TokenTypeTerminalSymbol(const std::string &name, Token::Type type) : TerminalSymbol(name), m_type(type) {}
};

/*

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
*/

//
// AnyTerminalSymbol
// 
class AnyTerminalSymbol : public TerminalSymbol {
  friend class SymbolPool;
public:
  virtual ~AnyTerminalSymbol() {}
  bool isConstant() const { return false; }
  bool matches(const Token &) const { return true; }
  int getConstantStrength() const { return 1; }
protected:
  AnyTerminalSymbol() : TerminalSymbol("any") {}
};

//
// SymbolPool
// 
class SymbolPool {
  std::map<std::string, NonTerminalSymbol*> m_ntSymbols;
  std::map<std::string, TerminalSymbol*> m_tSymbols;
public:
  SymbolPool();
  ~SymbolPool();

  const NonTerminalSymbol* getNonTerminalSymbol(const std::string &name, bool createIfNeeded=true);

  const TerminalSymbol* getTextTerminalSymbol(const std::string &text, bool createIfNeeded=true);
  const TerminalSymbol* getIdentTerminalSymbol() { return getTokenTypeTerminalSymbol("ident", Token::T_Ident); }
  const TerminalSymbol* getNumberTerminalSymbol()  { return getTokenTypeTerminalSymbol("number", Token::T_Number); }
  const TerminalSymbol* getQuotedStringTerminalSymbol()  { return getTokenTypeTerminalSymbol("qstring", Token::T_QuotedString); }
  const TerminalSymbol* getSpecialTerminalSymbol()  { return getTokenTypeTerminalSymbol("special", Token::T_Special); }
  const TerminalSymbol* getEolTerminalSymbol() { return getTokenTypeTerminalSymbol("EOL", Token::T_Eol); }
  const TerminalSymbol* getEofTerminalSymbol() { return getTokenTypeTerminalSymbol("EOF", Token::T_Eof); }
  const TerminalSymbol* getAnyTerminalSymbol();

  const NonTerminalSymbol* nt(const std::string &name) { return getNonTerminalSymbol(name); }
  const TerminalSymbol* t(const std::string &text) { return getTextTerminalSymbol(text); }
  const TerminalSymbol* eof() { return getEofTerminalSymbol(); }
  
  void getNonTerminalSymbols(std::vector<const NonTerminalSymbol*> &nts) const;
  void getNonTerminalSymbolNames(std::vector<std::string> &nts) const;

protected:
  TerminalSymbol* getTerminalSymbol(const std::string &name);
  TerminalSymbol* getTokenTypeTerminalSymbol(const std::string &name, Token::Type type);
  
};

//
// RuleAction
// 
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

//
// Rule
// 
class Rule {
private:
  const NonTerminalSymbol *m_left;    // not owner
  std::vector<const Symbol*> m_right; // not owner
  int m_id;
  RuleAction m_action;
public:
  Rule(
    const NonTerminalSymbol *left, 
    const std::vector<const Symbol*> &right,
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


//
// Grammar
// 
class Grammar {
  SymbolPool m_symbols;
  std::vector<Rule*> m_rules;
  const NonTerminalSymbol *m_rootSymbol;
  mutable std::set<std::string> m_nullables;
  mutable std::map<std::string, std::set<const TerminalSymbol*> > m_firsts;
  mutable bool m_dirty;

public:
  Grammar();
  ~Grammar();

  SymbolPool &symbols() { return m_symbols; }
  const SymbolPool &symbols() const { return m_symbols; }

  void clear();

  const Rule *addRule(Rule *rule); // takes ownership; n.b. the left-side of the first rule becomes the start symbol
  
  int getRuleCount() const { return (int)m_rules.size(); }
  const Rule *getRule(int index) const { return m_rules[index]; }

  void getRulesByLeftSymbol(std::vector<const Rule*> &rules, const std::string &leftSymbolName) const;

  const NonTerminalSymbol *getStartSymbol() const;
  void setStartSymbol(const NonTerminalSymbol*nt);

  const NonTerminalSymbol *getRootSymbol() const { return m_rootSymbol; }  

  void dump(std::ostream &out) const;

  const std::set<std::string> &getNullables() const;
  bool isNullable(std::string ntName) const;
  bool isNullable(const NonTerminalSymbol *nt) const { return isNullable(nt->getName()); }

  const std::set<const TerminalSymbol*> &getFirstSet(const std::string &ntName) const;

private:
  void updateRootRule(const NonTerminalSymbol *startSymbol);
  void computeAll() const;
};


//
// RuleBuilder
// 
class RuleBuilder {
  Grammar *m_grammar;
  const NonTerminalSymbol *m_leftSymbol;
  std::vector<const Symbol*> m_symbols;
public:
  RuleBuilder(Grammar *g, const std::string &leftSymbolName);
  RuleBuilder &t(const std::string &t);
  RuleBuilder &t(const TerminalSymbol *terminal);
  RuleBuilder &id() { return t(m_grammar->symbols().getIdentTerminalSymbol()); }
  RuleBuilder &number() { return t(m_grammar->symbols().getNumberTerminalSymbol()); }
  RuleBuilder &special() { return t(m_grammar->symbols().getSpecialTerminalSymbol()); }
  RuleBuilder &qstring() { return t(m_grammar->symbols().getQuotedStringTerminalSymbol()); }
  RuleBuilder &eol() { return t(m_grammar->symbols().getEolTerminalSymbol()); }
  RuleBuilder &eof() { return t(m_grammar->symbols().getEofTerminalSymbol()); }
  RuleBuilder &any() { return t(m_grammar->symbols().getAnyTerminalSymbol()); }
  RuleBuilder &n(const std::string &ntName);
  const Rule *end(const RuleAction &action = RuleAction());
};


#endif

