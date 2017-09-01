#include "parsetree.h"
#include <assert.h>
#include <sstream>

using namespace std;

ParseTree::ParseTree(const BaseTokenizer *tokenizer)
  : m_tokenizer(tokenizer)
{
  // m_buffer.reserve(10000000);
}

ParseTree::~ParseTree()
{
}

void ParseTree::addLeaf(int tokenIndex)
{
  m_stack.push_back((int)m_buffer.size());
  m_buffer.push_back(3); // size
  m_buffer.push_back(-1); // is a token
  m_buffer.push_back(tokenIndex);
}

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

// replace count elements from the stack with the index-th last element; update the buffer
void ParseTree::takeOne(int count, int index)
{
  assert(0<=index && index<count);
  int h = (int)m_stack.size() - count; 
  assert(h>=0);
  int k = m_stack[h];
  int k1 = m_stack[h+index];
  if(k1>k) m_buffer.erase(m_buffer.begin()+k, m_buffer.begin()+k1);
  k1 = k + m_buffer[k];
  if(k1<(int)m_buffer.size()) m_buffer.erase(m_buffer.begin()+k1, m_buffer.end());
  m_stack.erase(m_stack.begin()+h, m_stack.end());
  m_stack.push_back(k);
}

// remove count elements from the stack, push a node with the given tag to the stack; 
// the node contains a number of old elements according to mask
// update the buffer
// update the tag table (tags are stored with their ids)
void ParseTree::makeNode(const std::string &tag, int count, unsigned long mask)
{
  // add the group to the 
  pair<map<string, int>::iterator, bool> res = m_nodeTagTable.insert(make_pair(tag,  m_nodeTags.size()));
  if(res.second) m_nodeTags.push_back(tag);
  int tagIndex = res.first->second;
    

  static int tmpBuffer[1000000];

  /*
  vector<int> tmpBuffer;
  tmpBuffer.reserve(10000);
  tmpBuffer.push_back(0);
  tmpBuffer.push_back(tagIndex);
  tmpBuffer.push_back(0); // child count
  */
  tmpBuffer[0] = 0;
  tmpBuffer[1] = tagIndex;
  tmpBuffer[2] = 0;
  
  int tmpBufferLength = 3;

  int h = (int)m_stack.size() - count; 
  assert(h>=0);
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

    // tmpBuffer.insert(tmpBuffer.end(), m_buffer.begin()+a, m_buffer.begin()+b);
    // memcpy(&(tmpBuffer[tmpBufferLength]), &(m_buffer[a]), sizeof(int)*(b-a));
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
    k += 3;
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
  else
  {
    ostringstream ss;
    for(int i=0;i<getChildCount();i++)
    {
      string childText = getChild(i).getText(includeLeadingSpace || i>0);
      ss << childText.c_str();
    }
    return ss.str();
  }
}
