#include "rose.h"
#include "DefUseAnalysis.h"
#include <string>
#include <iostream>
#include <xevxml.hpp>

using namespace std;

class DefuseUser:public XevXml::XevSageVisitor
{
  DFAnalysis* defuse;
public:
  DefuseUser():XevSageVisitor() {
    defuse = NULL;
  }

  bool defuseAnalysis(SgProject* p){
    // Call the Def-Use Analysis
    bool debug = false;
    defuse = new DefUseAnalysis(p);
    defuse->run(debug);
    return true;
  }

  /* add a new attribute to SgVarRefExp nodes */
  void attribSgVarRefExp(SgNode* node){
    /* write default attributes */
    XevSageVisitor::attribSgVarRefExp(node);

    /* write an additional attribute */
    SgVarRefExp * varRef = isSgVarRefExp(node);
    SgInitializedName* initName
      = isSgInitializedName(varRef->get_symbol()->get_declaration());
    vector<SgNode* > vec = defuse->getDefFor(varRef, initName);
    sstr() << " num_defs=\"" << vec.size() <<"\" ";
  }
};

int main( int argc, char * argv[] )
{
  vector<string> argvList(argv, argv + argc);
  SgProject* project = frontend(argvList);

  XevXml::XevInitialize();

  /* setup an AST visitor class object */
  DefuseUser visitor;
  XevXml::XevXmlOption opt;
  visitor.setXmlOption(&opt);

  /* do Def-Use analysis */
  visitor.defuseAnalysis(project);

  /* visit every node and write it as an XML element */
  visitor.write(cout,&project);

  XevXml::XevFinalize();
  return 0;
}
