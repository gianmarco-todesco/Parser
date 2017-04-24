#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <map>

class Token {
public:
  enum Type { T_None, T_Ident, T_Number, T_QuotedString, T_Special, T_Eol, T_Eof };

  typedef long int Position;

  Token(); // a T_None-Token
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

  void convertEofToEol() { if(m_type==T_Eof) m_type = T_Eol; } 

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
public:
  BaseTokenizer() : m_read(false) {}
  virtual ~BaseTokenizer() {}

  // this method reads actually the tokens; returns false if could not read
  virtual bool read() = 0;

  const std::vector<Token> &getTokens() const { return m_tokens; } 

  // return line and columns of the given token position
  virtual std::pair<int, int> getLineAndColumn(Token::Position tokenPosition) const = 0;

  // write filename (if any), line and column; dump the whole line with a mark under the position
  virtual void dumpPosition(std::ostream &out, Token::Position tokenPosition) const = 0;  
};

//
// single string tokenizer
//
class StringTokenizer : public BaseTokenizer {
  std::string m_buffer;
public:
  StringTokenizer(const std::string &buffer) : m_buffer(buffer) { }
  ~StringTokenizer() {}

  static void read(std::vector<Token> &tokens, const std::string &text, int startPos = 0);


  std::pair<int, int> getLineAndColumn(Token::Position tokenPosition) const;
  void dumpPosition(std::ostream &out, Token::Position tokenPosition) const;  

  bool read() { read(m_tokens, m_buffer); return true; }
};

/*

class FileTokenizer : public BaseTokenizer {
  const std::string m_filepath;
  std::vector<std::pair<int, int> > m_lines; // (line first token position, lineNumber)
public:
  FileTokenizer(const std::string &filepath) : m_filepath(filepath) {}
  ~FileTokenizer() {}
  void read();
  std::string getLocation(long int tokenPos) const;
};
*/


#endif

