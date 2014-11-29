#include "common.hpp"
#include "rose2xml.hpp"
#include "xml2rose.hpp"

#include <vector>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
  SgProject*      sageProject=0;
  vector<string> args;

  for(int id(0);id<argc;++id)
    args.push_back( string(argv[id]) );
  args.push_back( string("-rose:skip_syntax_check"));
  args.push_back( string("-rose:skipfinalCompileStep"));
  sageProject = frontend(args); // build an ROSE AST from a code

  AstTests::runAllTests(sageProject);
  XevXML::PrintSymTable symtbl;
  symtbl.visit(&sageProject->get_file(0));

  //XevXML::CheckUndeclVars checker;
  //checker.visit(&sageProject->get_file(0));

  //return backend(sageProject);
  return 0;
}
