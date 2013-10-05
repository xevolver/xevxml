#include "xevxml.hpp"
#include "xml2ast.hpp"

using namespace std;

int main(int argc, char** argv)
{
  SgProject* sageProject=0;
  char c;
  if( argc < 2 ) {
    cerr << "USAGE:" << argv[0] << " [output file name] " << endl;
    return -1;
  }
  string fn(argv[1]);
  stringstream istr;

  while((c=cin.get()) != cin.eof()){
    istr << c;
  }

  xevxml::XmlInitialize();

  SgFile* file = xevxml::Xml2Ast(istr,sageProject,fn);
  file->unparse();
  //std::cout << xmlString1.str();

  xevxml::XmlFinalize();
  //sageProject->get_file(sageProject->numberOfFiles()-1).unparse();
  //sageProject->unparse();
  return 0;
}
