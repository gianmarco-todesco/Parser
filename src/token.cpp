#include "token.h"

// #include <sstream>
#include <set>
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <assert.h>

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
    if(!(isalpha(buffer[pos]) || buffer[pos]=='_')) return false;
    int startPos = pos;
    pos++;
    while(pos<(int)buffer.size() && (isalnum(buffer[pos]) || buffer[pos]=='_')) pos++;
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
    else
    {
      if(buffer[pos]=='u' || buffer[pos]=='U') pos++;
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
    qstring = "";
    qstring.push_back(q);
    while(pos<n && buffer[pos] != q)
    {
      if(pos+1<n && buffer[pos]=='\\')
      {
        qstring.push_back(buffer[pos+1]);
        pos+=2;
      }
      else 
      {
        qstring.push_back(buffer[pos]);
        pos++;
      }
    }
    if(pos<n && buffer[pos] == q) { qstring.push_back(q); pos++; }
    return true;
  }
  
} // namespace

//=============================================================================

void BaseTokenizer::addToken(Token::Type type, const std::string &value, const std::string &spaces)
{
  Token token(type, value); 
  token.setPosition(m_tokens.size());
  token.setSpaces(spaces);
  m_tokens.push_back(token);
  if(m_newLine)
  {
    m_lines.push_back(token.getPosition()); 
    ++m_lineCount;
    assert(m_lines.size() == m_lineCount);
    if(token.isEof()) --m_lineCount;
  }
  m_newLine = token.isEol();
}


void BaseTokenizer::readLine(const std::string &buffer)
{
  m_newLine=true;
  int i = 0;
  int n = (int)buffer.size();
  string spaces;
  Token token;
  for(;;)
  {
    int pos = i;
    // skip spaces and tabs
    while(i<n && (buffer[i]==' ' || buffer[i]=='\t')) i++;
    spaces = buffer.substr(pos,i-pos);
    // int j = i;
    Token::Type type = Token::T_None;
    string value;
    if(i>=n) 
      break;
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
    addToken(type,value,spaces);
  }
}



pair<int, int> BaseTokenizer::getLineAndColumn(Token::Position tokenPosition) const
{
  if(m_tokens.empty() || m_lines.empty()) return make_pair(0,0);
  if(tokenPosition<=0) return make_pair(1,m_tokens.front().getSpaces().length()+1);
  else if(tokenPosition>=(Token::Position)m_tokens.size()-1) return make_pair(m_lineCount+1,1);
  assert(m_lines.front() <= tokenPosition && tokenPosition<m_lines.back());
  int a = 0, b = m_lines.size()-1;
  while(b-a>1)
  {
    int c = (a+b)/2;
    if(m_lines[c] <= tokenPosition) a=c;
    else b=c;
  }
  assert(m_lines[a] <= tokenPosition && tokenPosition<m_lines[b]);
  assert(b-a==1);
  int lineNumber = a+1;
  int columnNumber = 1;
  int j = m_lines[a];
  while(j<tokenPosition) 
  {
    columnNumber += m_tokens[j].getSpaces().length() + m_tokens[j].getText().length();
    j++;
  }
  columnNumber += m_tokens[j].getSpaces().length();
  return make_pair(lineNumber, columnNumber);
}


std::string BaseTokenizer::getLineText(int lineNumber) const
{
  if(m_tokens.empty() || m_lines.empty() || lineNumber>m_lineCount) return "";
  int a;
  if(lineNumber<=1)
  {
    lineNumber = 1;
    a=0;
  }
  else 
  {
    a = 0;
    int b = (int)m_lines.size()-1;
    assert(a+1<=lineNumber && lineNumber<b+1);
    while(b-a>1)
    {
      int c=(a+b)/2;
      if(c+1<=lineNumber) a=c;
      else b=c;
    }
    assert(b-a==1);
    assert(a+1<=lineNumber && lineNumber<b+1);
  }
  int t0 = m_lines[a];
  assert(a+1<(int)m_lines.size());
  int t1 = m_lines[a+1]-1;
  assert(t0<=t1);
  assert(m_tokens[t1].isEol() || m_tokens[t1].isEof());
  string line = "";
  for(int t=t0;t<t1;t++)
  {
    line.append(m_tokens[t].getSpaces());
    line.append(m_tokens[t].getText());
  }
  line.append(m_tokens[t1].getSpaces());
  return line;
}

std::string BaseTokenizer::getLineTextAndArrow(int lineNumber, int columnNumber) const
{
  if(lineNumber > m_lineCount) return "<EOF>\n^\n";
  string line = getLineText(lineNumber);
  if(columnNumber>(int)line.length()) { columnNumber = line.length()+1; line += "<EOL>"; }
  line.push_back('\n');
  for(int i=0;i<columnNumber-1;i++)
  {
    if(line[i]=='\t') line.push_back('\t');
    else line.push_back(' ');
  }
  line.append("^\n");
  return line;
}

//=============================================================================

StringTokenizer::StringTokenizer(const string &buffer) 
  : m_buffer(buffer)
{ 
  readLine(m_buffer); 
  addToken(Token::T_Eof,"","");
  m_lines.push_back(m_tokens.size());
}

void StringTokenizer::dumpPosition(std::ostream &out, Token::Position tokenPosition) const
{
  pair<int, int> rc = getLineAndColumn(tokenPosition);
  out << "at";
  if(getLineCount()>1) out << " Ln:" << rc.first; 
  out << " Col:" << rc.second << endl;
  string lineAndArrow = getLineTextAndArrow(rc);
  out << lineAndArrow;
}


//-----------------------------------------------------------------------------


bool FileTokenizer::read(const std::string &filepath)
{
  m_read = false;
  m_lineCount = 0;
  m_newLine = true;
  m_filepath = filepath;
  m_lines.clear();
  m_tokens.clear();
  ifstream f(m_filepath.c_str());
  if(!f) return false;
  std::string text;

  while(std::getline(f,text)) 
  {
    readLine(text);
    addToken(Token::T_Eol,"","");    
  }
  addToken(Token::T_Eof,"","");
  return true;
}



void FileTokenizer::dumpPosition(std::ostream &out, Token::Position tokenPosition) const
{
  pair<int, int> rc = getLineAndColumn(tokenPosition);
  out << "File: " << m_filepath.c_str();
  if(getLineCount()>1) out << " Ln:" << rc.first; 
  out << " Col:" << rc.second << endl;
  string lineAndArrow = getLineTextAndArrow(rc);
  out << lineAndArrow;
}
