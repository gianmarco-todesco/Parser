#include "parser.h"
#include "grammarbuilder.h"
#include "catch.hpp"
#include <sstream>
#include <iostream>

using namespace std;

class MyCGrammarProvider {
  Grammar *m_grammar;
  bool m_valid;
public:
  MyCGrammarProvider() : m_grammar(0), m_valid(true) {}
  ~MyCGrammarProvider() { delete m_grammar; m_grammar = 0; }
  Grammar *getGrammar() {
    if(m_grammar || !m_valid) return m_grammar;
    m_valid = false;
    FileTokenizer ft;
    if(ft.read("data/cgrammar.txt"))
    {
      GrammarBuilder gb;
      m_grammar = gb.build(&ft);
      m_valid = m_grammar != 0;
    }
    return m_grammar;
  }
};

class MyCParser : public Parser {
public:
  MyCParser(MyCGrammarProvider &gp) : Parser(gp.getGrammar()) {
    setSkipNewLines(true);
  }
  ~MyCParser() {
  }

private:
};

TEST_CASE( "CParser1", "[parser2]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  SECTION("body")
  {
    MyCGrammarProvider gp;
    MyCParser parser(gp);
    //StringTokenizer st("char buffer[100];");
    StringTokenizer st("#define CHECK_EXIT(ptr) \\\ndo {		        \\\n    check_exit(ptr);    \\\n  } while (0)\n");

    // parser.traceOn();
    bool ret = parser.parse(&st);
    REQUIRE(ret);
    // REQUIRE(parser.getParseTree()->toString() == "");
  }
  REQUIRE(Symbol::getTotalCount() == 0);
}



TEST_CASE( "CParser2", "[parser2]") {
  REQUIRE(Symbol::getTotalCount() == 0);
  SECTION("body")
  {
    MyCGrammarProvider gp;
    MyCParser parser(gp);

    FileTokenizer ft;
    ft.read("data/test1.c");
    
    
    bool ret = parser.parse(&ft);
    REQUIRE(ret);
    // REQUIRE(parser.getParseTree()->toString() == "");
    ParseNode top = parser.getParseTree()->getNode(0);
    for(int i=0;i<top.getChildCount();i++)
    {
      ParseNode stat = top.getChild(i);
      // cout << stat.getTag() << " - " << stat.getText() << endl;
      // REQUIRE(stat.getTag() == "Stat");
    }

  }
  REQUIRE(Symbol::getTotalCount() == 0);
}
