#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <map>

class Token {
public:
  enum Type { T_None, T_Ident, T_Number, T_QuotedString, T_Special, T_Eol, T_Eof };

  typedef long int Position;

  Token(); // initialize a T_None-Token
  Token(Type type, const std::string &text = "");
  ~Token();

  std::string getText() const { return m_text; }
  Type getType() const { return m_type; }

  // spaces before the token
  void setSpaces(const std::string &spaces) { m_spaces=spaces; }
  std::string getSpaces() const { return m_spaces; }

  // index of the token (in the sequence read by the tokenizer)
  void setPosition(Position position) { m_position = position; }
  Position getPosition() const { return m_position; }

  bool isEof() const { return m_type == T_Eof; }
  bool isEol() const { return m_type == T_Eol; }

  bool operator==(const Token &other) const { return m_type == other.m_type && m_text == other.m_text; }
  bool operator!=(const Token &other) const { return m_type != other.m_type || m_text != other.m_text; }

  static const Token Eof, Eol;

private:
  Type m_type;
  std::string m_spaces, m_text;
  Position m_position;
};

std::ostream &operator<<(std::ostream &out, const Token &token);

//
// base class for tokenizers
//
class BaseTokenizer {
protected:
  std::vector<Token> m_tokens;
  bool m_read;
  std::vector<Token::Position> m_lines; // position of first token of each line
  int m_lineCount;
  bool m_newLine;

public:
  BaseTokenizer() : m_read(false), m_lineCount(0), m_newLine(true) {}
  virtual ~BaseTokenizer() {}

  const std::vector<Token> &getTokens() const { return m_tokens; } 
  int getLineCount() const { return m_lineCount; }

  // return line and columns of the given token position
  std::pair<int, int> getLineAndColumn(Token::Position tokenPosition) const;
  std::pair<int, int> getLineAndColumn(const Token &token) const { return getLineAndColumn(token.getPosition()); }

  std::string getLineText(int lineNumber) const;
  std::string getLineTextAndArrow(int lineNumber, int columnNumber) const;
  std::string getLineTextAndArrow(const std::pair<int, int> lc) const { return getLineTextAndArrow(lc.first,lc.second); }

  // write filename (if any), line and column; dump the whole line with a mark under the position
  virtual void dumpPosition(std::ostream &out, Token::Position tokenPosition) const = 0;  
  void dumpPosition(std::ostream &out, const Token &token) const { dumpPosition(out, token.getPosition()); }  

protected:
  void addToken(Token::Type type, const std::string &value, const std::string &spaces);
  void readLine(const std::string &buffer);

};

//
// single string tokenizer
// (the token sequence is terminates with an EOF)
//
class StringTokenizer : public BaseTokenizer {
  std::string m_buffer;
public:
  StringTokenizer(const std::string &buffer);
  ~StringTokenizer() {}

  void dumpPosition(std::ostream &out, Token::Position tokenPosition) const;  

};


//
// file tokenizer
// (each line is terminated with an EOL; the whole sequence is terminates with an EOF)
// (note: the last line is always terminates with EOL, therefore the last two tokens are always EOL EOF)
//
class FileTokenizer : public BaseTokenizer {
  std::string m_filepath;
public:
  FileTokenizer() {}
  ~FileTokenizer() {}
  bool read(const std::string &filepath);

  // std::string getLocation(long int tokenPos) const;
  void dumpPosition(std::ostream &out, Token::Position tokenPosition) const;  
};

#endif

