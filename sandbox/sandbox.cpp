#include "../include/parser.h"
#include "../include/grammarbuilder.h"

#include <iostream>

using namespace std;


int main2()
{
  StringTokenizer st(
      "E->T\n"
      "E->E '+' T\n"
      "T->F\n"
      "T->T '*' F\n"
      "F->V\n"
      "F->'(' E ')'\n"
      "V->'x'\n"
      "V->'y'\n"
      "V->'z'\n"
      "Stat->V '=' E\n");

  GrammarDefinitionParser p;
  p.parse(&st);

  const ParseTree *ptree = p.getParseTree();
  for(int i=0; i<ptree->getNode(0).getChildCount();i++)
  {
    ParseNode node = ptree->getNode(0).getChild(i);
    cout << " - " << node.getTag().c_str() << endl;

  }
  ptree->dump(cout);
  
  return 0;
}

int main()
{
  GrammarBuilder gb;

  StringTokenizer st(
    "StmLst -> : pass(1)\n"
    "StmLst -> StmLst Stm : pass(1,2)\n"
    "Stm -> type vLst ';': decl(1,2)\n"
    "type -> 'int' : int\n"
    "type -> 'char' : char\n"
    "vLst -> var : pass(1)\n"
    "vLst -> vLst ',' var : pass(1,3)\n"
    "var -> ident : var(1)\n"
    "var -> '*' var : star(2)\n"
    "var -> var '[' ident ']': braket(1,3)\n"
  );

  Grammar *g = gb.build(&st);

  TerminalSymbol *eof = g->addTerminal(
      new TokenTypeTerminalSymbol("EOF", Token::T_Eof));
  RuleBuilder(g,"S").n("StmLst").t(eof).end();


  //g->dump(cout);

  Parser parser(g, "S");
  //parser.getParseTable().dump();

  int m = 500000;

  char *buffer = new char[m+1000];
  char *unit = "int x; char *s, buffer[size];";
  int i = 0;
  while(i + strlen(unit) < m) { strcpy( buffer+i, unit); i += strlen(unit); }

  cout << "Start" << endl;

  // StringTokenizer text("int x; char *s, buffer[size];");
  StringTokenizer text(buffer);
  bool ret = parser.parse(&text);
  if(ret)
  {
    cout << "Parsed ok " << endl;
    const ParseTree *parseTree = parser.getParseTree();
    cout << "parse tree ok " << endl;
    // parseTree->dump(cout);
    cout << parseTree->getNode(0).getChildCount() << endl;

  }

  

  return 0;
}
