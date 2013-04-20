#ifndef ___XML2AST_H___
#define ___XML2AST_H___

#include "xevxml.hpp"

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMLSInput.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

class Xml2AstVisitor
{
  SgSourceFile*      _file;
public:
  Xml2AstVisitor(const std::string& ifn, 
		 const std::string& ofn, 
		 SgProject* prj);
  ~Xml2AstVisitor();

  void visit2(xercesc::DOMNode* node,int depth=0)
  {
    if(node) {
      xercesc::DOMNode* child=node->getFirstChild();
      while(child){
	std::cerr<< depth 
		 << xercesc::XMLString::transcode(child->getNodeName()) 
		 << std::endl;
	this->visit2(child,depth+1);
	xercesc::DOMNode* next=child->getNextSibling();
	child=next;
      }
    }
  }

  SgNode* visit(xercesc::DOMNode* node, SgNode* astParent=0);


  SgNode* visitSgSourceFile(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgGlobal(xercesc::DOMNode* node, SgNode* astParent=0);


  SgNode* visitSgPragmaDeclaration(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgPragma(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgVariableDeclaration(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFunctionDeclaration(xercesc::DOMNode* node, SgNode* astParent=0);


  SgNode* visitSgFunctionParameterList(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgBasicBlock(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgExprStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgForStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgForInitStatement(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgPlusPlusOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgPlusAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgAddOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLessThanOp(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgVarRefExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgCastExp(xercesc::DOMNode* node, SgNode* astParent=0);


  SgNode* visitSgReturnStmt(xercesc::DOMNode* node, SgNode* astParent=0);

  /* NOTE: this function returns SgBasicBlock */
  SgNode* visitSgFunctionDefinition(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgInitializedName(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgAssignInitializer(xercesc::DOMNode* node, SgNode* astParent=0);

  /* --- values --- */
  SgNode* visitSgBoolValExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgCharVal(xercesc::DOMNode* node, SgNode* astParent=0);
  //SgNode* visitSgWcharVal(xercesc::DOMNode* node, SgNode* astParent=0);
  //SgNode* visitSgComplexVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDoubleVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFloatVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgIntVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLongIntVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLongLongIntVal(xercesc::DOMNode* node, SgNode* astParent=0);
  //SgNode* visitSgEnumVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLongDoubleVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgShortVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgStringVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgUnsignedCharVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgUnsignedShortVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgUnsignedIntVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgUnsignedLongVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgUnsignedLongLongIntVal(xercesc::DOMNode* node, SgNode* astParent=0);


  SgType* buildModifierType(xercesc::DOMNode* node, SgType* itype);

  SgType* buildType(xercesc::DOMNode* node, SgExpression* ex=0);
  
  SgNode* visitSgTypeInt(xercesc::DOMNode* node, SgNode* astParent=0)
  {
    return buildType(node);
  }

  SgNode* visitSgTypeFloat(xercesc::DOMNode* node, SgNode* astParent=0)
  {
    return buildType(node);
  }

  SgNode* visitSgTypeDouble(xercesc::DOMNode* node, SgNode* astParent=0)
  {
    return buildType(node);
  }

  SgNode* visitSgModifierType(xercesc::DOMNode* node, SgNode* astParent=0)
  {
    return buildType(node);
  }

  SgNode* visitSgPointerType(xercesc::DOMNode* node, SgNode* astParent=0)
  {
    return buildType(node);
  }

  void checkPreprocInfo(xercesc::DOMNode* node, SgNode* astNode);
  SgNode* visitPreprocessingInfo(xercesc::DOMNode* node, SgNode* astParent=0);


  SgFile* getSgFile() {return _file;}
};

extern void Xml2Ast(std::stringstream& str, 
		    SgProject* prj, 
		    std::string ofn="output.c");

#endif
