#include "parsetree.h"
#include <assert.h>
#include <sstream>
#include <iostream>

using namespace std;

// buffer:
//   leaf => [size, -1, tokenIndex]
//   node => [size, tagIndex, firstToken, lastToken+1, childCount] [child1] [child2]... [childn]

ParseTree::ParseTree(const BaseTokenizer *tokenizer)
  : m_tokenizer(tokenizer)
{  
  m_buffer.reserve(10000);
}

ParseTree::~ParseTree()
{
}

// 
// create a leaf element; push it on the stack
//
void ParseTree::addLeaf(int tokenIndex)
{
  // leaf : size, -1, tokenIndex
  m_stack.push_back((int)m_buffer.size());
  m_buffer.push_back(3); // size
  m_buffer.push_back(-1); // tag = -1 => it is a leaf
  m_buffer.push_back(tokenIndex);
}

#ifndef CICCIO
void ParseTree::makeNode(const std::string &tag, int count, unsigned long mask)
{
  // node : size, tagIndex, childCount, startTokenIndex, lastTokenIndex .. (children)
  
  // create/get the tag index
  pair<map<string, int>::iterator, bool> res = m_nodeTagTable.insert(make_pair(tag,  m_nodeTags.size()));
  if(res.second) m_nodeTags.push_back(tag);
  int tagIndex = res.first->second;

  int h = (int)m_stack.size() - count; // index (in the stack) of the first element to remove
  assert(h>=0);

  // find node span
  int ta=m_tokenizer->getTokens().size();
  int tb=0;
  for(int i=0; i<count; i++)
  {
    int tokenIndex;
    int j = m_stack[h+i];
    if(m_buffer[j+1]<0) 
    {
      tokenIndex = m_buffer[j+2];
      if(tokenIndex<ta) ta = tokenIndex; 
      if(tokenIndex>tb) tb = tokenIndex;
    }
    else
    {
      tokenIndex = m_buffer[j+3];
      if(tokenIndex<ta) ta = tokenIndex; 
      tokenIndex = m_buffer[j+4];
      if(tokenIndex>tb) tb = tokenIndex;
    }
  }


  int start = h<(int)m_stack.size() ? m_stack[h] : m_buffer.size();
  int headerLength = 5;
  m_buffer.insert(m_buffer.begin()+start,headerLength,0);
  m_buffer[start+1] = tagIndex;
  m_buffer[start+3] = ta;
  m_buffer[start+4] = tb;
  
  int pos = start + headerLength;


  int fieldIndex = 0;
  for(int i=0; i<count; i++)
  {
    if(((mask>>i)&1)==0) continue;
    fieldIndex++;
    // buffer position of the i-th element is [a,b[
    int a = m_stack[h+i] + headerLength;
    int b = a + m_buffer[a];
    if(fieldIndex == 1 && m_buffer[a+1]==tagIndex) // left recursion
    {
      m_buffer[start+2] += m_buffer[a+2];
      a += headerLength;
    }
    else
    {
      m_buffer[start+2]++;
    }

    if(pos<a) 
    {
      memmove(&m_buffer[pos], &m_buffer[a], sizeof(int)*(b-a));
      pos += b-a;
    }
    else
    {
      pos += b-a;
    }
  }
  m_buffer[start] = pos-start;
  m_buffer.erase(m_buffer.begin()+pos, m_buffer.end());
  m_stack.erase(m_stack.begin()+h, m_stack.end());
  m_stack.push_back(start);  
}
#endif

#ifdef CICCIO
// 
// remove count elements from the stack
// create a node element with the given tag
// add children elements according to the mask
// push the new node element on the stack
void ParseTree::makeNode(const std::string &tag, int count, unsigned long mask)
{
  // create/get the tag index
  pair<map<string, int>::iterator, bool> res = m_nodeTagTable.insert(make_pair(tag,  m_nodeTags.size()));
  if(res.second) m_nodeTags.push_back(tag);
  int tagIndex = res.first->second;
    
  // bad! change it!
  static int tmpBuffer[1000000];  
  tmpBuffer[0] = 0; // size still unknown
  tmpBuffer[1] = tagIndex;
  tmpBuffer[2] = 0;
  
  int tmpBufferLength = 3;

  int h = (int)m_stack.size() - count; // index (in the stack) of the first element to remove
  assert(h>=0);

  // compute children number
  int fieldCount = 0;
  for(int i=0; i<count; i++)
  {
    if(((mask>>i)&1)!=0) fieldCount++;
  }

  int fieldIndex = 0;
  for(int i=0; i<count; i++)
  {
    if(((mask>>i)&1)==0) continue;
    fieldIndex++;
    // buffer position of the i-th element is [a,b[
    int a = m_stack[h+i];
    int b = a + m_buffer[a];
    if(fieldCount>1 && fieldIndex==1 && m_buffer[a+1]==tagIndex) // left recursion
    {
      tmpBuffer[2] += m_buffer[a+2];
      a+=3;
    }
    else
    {
      tmpBuffer[2]++;
    }

    for(int j=0; j<b-a; j++) tmpBuffer[tmpBufferLength+j] = m_buffer[a+j];
    tmpBufferLength += (b-a);
  }

  // tmpBuffer[0] = (int)tmpBuffer.size();
  tmpBuffer[0] = tmpBufferLength;

  int k;
  if(count>0)
  {
    k = m_stack[m_stack.size()-count];
    m_buffer.erase(m_buffer.begin()+k, m_buffer.end());
  }
  else
  {
    k = m_buffer.size();
  }
  //m_buffer.insert(m_buffer.end(), tmpBuffer.begin(), tmpBuffer.end());
  m_buffer.insert(m_buffer.end(), tmpBuffer, tmpBuffer + tmpBufferLength); //    tmpBuffer.begin(), tmpBuffer.end());

  m_stack.erase(m_stack.begin()+h, m_stack.end());
  m_stack.push_back(k);
}

