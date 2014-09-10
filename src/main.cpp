#include "common.hpp"
#include "ast2xml.hpp"
#include "xml2ast.hpp"
#include "xslt.hpp"

int main(int argc, char** argv)
{
  // AST generation
  SgProject* sageProject = frontend(argc, argv);
  if(sageProject->numberOfFiles() != 1) {
    std::cerr << "ERROR: multiple files are given." << std::endl;
    return 0;
  }

  std::stringstream xmlString1,xmlString2,xmlString3;

  XmlInitialize();

  Ast2Xml(xmlString1,sageProject,0);

  std::cerr << "\n--- original -----------------------------------" << std::endl;
  std::cerr << xmlString1.str();

  XsltTransform(xmlString1,xmlString2,"example.xsl");
  std::cerr << "\n--- translated ---------------------------------" << std::endl;
  std::cerr << xmlString2.str();

  Xml2Ast(xmlString2,sageProject,"hoge.c");
  Ast2Xml(xmlString3,sageProject,1);

  std::cerr << "\n--- rebuilt ------------------------------------" << std::endl;
  std::cerr << xmlString3.str();

  sageProject->unparse();

  XmlFinalize();
  return 0;
}
