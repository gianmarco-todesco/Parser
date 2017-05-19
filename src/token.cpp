#include "token.h"

// #include <sstream>
#include <set>
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <fstream>

using namespace std;
  

Token::Token()
  : m_type(T_None)
  , m_spaces("")
  , m_text("")
  , m_position(-1)
{
}

Token::Token(Type type, const std::string &text)
  : m_type(type)
  , m_spaces("")
  , m_text(text)
  , m_position(-1)
{
}

Token::~Token()
{
}

const Token Token::Eof(Token::T_Eof);
const Token Token::Eol(Token::T_Eol);


std::ostream &operator<<(std::ostream &out, const Token &token) 
{
  switch(token.getType())
  {
  case Token::T_None: return out << "EmptyToken";
  case Token::T_Ident: return out << token.getText().c_str() << "(Ident)";
  case Token::T_Number: return out << token.getText().c_str() << "(Number)";
  case Token::T_QuotedString: return out << token.getText().c_str() << "(QuotedString)";
  case Token::T_Special: return out << token.getText().c_str() << "(Special)";
  case Token::T_Eof: return out << "EOF";
  case Token::T_Eol: return out << "EOL";
  default: return out << token.getText().c_str()  << "(??" << (int)token.getType() << ")" ;
  }
}

//=============================================================================

namespace {

  bool matchIdent(const std::string &buffer, int &pos, std::string &ident) {
    if(!isalpha(buffer[pos])) return false;
    int startPos = pos;
    pos++;
    while(pos<(int)buffer.size() && isalnum(buffer[pos])) pos++;
    ident = buffer.substr(startPos, pos-startPos);
    return true;
  }

  bool matchNumber(const std::string &buffer, int &pos, std::string &number) {
    int startPos = pos, n = buffer.length();
    if(!(isdigit(buffer[pos]) || (buffer[pos]=='.' && pos+1<n && isdigit(buffer[pos+1]))))
      return false;
    pos++;
    while(pos<n && isdigit(buffer[pos]))pos++;
    if(pos<n && buffer[pos]=='.')
    {
      pos++;
      while(pos<n && isdigit(buffer[pos]))pos++;
    }
    if(pos<n && (buffer[pos]=='e' || buffer[pos]=='E'))
    {
      int h = pos+1;
      if(h<n && buffer[h]=='-') h++;
      while(h<n && isdigit(buffer[h])) h++;
      if(isdigit(buffer[h-1])) pos = h;
    }
    number = buffer.substr(startPos, pos - startPos);
    return true;
  }

  bool matchQuotedString(const std::string &buffer, int &pos, std::string &qstring) {
    int startPos = pos, n = buffer.length();
    char q = buffer[pos];
    if(q!='"' && q!='\'') return false;
    pos++;
    while(pos<n && buffer[pos] != q)
    {
      if(buffer[pos]=='\\') pos++;
      if(pos<n) pos++;
    }
    if(pos<n && buffer[pos] == q) pos++;
    qstring = buffer.substr(startPos, pos-startPos);
    return true;
  }
  
} // namespace


void StringTokenizer::read(vector<Token> &tokens, const std::string &buffer, int tokenPos)
{
  int i = 0;
  int n = (int)buffer.size();
  for(;;)
  {
    int pos = i;
    // skip spaces and tabs
    while(i<n && (buffer[i]==' ' || buffer[i]=='\t')) i++;
    string spaces = buffer.substr(pos,i-pos);
    // int j = i;
    Token::Type type = Token::T_None;
    string value;
    if(i>=n) 
      type = Token::T_Eof;
    else if(matchIdent(buffer, i, value)) 
      type = Token::T_Ident;
    else if(matchNumber(buffer, i, value))
      type = Token::T_Number;
    else if(matchQuotedString(buffer, i, value))
      type = Token::T_QuotedString;
    else if(buffer[i]=='\n')
    {
      type = Token::T_Eol;
      i++;
    }
    else if(buffer[i]=='\r' && i+1<n && buffer[i+1]=='\n')
    {
      type = Token::T_Eol;
      i+=2;
    }
    else
    {
      value = buffer.substr(i, 1); i++;
      type = Token::T_Special;
    }

    Token token = Token(type, value); 
    token.setPosition(tokenPos++);
    token.setSpaces(spaces);
    tokens.push_back(token);
    if(token.isEof()) break;
  }
}

