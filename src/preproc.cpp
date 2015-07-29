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
#include "rose2xml.hpp"

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;
using namespace XevXml;

/* this function is borrowed from unparseCxx_expressions.C in ROSE */
static bool removeIncludeDirectives(SgLocatedNode* loc)
{
  bool ret = false;
  AttachedPreprocessingInfoType* info = loc->getAttachedPreprocessingInfo();
  AttachedPreprocessingInfoType dirs;
  AttachedPreprocessingInfoType::iterator i;
  if(info==NULL) return ret;
  PreprocessingInfo::DirectiveType removedDirectiveType
    = PreprocessingInfo::CpreprocessorIncludeDeclaration;
  /* find a directive contained in a construct */
  for(i=info->begin();i!=info->end();++i){
    if(loc->get_startOfConstruct()->isSameFile((*i)->get_file_info())
       && loc->get_endOfConstruct()->isSameFile((*i)->get_file_info())
       && *(loc->get_startOfConstruct()) <= *((*i)->get_file_info())
       && *(loc->get_endOfConstruct()) >= *((*i)->get_file_info())){
      if((*i)->getTypeOfDirective()==removedDirectiveType) {
        dirs.push_back(*i);
      }
    }
  }

  /* remove directories */
  for(i=dirs.begin();i!=dirs.end();++i){
    info->erase(find(info->begin(),info->end(),*i));
  }
  return ret;
}

namespace XevXml {
/*
 * find a prefix (!$) in the Fortran comment and create SgPragmaDeclaration.
 */
void
writeFortranPragma(std::ostream& sstr_, SgNode* node,
                   PreprocessingInfo::RelativePositionType pos)
{
  if( SageInterface::is_Fortran_language()==false) return;

  SgLocatedNode* loc =isSgLocatedNode(node);
  if(loc==NULL) return;

  AttachedPreprocessingInfoType* info = loc->getAttachedPreprocessingInfo();
  if(info==NULL) return;

  std::string str;
  int idx;

  if(info){
    for(size_t i(0);i<(*info).size();i++) {
      if((*info)[i]->getRelativePosition()==pos){
        str = (*info)[i]->getString();
        std::transform(str.begin(),str.end(),str.begin(),::tolower);
        idx = str.find( XEV_PRAGMA_PREFIX );
        if( idx >= 0 ) {
          str = (*info)[i]->getString(); // read the string again
          sstr_ << "<SgPragmaDeclaration >\n";
          sstr_ << "  "; // indent
          sstr_ << "<SgPragma pragma=\"";
          // assuming Fortran directives start with !$
          sstr_ << XevXml::XmlStr2Entity(str.substr( idx+strlen("!$") )) << "\" />\n";
          //sstr_ << XevXml::XmlStr2Entity(str.substr( idx+strlen("!$") )) << "\n";
          sstr_ << "</SgPragmaDeclaration >\n";
        }
      }
    }
  }
}

/*
 * writes Preprocessing Info of a SgNode as a text element in XML.
 */
AttachedPreprocessingInfoType*
writePreprocessingInfo(std::ostream& sstr_,SgNode* n)
{

  SgLocatedNode* loc = isSgLocatedNode(n);
  if(loc==NULL) return NULL;
  if(removeIncludeDirectives(loc)){
    XEV_INFO("Include directives contained within a construct are removed.");
  }

  AttachedPreprocessingInfoType* info = loc->getAttachedPreprocessingInfo();
  if(info==NULL) return NULL;

  std::string str;
  for(size_t i(0);i<(*info).size();i++) {
    str = (*info)[i]->getString();
    str = XmlStr2Entity( str );
    sstr_ << "<PreprocessingInfo pos=\"";
    sstr_ << (*info)[i]->getRelativePosition() <<"\" ";
    sstr_ << " type=\"";
    sstr_ << (*info)[i]->getTypeOfDirective() << "\">";
    sstr_ << str;
    //sstr_ << "\n";
      sstr_ << "</PreprocessingInfo>\n";
  }

  return info;
}

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
  if(astParent==0 || isSgLocatedNode(astParent)==0) {
    XEV_DEBUG_INFO(node);
    XEV_INFO("PreprocessingInfo ignored");
    return 0;;
  }
  //std::string pos;
  //std::string typ;
  int tval = 0;
  int pval = 0;
  SgLocatedNode* loc = isSgLocatedNode(astParent);

  if(loc==NULL){
    XEV_DEBUG_INFO(node);
    XEV_WARN("PreprocessingInfo ignored");
    XEV_WARN("its parent is not a kind of SgLoatedNode");
    return 0;
  }
  if(XmlGetAttributeValue(node,"pos", &pval)==false)
    XEV_MISSING_ATTR(PreprocessingInfo,pos,true);
  if(XmlGetAttributeValue(node,"type",&tval)==false)
    XEV_MISSING_ATTR(PreprocessingInfo,type,true);

  PreprocessingInfo::DirectiveType dtype
    = (PreprocessingInfo::DirectiveType)tval;
  PreprocessingInfo::RelativePositionType pos
    =(PreprocessingInfo::RelativePositionType)pval;
  std::string content = "";
  if(node->getFirstChild()){
    buf = xe::XMLString::transcode(node->getFirstChild()->getNodeValue());
    content = buf;
    xe::XMLString::release(&buf);
  }
  content = XevXml::XmlEntity2Str(content);
  //cerr << "-----------------------------------------\n";
  //cerr << content;
  //cerr << "-----------------------------------------\n";
  //cerr << pos << ":" << astParent->class_name() << endl;

  PreprocessingInfo* info
    = new PreprocessingInfo(dtype,content,"transformation",0,0,0,pos);
  XEV_ASSERT(info!=NULL);
  info->set_file_info(loc->get_file_info());
  loc->addToAttachedPreprocessingInfo(info);
  //PreprocessingInfo* info
  //= si::attachArbitraryText(loc,content,
  //                          (PreprocessingInfo::RelativePositionType)pval);
  //cerr << "-----------------------------------------\n";
  //si::dumpPreprocInfo(loc);
  //cerr << "-----------------------------------------\n";
  //info->get_file_info()->display();
  //loc->get_file_info()->display();

  return 0;
}

} //namespace XevXml
