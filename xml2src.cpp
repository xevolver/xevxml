#include "xevxml.hpp"
#include "xml2ast.hpp"

using namespace std;

int main(int argc, char** argv)
{
  ofstream ofs(argv[1]);
  SgProject* sageProject=frontend(argc,argv);
  //SgProject* sageProject=frontend(argc,argv);
  char c;
  /*
  if( argc < 2 ) {
    cerr << "USAGE:" << argv[0] << " [output file name] " << endl;
    return -1;
  }
  */
  string fn(argv[1]);
  stringstream istr;

  while((c=cin.get()) != cin.eof()){
    istr << c;
  }
  //cerr << fn << ":" << sageProject->get_Fortran_only() << endl;
  xevxml::XmlInitialize();
  
  SgFile* file = xevxml::Xml2Ast(istr,sageProject,fn);
  //AstPostProcessing(file);

  //cerr << "Writing to " << file->getFileName() << endl;
  //file->unparse();
  //std::cout << xmlString1.str();
  ofs << file->unparseToCompleteString()  << endl;
  xevxml::XmlFinalize();
  //cerr << "Creating " << sageProject->get_file(sageProject->numberOfFiles()-1).getFileName() << endl;
  //sageProject->get_file(sageProject->numberOfFiles()-1).unparse();
  //sageProject->unparse();
  return 0;
}
