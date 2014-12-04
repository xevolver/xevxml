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
#include "xml2rose.hpp"

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;
using namespace XevXml;

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

/// Visitor of a PreprocessingInfo element in an XML document
SgNode*
XevXmlVisitor::visitPreprocessingInfo(xe::DOMNode* node, SgNode* astParent)
{
  char* buf = xe::XMLString::transcode(node->getNodeName());
  string nname = buf;
  xe::XMLString::release(&buf);

  if(nname!="PreprocessingInfo") return 0;
  if(astParent==0 || isSgLocatedNode(astParent)==0) return 0;;
  std::string pos;
  //std::string typ;
  XmlGetAttributeValue(node,"pos", &pos);
  //XmlGetAttributeValue(node,"type",&typ);

  if(pos.size() /*&& typ.size()*/ ){
    int pval;
    std::string content = "";
    if(node->getFirstChild()){
      buf = xe::XMLString::transcode(node->getFirstChild()->getNodeValue());
      content = buf;
      xe::XMLString::release(&buf);
    }
    //cerr << "#####################" << content;
    pval = atoi(pos.c_str());
    //tval = atoi(typ.c_str());
    content = XevXml::XmlEntity2Str(content);

    si::attachArbitraryText(isSgLocatedNode(astParent),content,
                                       (PreprocessingInfo::RelativePositionType)pval);

  }
  else XEV_ABORT();

  return 0;
}
