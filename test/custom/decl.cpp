#include "rose.h"
#include "DefUseAnalysis.h"
#include <string>
#include <iostream>
#include <xevxml.hpp>

using namespace std;

class DeclWriter:public XevXml::XevSageVisitor
{
public:
  DeclWriter():XevSageVisitor() {
  }

  /* add a new attribute to SgVarRefExp nodes */
  void attribSgVarRefExp(SgNode* node){
    /* write default attributes */
    XevSageVisitor::attribSgVarRefExp(node);

    /* write an additional attribute */
    SgVarRefExp * varRef = isSgVarRefExp(node);
    SgInitializedName* initName
      = isSgInitializedName(varRef->get_symbol()->get_declaration());
    if(initName){
      sstr() << " decl=\"";
      sstr().setf(ios::hex,ios::basefield);
      sstr() << initName << "\"";
      sstr().unsetf(ios::hex);
    }
  }
  void attribSgInitializedName(SgNode* node){
    /* write default attributes */
    XevSageVisitor::attribSgInitializedName(node);

    /* write an additional attribute */
    SgInitializedName* initName
      = isSgInitializedName(node);
    if(initName){
      sstr() << " address=\"";
      sstr().setf(ios::hex,ios::basefield);
      sstr() << initName << "\"";
      sstr().unsetf(ios::hex);
    }
  }
};

int main( int argc, char * argv[] )
{
  vector<string> argvList(argv, argv + argc);
  SgProject* project = frontend(argvList);

  XevXml::XevInitialize();

  /* setup an AST visitor class object */
  DeclWriter visitor;
  XevXml::XevXmlOption opt;
  visitor.setXmlOption(&opt);

  /* visit every node and write it as an XML element */
  visitor.write(cout,&project);

  XevXml::XevFinalize();
  return 0;
}
