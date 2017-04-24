#include "token.h"
#include "catch.hpp"
#include <strstream>

using namespace std;



TEST_CASE( "class Token" ) {
  REQUIRE(Token() == Token(Token::T_None, ""));
  REQUIRE(Token(Token::T_Special,"a") != Token(Token::T_Ident, "a"));
  REQUIRE(Token(Token::T_Ident,"pippo").getText() == "pippo");
  REQUIRE(Token(Token::T_Ident,"pippo").getType() == Token::T_Ident);

  Token eof(Token::T_Eof);
  REQUIRE(eof.isEof());
  REQUIRE(eof.getText() == "");
  REQUIRE(eof.getType() == Token::T_Eof);

  Token token;
  token.setSpaces(" \t");
  REQUIRE(token.getSpaces() == " \t");
  token.setPosition(987654321);
  REQUIRE(token.getPosition() == 987654321L);
  REQUIRE(Token(Token::T_Special, "+") == Token(Token::T_Special, "+"));
  REQUIRE(Token(Token::T_Special, "+") != Token(Token::T_Special, "-"));
  REQUIRE(Token(Token::T_Special, "+") != Token(Token::T_None, "+"));
}

TEST_CASE( "ostream << Token" ) {
  strstream ss;
  ss  << Token() << "," 
      << Token(Token::T_Ident, "hello") << ","
      << Token(Token::T_Number, "123") << ","
      << Token(Token::T_QuotedString, "'hello'") << ","
      << Token(Token::T_Special, "'+'") << ","
      << Token(Token::T_Eol) << ","
      << Token(Token::T_Eof) << ","
      << Token((Token::Type)123, "boh")
      << '\0';
  string expected = "EmptyToken,hello(Ident),123(Number),'hello'(QuotedString),'+'(Special),"
                    "EOL,EOF,boh(??123)";
  
  REQUIRE(ss.str() == expected);
  
}


void check(string text, Token tokens[])
{
  StringTokenizer st(text);
  REQUIRE(st.getTokens().empty());
  bool ret = st.read();
  REQUIRE(ret);
  int i;
  for(i=0; tokens[i] != Token(); i++) {
    REQUIRE(i<(int)st.getTokens().size());
    REQUIRE(tokens[i] == st.getTokens()[i]);
  }
  REQUIRE(i == st.getTokens().size());
}

TEST_CASE( "StringTokenizer 1" ) {
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

};
