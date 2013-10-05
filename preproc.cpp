#include "xevxml.hpp"
#include "xml2ast.hpp"

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;
using namespace xevxml;

void 
Xml2AstVisitor::checkPreprocInfo(xe::DOMNode* node, SgNode* astNode)
{
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      this->visitPreprocessingInfo(child,astNode);
    }
    child=child->getNextSibling();
  } 
}

SgNode* 
Xml2AstVisitor::visitPreprocessingInfo(xe::DOMNode* node, SgNode* astParent)
{
  std::string nname = xe::XMLString::transcode(node->getNodeName());
  if(nname != "PreprocessingInfo") return 0;
  if(astParent==0 || isSgLocatedNode(astParent)==0) return 0;
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* posatt = 0;
  std::stringstream pos,typ;
  if(amap) {
    posatt=amap->getNamedItem(xe::XMLString::transcode("pos"));
    if(posatt)
      pos << xe::XMLString::transcode(posatt->getNodeValue());
    posatt=amap->getNamedItem(xe::XMLString::transcode("type"));
    if(posatt)
      typ << xe::XMLString::transcode(posatt->getNodeValue());
  }
  else ABORT();
  
  if(pos.str().size() && typ.str().size()){
    int pval,tval;
    std::string content 
      = xe::XMLString::transcode(node->getFirstChild()->getNodeValue());
    pos >> pval;
    typ >> tval;
    si::attachArbitraryText(isSgLocatedNode(astParent),content,
				       (PreprocessingInfo::RelativePositionType)pval);
    
  }
  else ABORT();
  
  return 0;
}
