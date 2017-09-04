#ifndef PARSETREE_H
#define PARSETREE_H

#include "token.h"


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

  const BaseTokenizer *getTokenizer() const { return m_tokenizer; }

  std::pair<Token::Position, Token::Position> getTokensRange(const ParseNode &node) const;
  
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

  std::string toString() const; // used mainly for debugging

  std::string getText(bool includeLeadingSpace = false) const;
};


#endif