pair<int, int> StringTokenizer::getLineAndColumn(Token::Position tokenPosition) const
{
  int c = 0;
  for(vector<Token>::const_iterator it = m_tokens.begin(); it != m_tokens.end(); ++it)
  {
    const Token &token = *it;
    c += token.getSpaces().length();
    if(tokenPosition <= token.getPosition()) break;
    c += token.getText().length();
  }
  return make_pair(1,c+1);
}

void StringTokenizer::dumpPosition(std::ostream &out, Token::Position tokenPosition) const
{
  pair<int, int> rc = getLineAndColumn(tokenPosition);
  out << "at column " << rc.second << ":" << endl;
  for(int i=0; i+1<(int)m_tokens.size(); i++)
  {
    const Token &token = m_tokens[i];
    out << token.getSpaces().c_str() << token.getText().c_str();
  }
  out << endl;
  for(int i=0; i+1<(int)m_tokens.size(); i++)
  {
    const Token &token = m_tokens[i];
    out << token.getSpaces().c_str() << string(' ', token.getText().length());
  }
  out << "^" << endl;  
}

//-----------------------------------------------------------------------------

string FileTokenizer::getLocation(long int tokenPos) const
{
  return "";
}


bool FileTokenizer::read(const std::string &filepath)
{
  m_filepath = filepath;
  m_lines.clear();
  m_tokens.clear();
  ifstream f(m_filepath.c_str());
  if(!f) return false;
  std::string text;
  int tokenPos = 0;
  int lineNumber = 0;
  while(std::getline(f,text)) 
  {
    m_lines.push_back(make_pair(tokenPos, lineNumber));
    StringTokenizer::read(m_tokens, text, tokenPos);
    tokenPos = m_tokens.back().getPosition();
    m_tokens.pop_back();
    m_tokens.push_back(Token::Eol);
    m_tokens.back().setPosition(tokenPos);
    tokenPos++;
    lineNumber++;
  }
  m_tokens.push_back(Token::Eof);
  return true;
}


pair<int, int> FileTokenizer::getLineAndColumn(Token::Position tokenPosition) const
{
  return make_pair(0,0);
}

void FileTokenizer::dumpPosition(std::ostream &out, Token::Position tokenPosition) const
{
}

/*
std::string StringTokenizer::getLocation(long int tokenPos) const
{
  strstream ss;
  ss << " at " << (c+1) << '\0';
  return ss.str();
}

//-----------------------------------------------------------------------------

void FileTokenizer::read()
{
  
}

//-----------------------------------------------------------------------------

std::string FileTokenizer::getLocation(long int tokenPos) const 
{
  int lineFirstTokenPos, lineNumber;
  if(tokenPos<=0) lineFirstTokenPos = lineNumber = 0;
  else if(tokenPos>=m_lines.back().first) 
  {
    lineFirstTokenPos = m_lines.back().first;
    lineNumber = m_lines.back().second;
  }
  else
  {
    int a=0, b=(int)m_lines.size()-1;
    // assert(m_lines[a].first<=tokenPos && tokenPos<m_lines[b].first);
    while(b-a>1)
    {
      int c = (a+b)/2;
      if(m_lines[c].first<=tokenPos) a=c; else b=c;
    }
    lineFirstTokenPos = m_lines[a].first;
    lineNumber = m_lines[a].second;
  }

  int col = 0;
  int i = lineFirstTokenPos;
  while(i<(int)m_tokens.size() && i<tokenPos) 
  { 
    const Token &token = m_tokens[i];
    col += token.getSpaces().length() + token.getText().length();
    i++;
  }
  if(i==tokenPos) col += m_tokens[i].getSpaces().length();

  strstream ss;
  ss << "at line " << (lineNumber+1) << ", col " << (col+1) << '\0';
  return ss.str();
}

*/
