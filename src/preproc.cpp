/**
 * @file     preproc.cpp
 * @brief    Preprocessing Information Hanlders
 * \license This project is released under the BSD 2-clause license
 *
 * Copyright (C) 2010-2013 Hiroyuki TAKIZAWA. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *    
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "common.hpp"
#include "xml2ast.hpp"

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;
using namespace XevXML;

void 
XevXmlVisitor::checkPreprocInfo(xe::DOMNode* node, SgNode* astNode)
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
XevXmlVisitor::visitPreprocessingInfo(xe::DOMNode* node, SgNode* astParent)
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
    std::string content = "";
    if(node->getFirstChild())
      content = xe::XMLString::transcode(node->getFirstChild()->getNodeValue());
    //cerr << "#####################" << content;
    pos >> pval;
    typ >> tval;
    content = XevXML::XmlEntity2Str(content);
    si::attachArbitraryText(isSgLocatedNode(astParent),content,
				       (PreprocessingInfo::RelativePositionType)pval);
  }
  else ABORT();
  
  return 0;
}
