#include "../include/parser.h"
#include "../include/grammarbuilder.h"

#include <iostream>

using namespace std;

#include <Windows.h>

LARGE_INTEGER frequency;        // ticks per second
LARGE_INTEGER t1, t2;           // ticks

void start()
{
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&t1);
}

void end()
{
  double elapsedTime;
  QueryPerformanceCounter(&t2);

  // compute and print the elapsed time in millisec
  elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
  cout << "Elapsed time = " << elapsedTime << endl;
}


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

int main3()
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

  const TerminalSymbol *eof = g->addTerminal(
      new TokenTypeTerminalSymbol("EOF", Token::T_Eof));
  RuleBuilder(g,"S").n("StmLst").t(eof).end();


  //g->dump(cout);

  Parser parser(g);
  //parser.getParseTable().dump();

  int m = 500000;

  char *buffer = new char[m+1000];
  char *unit = "int x; char *s, buffer[size];";
  int i = 0;
  while((int)(i + strlen(unit)) < m) { strcpy_s( buffer+i, m-i, unit); i += strlen(unit); }

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



int main4()
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
    "var -> var '[' arraysize ']': braket(1,3)\n"
    "arraysize -> ident : pass(1)\n"
    "arraysize -> int : pass(1)\n"
  );

  Grammar *g = gb.build(&st);

  const TerminalSymbol *eof = g->addTerminal(
      new TokenTypeTerminalSymbol("EOF", Token::T_Eof));
  RuleBuilder(g,"S").n("StmLst").t(eof).end();


  //g->dump(cout);

  Parser parser(g);
  //parser.getParseTable().dump();
  StringTokenizer text("int a,*b,c[10],*d[10]; int x,y,z;");
  bool ret = parser.parse(&text);
  if(ret)
  {
    cout << "Parsed ok " << endl;
    const ParseTree *parseTree = parser.getParseTree();
    cout << "parse tree ok " << endl;
    parseTree->dump(cout);
    // cout << parseTree->getNode(0).getChildCount() << endl;

  }
  return 0;
}

int main5()
{
  GrammarBuilder gb;

  StringTokenizer st(
    "A -> ident\n"
  );

  Grammar *g = gb.build(&st);
  g->dump(cout);

  Parser parser(g);
  //parser.getParseTable().dump();
  StringTokenizer text("ciccio");
  bool ret = parser.parse(&text);
  if(ret)
  {
    cout << "Parsed ok " << endl;
    const ParseTree *parseTree = parser.getParseTree();
    cout << "parse tree ok " << endl;
    parseTree->dump(cout);
    // cout << parseTree->getNode(0).getChildCount() << endl;

  }

  

  return 0;
}



int main6()
{
  GrammarBuilder gb;

  StringTokenizer st(
    "Lst -> Item\n"
    "Lst -> Lst ',' Item : Lst(1,3)\n"
    "Item -> Var : pass(1)\n"
    "Var -> 'x' : pass(1)\n"
    "Var -> 'y' : pass(1)\n"
    "Var -> 'z' : pass(1)\n"

  );

  Grammar *g = gb.build(&st);
  g->dump(cout);

  Parser parser(g);
  //parser.getParseTable().dump();
  StringTokenizer text("x");
  bool ret = parser.parse(&text);
  if(ret)
  {
    cout << "Parsed ok " << endl;
    const ParseTree *parseTree = parser.getParseTree();
    cout << "parse tree ok " << endl;
    parseTree->dump(cout);
    // cout << parseTree->getNode(0).getChildCount() << endl;

  }

  

  return 0;
}


int main7()
{
  cout << "Building grammar ..." << endl;

  GrammarBuilder gb;

  StringTokenizer st(
    "StmLst -> \n"
    "StmLst -> StmLst Stm\n"
    "Stm -> Declaration ';' : pass(1)\n"
    "Declaration -> Type DeclaratorLst\n"
    "Type -> 'void'\n"
    "Type -> 'char'\n"
    "Type -> 'short'\n"
    "Type -> 'int'\n"
    "Type -> 'long'\n"
    "Type -> 'float'\n"
    "Type -> 'double'\n"
    "Type -> 'signed'\n"
    "Type -> 'unsigned'\n"
    "DeclaratorLst -> Declarator\n"
    "DeclaratorLst -> DeclaratorLst ',' Declarator : DeclaratorLst(1,3)\n"
    "Declarator -> DirectDeclarator : pass(1)\n"
    "Declarator -> '*' Declarator : Pointer(2)\n"
    "DirectDeclarator -> ident : Name(1)\n"
    "DirectDeclarator -> '(' Declarator ')' : pass(2)\n"
    "DirectDeclarator -> DirectDeclarator '[' ArraySize ']' : Array(1,3)\n"
    "DirectDeclarator -> DirectDeclarator '[' ']' : Array(1)\n"
    "DirectDeclarator -> DirectDeclarator '(' ')' : Function(1)\n"
    "DirectDeclarator -> DirectDeclarator '(' ParamTypeLst ')' : Function(1,3)\n"
    "ArraySize -> int : pass(1)\n"
    "ArraySize -> ident : pass(1)\n"
    "ParamTypeLst -> ParamDecl \n"
    "ParamTypeLst -> ParamTypeLst ',' ParamDecl : ParamTypeLst(1,3)\n"
    "ParamDecl -> ident :pass(1) \n"    
  );

  Grammar *g = gb.build(&st);
  cout << "Done" << endl;

  g->dump(cout);

  Parser parser(g);

  //parser.getParseTable().dump();

  cout << "Building text..." << endl;
  int m = 5000;
  char *temp = "int a,*b,c[10],*d[DIM]; char *buffer[1024]; float **p; int (*pf)(int); ";
  int tempLength = strlen(temp) ;
  char *buffer = (char*)malloc(tempLength * m + 100);
  for(int i=0;i<m;i++)
  {
    strcpy(buffer + i*tempLength, temp);
  }

  // StringTokenizer text("int a,*b,c[10],*d[DIM]; char *buffer[1024]; float **p; int (*pf)(int); ");
  StringTokenizer text(buffer);
  cout << "Started parsing...." << endl;
  start();
  bool ret = parser.parse(&text);
  end();
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



int main()
{
  GrammarBuilder gb;

  StringTokenizer st(
    "StmLst -> \n"
    "StmLst -> StmLst Stm\n"
    "Stm -> 'int' ident '(' ')' Block : Function(1,2,5)\n"
    "Block -> '{' BlockContent '}' : null\n"
    "BlockContent -> : null\n"
    "BlockContent -> BlockContent Block : null\n"
    "BlockContent -> BlockContent any : null\n"
  );

  Grammar *g = gb.build(&st);
  g->dump(cout);

  Parser parser(g);
  parser.setSkipNewLines(true);

  //parser.getParseTable().dump();
  StringTokenizer text(
    "int pippo()\n"
    "{\n"
    "  for(int j=0; j<10; j++)\n"
    "  {\n"
    "    uff++;\n"
    "  }\n"
    "}\n"
    "int pluto()\n"
    "{\n"
    "}\n");
  bool ret = parser.parse(&text);
  if(ret)
  {
    cout << "Parsed ok " << endl;
    const ParseTree *parseTree = parser.getParseTree();
    cout << "parse tree ok " << endl;
    parseTree->dump(cout);
    // cout << parseTree->getNode(0).getChildCount() << endl;

  }

  

  return 0;
}

