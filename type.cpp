#include "xevxml.hpp"
#include "xml2ast.hpp"

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;

SgType* 
Xml2AstVisitor::buildModifierType(xe::DOMNode* node, SgType* itype)
{
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* typatt = 0;
  string modtype;
  
  if(amap) {
    typatt=amap->getNamedItem(xe::XMLString::transcode("modifier"));
    if(typatt)
      modtype = xe::XMLString::transcode(typatt->getNodeValue());
    else 
      ABORT();
  }
  if( modtype == "const" )
    return sb::buildConstType(itype);
  else if ( modtype == "volatile" )
    return sb::buildVolatileType(itype);
  else if ( modtype == "restrict" )
    return sb::buildRestrictType(itype);
  else return 0;
}

SgType* 
Xml2AstVisitor::buildType(xe::DOMNode* node, SgExpression* ex)
{
  SgType* itype = 0;
  SgType* ret = 0;
  if(node==0) return ret;
  
  // reverse order
  xe::DOMNode* child=node->getLastChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      if(itype==0)
	itype = buildType(child,ex);
    }
    child=child->getPreviousSibling();
  } 
  
  string name = xe::XMLString::transcode(node->getNodeName());
  
  
  if(name=="SgTypeBool")
    ret = sb::buildBoolType();
  else if(name=="SgTypeChar")
    ret = sb::buildCharType();
  else if(name=="SgTypeDouble")
    ret = sb::buildDoubleType();
  else if(name=="SgTypeFloat")
    ret = sb::buildFloatType();
  else if(name=="SgTypeInt")
    ret = sb::buildIntType();
  else if(name=="SgTypeLong")
    ret = sb::buildLongType();
  else if(name=="SgTypeLongLong")
    ret = sb::buildLongLongType();
  else if(name=="SgTypeShort")
    ret = sb::buildShortType();
  else if(name=="SgTypeString"){
    if(ex) 
      ret = sb::buildStringType(ex);
    else
      ret = sb::buildStringType();
  }
  else if(name=="SgTypeVoid")
    ret = sb::buildVoidType();
  else if(name=="SgTypeWchar")
    ret = sb::buildWcharType();
  else if(name=="SgTypeSignedChar")
    ret = sb::buildSignedCharType();
  else if(name=="SgTypeSignedInt")
    ret = sb::buildSignedIntType();
  else if(name=="SgTypeSignedLong")
    ret = sb::buildSignedLongType();
  else if(name=="SgTypeSignedLongLong")
    ret = sb::buildSignedLongLongType();
  else if(name=="SgTypeSignedShort")
    ret = sb::buildSignedShortType();
  else if(name=="SgTypeUnsignedChar")
    ret = sb::buildSignedCharType();
  else if(name=="SgTypeUnsignedInt")
    ret = sb::buildSignedIntType();
  else if(name=="SgTypeUnsignedLong")
    ret = sb::buildSignedLongType();
  else if(name=="SgTypeUnsignedLongLong")
    ret = sb::buildSignedLongLongType();
  else if(name=="SgTypeUnsignedShort")
    ret = sb::buildSignedShortType();
  else if(name=="SgTypeUnknown")
    ret = sb::buildUnknownType();
  else if(name=="SgPointerType") {
    if(itype)
      ret = sb::buildPointerType(itype);
    else ABORT();
  }
  else if(name=="SgReferenceType") {
    if(itype)
      ret = sb::buildReferenceType(itype);
    else ABORT();
  }
  else if (name=="SgModifierType") {
    if(itype)
      ret = buildModifierType(node,itype);
    else ABORT();
  }
  
  return ret;
}
