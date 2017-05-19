#include "token.h"
#include "catch.hpp"
#include <sstream>
#include <fstream>

using namespace std;


TEST_CASE( "class Token", "[token]" ) {
  REQUIRE(Token() == Token(Token::T_None, ""));
  REQUIRE(Token(Token::T_Special,"a") != Token(Token::T_Ident, "a"));
  REQUIRE(Token(Token::T_Ident,"pippo").getText() == "pippo");
  REQUIRE(Token(Token::T_Ident,"pippo").getType() == Token::T_Ident);

  Token eof(Token::T_Eof);
  REQUIRE(eof.isEof());
  REQUIRE(!eof.isEol());
  REQUIRE(eof.getText() == "");
  REQUIRE(eof.getType() == Token::T_Eof);
  REQUIRE(Token::Eof == eof);

  Token eol(Token::T_Eol);
  REQUIRE(!eol.isEof());
  REQUIRE(eol.isEol());
  REQUIRE(eol.getText() == "");
  REQUIRE(eol.getType() == Token::T_Eol);
  REQUIRE(Token::Eol == eol);

  Token token;
  token.setSpaces(" \t");
  REQUIRE(token.getSpaces() == " \t");
  token.setPosition(987654321);
  REQUIRE(token.getPosition() == 987654321L);
  REQUIRE(Token(Token::T_Special, "+") == Token(Token::T_Special, "+"));
  REQUIRE(Token(Token::T_Special, "+") != Token(Token::T_Special, "-"));
  REQUIRE(Token(Token::T_Special, "+") != Token(Token::T_None, "+"));

}

TEST_CASE( "ostream << Token", "[token]" ) {
  ostringstream ss;
  ss  << Token() << "," 
      << Token(Token::T_Ident, "hello") << ","
      << Token(Token::T_Number, "123") << ","
      << Token(Token::T_QuotedString, "'hello'") << ","
      << Token(Token::T_Special, "'+'") << ","
      << Token(Token::T_Eol) << ","
      << Token(Token::T_Eof) << ","
      << Token((Token::Type)123, "boh");
  string expected = "EmptyToken,hello(Ident),123(Number),'hello'(QuotedString),'+'(Special),"
                    "EOL,EOF,boh(??123)";
  
  REQUIRE(ss.str() == expected);
  
}


void compareTokens(const std::vector<Token> &tokens, const Token refTokens[])
{
  int i;
  for(i=0; refTokens[i] != Token(); i++) {
    REQUIRE(i<(int)tokens.size());
    REQUIRE(refTokens[i] == tokens[i]);
  }
  REQUIRE(i == tokens.size());
}


void check(string text, Token refTokens[])
{
  StringTokenizer st(text);
  compareTokens(st.getTokens(), refTokens);

}

TEST_CASE( "StringTokenizer 1", "[token]" ) {
  Token tokens1[] = {
    Token(Token::T_Ident,"uno"),
    Token(Token::T_Ident,"due"),
    Token(Token::T_Ident,"tre"),
    Token(Token::T_Special,"+"),
    Token(Token::T_Number,"12.34"),
    Token(Token::T_Special,"/"),
    Token(Token::T_Number,"4"),
    Token(Token::T_Special,"<"),
    Token(Token::T_Number,"67"),
    Token(Token::T_Special,">"),
    Token(Token::T_Eof),
    Token()
  };
  check("uno due\ttre+12.34/4<67>", tokens1);

  Token tokens2[] = {
    Token(Token::T_QuotedString,"'hello'"),
    Token(Token::T_QuotedString,"'\"'"),
    Token(Token::T_QuotedString,"'\\''"),
    Token(Token::T_QuotedString,"\"hello\""),
    Token(Token::T_QuotedString,"\"'\""),
    Token(Token::T_QuotedString,"\"\\\"\""),
    Token(Token::T_Eof),
    Token()
  };

  check("   'hello' '\"' '\\''  \"hello\" \"'\"  \"\\\"\"  ", tokens2);

  Token tokens3[] = {
    Token(Token::T_Eol),
    Token(Token::T_Ident,"hello"),
    Token(Token::T_Eol),
    Token(Token::T_Eol),
    Token(Token::T_Eol),
    Token(Token::T_Eof),
    Token()
  };
  check("   \nhello\n\n\t\t\r\n", tokens3);

}


TEST_CASE( "FileTokenizer 1", "[token]" ) {
  string fn("test.txt");
  {
    ofstream fs(fn);
    fs << "uno due\n tre\n4";
  }
  FileTokenizer ft;
  REQUIRE(ft.read("does_not_exist.txt")==false);
  bool ret = ft.read(fn);
  REQUIRE(ret);

  Token tokens1[] = {
    Token(Token::T_Ident,"uno"),
    Token(Token::T_Ident,"due"),
    Token::Eol,
    Token(Token::T_Ident,"tre"),
    Token::Eol,
    Token(Token::T_Number,"4"),
    Token::Eol,
    Token::Eof,
    Token()
  };
  compareTokens(ft.getTokens(), tokens1);


}