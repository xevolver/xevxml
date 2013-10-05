#include "xevxml.hpp"
#include "ast2xml.hpp"

int main(int argc, char** argv)
{
  SgProject* sageProject = frontend(argc, argv);
  /*
  if(sageProject->numberOfFiles() != 1) {
    std::cerr << "ERROR: multiple files are given." << std::endl;
    return 0;
  }
  */
  std::stringstream xmlString1;

  xevxml::XmlInitialize();
  //printf( "FILE NUMBER = %d\n", sageProject->numberOfFiles() );

  xevxml::Ast2Xml(xmlString1,sageProject,sageProject->numberOfFiles()-1);

  std::cout << xmlString1.str();

  xevxml::XmlFinalize();
  return 0;
}
