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
    "E -> ident ident : foo(1,2)     \n"
  );

  Grammar *g = gb.build(&st);

  g->dump(cout);

  return 0;
}