#endif


/*
// remove count elements from the stack; remove them also from the buffer
void ParseTree::removeItems(int count)
{
  assert((int)m_stack.size()>=count);
  int k = m_stack[m_stack.size()-count];
  m_stack.erase(m_stack.end()-count,m_stack.end());
  m_buffer.erase(m_buffer.begin()+k, m_buffer.end());
}
*/

// replace count elements from the stack with the index-th element of the group; update the buffer
void ParseTree::takeOne(int count, int index)
{
  assert(0<=index && index<count);
  int h = (int)m_stack.size() - count; // h = first element of the group
  assert(h>=0);
  int k = m_stack[h]; // buffer position of the first element of the group
  int k1 = m_stack[h+index]; // buffer position of the index-th element of the group
  if(k1>k) 
    m_buffer.erase(m_buffer.begin()+k, m_buffer.begin()+k1); 
  k1 = k + m_buffer[k];
  if(k1<(int)m_buffer.size()) m_buffer.erase(m_buffer.begin()+k1, m_buffer.end());
  m_stack.erase(m_stack.begin()+h, m_stack.end());
  m_stack.push_back(k);
}

ParseNode ParseTree::getNode(int index) const
{
  assert(0<=index && index<(int)m_stack.size());
  int k = m_stack[index];
  return ParseNode(this,k);
}

ParseNode ParseTree::getChildNode(const ParseNode &parent, int childIndex) const
{
  int k = parent.getPosition();
  if(0<=childIndex && childIndex<m_buffer[k+2])
  {
    k += 5;
    for(int i=0;i<childIndex;i++)
    {
      k += m_buffer[k];
    }
    return ParseNode(this, k);
  }
  else return ParseNode();
}

std::string ParseTree::getTag(const ParseNode &node) const
{
  int k = node.getPosition();
  if(m_buffer[k+1]>=0) return m_nodeTags[m_buffer[k+1]];
  else return "";
}

bool ParseTree::isLeaf(const ParseNode &node) const
{
  int k = node.getPosition();
  return m_buffer[k+1]<0;
}

Token ParseTree::getToken(const ParseNode &node) const
{
  int k = node.getPosition();
  if(m_buffer[k+1]<0)
    return m_tokenizer->getTokens()[m_buffer[k+2]];
  else
    return Token();
}

int ParseTree::getChildCount(const ParseNode &node) const
{
  int k = node.getPosition();
  if(m_buffer[k+1]<0) return 0;
  else return m_buffer[k+2];
}

std::pair<Token::Position, Token::Position> ParseTree::getTokensRange(const ParseNode &node) const
{
  int k = node.getPosition();
  if(m_buffer[k+1]<0) // leaf
  {
    int tokenIndex = m_buffer[k+2];
    return make_pair(tokenIndex,tokenIndex);
  }
  else
  {
    return make_pair(m_buffer[k+3], m_buffer[k+4]);
  }
}


namespace {

  void serialize(
    ostream &ss, 
    const ParseNode &node)
  {
    if(node.isLeaf())
    {
      Token token = node.getToken();
      if(token.isEof()) ss << "EOF";
      else if(token.isEol()) ss << "EOL";
      else if(token.getType() == Token::T_Ident || token.getType() == Token::T_Special)
        ss << "'" << token.getText() << "'";
      else if(token.getType() == Token::T_Number) ss << token.getText();
      else if(token.getType() == Token::T_QuotedString) ss << "q" << token.getText();
      else ss << "?" << token.getText() << "?";
    }
    else
    {
      ss << node.getTag().c_str() << "(";
      for(int i=0;i<node.getChildCount();i++)
      {
        if(i>0) ss<<",";
        serialize(ss, node.getChild(i));      
      }
      ss << ")";
    }
  }

  void dump(
    ostream &out,
    int level,
    const ParseNode &node)
  {
    for(int i=0;i<level;i++) out << "    ";
    if(node.isLeaf())
    {
      out << "'"<<node.getToken().getText()<<"'\n";
    }
    else
    {
      out << node.getTag().c_str() << "\n";
      for(int i=0;i<node.getChildCount();i++)
      {
        dump(out, level+1, node.getChild(i));  
      }
    }
  }

}// namespace 

std::string ParseTree::toString() const
{
  ostringstream ss;
  for(int i=0;i<getStackSize();i++)
  {
    if(i>0) ss<<",";
    serialize(ss, getNode(i));
  }
  return ss.str();
}


void ParseTree::dump(ostream &out) const 
{
  for(int i=0;i<(int)m_stack.size();i++)
  {
    ::dump(out, 0, getNode(i));
  }
}


std::string ParseNode::toString() const
{
  ostringstream ss;
  serialize(ss, *this);
  return ss.str();
}


std::string ParseNode::getText(bool includeLeadingSpace) const
{
  if(isLeaf())
  {
    Token token = getToken();
    string ls = includeLeadingSpace ? token.getSpaces() : ""; 
    if(token.isEol()) return ls + "\n";
    else return ls + token.getText();
  }
  else if(getChildCount()==0)
  {
    return "";
  }
  else
  {
    pair<Token::Position, Token::Position> range = m_parseTree->getTokensRange(*this);
    ostringstream ss;
    for(int i=range.first;i<=range.second;i++)
    {
      Token token = m_parseTree->getTokenizer()->getTokens()[i];
      if(includeLeadingSpace || i>range.first) ss << token.getSpaces();
      if(token.isEol()) ss << "\n";
      else ss << token.getText();
    }
    return ss.str();
  }
}
