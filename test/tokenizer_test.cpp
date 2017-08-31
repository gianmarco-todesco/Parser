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
    Token(Token::T_Eol),
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
    Token(Token::T_Eol),
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
    Token(Token::T_Eol),
    Token(Token::T_Eof),
    Token()
  };
  check("   \nhello\n\n\t\t\r\n", tokens3);

  Token tokens4[] = {
    Token(Token::T_Ident, "Hello"),
    Token(Token::T_Special, ","),
    Token(Token::T_Ident,"World"),
    Token(Token::T_Special, "!"),
    Token(Token::T_Eol),
    Token(Token::T_Ident,"last"),
    Token(Token::T_Ident,"line"),
    Token(Token::T_Eol),
    Token(Token::T_Eof),
    Token()
  };
  check(" Hello, World!\nlast line", tokens4);

}


TEST_CASE( "FileTokenizer 1", "[token]" ) {
  FileTokenizer ft;
  REQUIRE(ft.read("does_not_exist.txt")==false);
  bool ret = ft.read("data/test1.txt");
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


TEST_CASE( "StringTokenizer 2", "[token]" ) {
  StringTokenizer st("  Hello, World! \nSecond line");
  const std::vector<Token> &tokens = st.getTokens();
  REQUIRE(tokens.size() == 9);
  REQUIRE(st.getLineCount() == 2);
  int ps[][2] = {
    {1,3},{1,8},{1,10},{1,15},{1,17},
    {2,1},{2,8},{2,12},
    {3,1}
  };
  for(int i=0;i<9;i++)
  {
    pair<int,int> linePos = st.getLineAndColumn(i);
    REQUIRE(linePos.first == ps[i][0]);
    REQUIRE(linePos.second == ps[i][1]);
  }
  REQUIRE(st.getLineText(1) == "  Hello, World! ");
  REQUIRE(st.getLineText(2) == "Second line");
  REQUIRE(st.getLineText(3) == "");
}


TEST_CASE( "FileTokenizer 2", "[token]" ) {
  FileTokenizer ft;
  bool ret = ft.read("data/test1.txt");
  REQUIRE(ret);
  const std::vector<Token> &tokens = ft.getTokens();
  REQUIRE(tokens.size() == 8);
  REQUIRE(ft.getLineCount() == 3);
  int ps[][2] = {
    {1,1},{1,5},{1,8},
    {2,2},{2,5},
    {3,1},{3,2},
    {4,1}
  };
  for(int i=0;i<8;i++)
  {
    pair<int,int> linePos = ft.getLineAndColumn(i);
    REQUIRE(linePos.first == ps[i][0]);
    REQUIRE(linePos.second == ps[i][1]);
  }
  REQUIRE(ft.getLineText(1) == "uno due");
  REQUIRE(ft.getLineText(2) == " tre");
  REQUIRE(ft.getLineText(3) == "4");
  REQUIRE(ft.getLineText(4) == "");
}


TEST_CASE( "StringTokenizer 3", "[token]" ) {
  StringTokenizer st("  Hello, World! \nSecond line\n\tthe last one!");
  const std::vector<Token> &tokens = st.getTokens();
  REQUIRE(tokens.size() == 14);
  string la = st.getLineTextAndArrow(3,6);
  REQUIRE(la == "\tthe last one!\n\t    ^\n");
  la = st.getLineTextAndArrow(2,12);
  REQUIRE(la == "Second line<EOL>\n           ^\n");
  la = st.getLineTextAndArrow(4,1);
  REQUIRE(la == "<EOF>\n^\n");
  
}



TEST_CASE( "FileTokenizer; multiple read", "[token]" ) {
  FileTokenizer ft;
  bool ret = ft.read("data/test1.txt");
  REQUIRE(ret);
  REQUIRE(ft.getLineText(2) == " tre");
  ret = ft.read("data/test1.txt");
  REQUIRE(ret);
  REQUIRE(ft.getLineText(2) == " tre");
}


TEST_CASE( "FileTokenizer 3", "[token]" ) {
  FileTokenizer ft;
  bool ret = ft.read("data/test2.cpp");
  REQUIRE(ret);
  const std::vector<Token> &tokens = ft.getTokens();
  int i;
  for(i=0;i<(int)tokens.size();i++)
    if(tokens[i].getText() == "skipNewLinesAndComments")
      break;
  REQUIRE(i<(int)tokens.size());
  const Token &token = tokens[i];
  pair<int,int> lc = ft.getLineAndColumn(token);
  REQUIRE(lc.first == 517);
  REQUIRE(lc.second == 7);

}

