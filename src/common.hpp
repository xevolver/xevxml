/**
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
#ifndef ___COMMON_HPP___
#define ___COMMON_HPP___
#include <xevxml.hpp>

//#define XEVXML_DEBUG

#ifdef XEVXML_DEBUG
#define XEV_ABORT()                      {                      \
    std::cerr << "ERROR @ " << __func__ << " :";                \
    std::cerr << __FILE__ ;                                     \
    std::cerr << "(" << __LINE__ << "): " << std::endl;         \
    abort();                                                    \
  }
#else
#define XEV_ABORT()                      {                      \
    std::cerr << "ERROR @ " << __func__ << " :";                \
    std::cerr << __FILE__ ;                                     \
    std::cerr << "(" << __LINE__ << "): " << std::endl;         \
    std::exit(1);                                               \
  }
#endif

#define XEV_WARN(x)                      {                              \
    std::cerr << "WARN @ " << __func__ << " :";                         \
    std::cerr << __FILE__ ;                                             \
    std::cerr << "(" << __LINE__ << "): "                               \
              << x << std::endl;                                        \
  }

#define XEV_ASSERT(x)            {if(!(x)) XEV_ABORT();}

#define XEV_DEBUG_INFO(x) {                                     \
    std::cerr << "XML Location Information:" << std::endl;      \
    std::cerr << XevXml::XmlGetNodePosition(x) << std::endl;    \
  }

#define DEFAULT_FILE_INFO (Sg_File_Info::generateDefaultFileInfoForTransformationNode())

#define SUBTREE_VISIT_BEGIN(X,Y,Z)                                      \
  {                                                                     \
  xercesc::DOMNode* cld_ = (X)->getFirstChild();                        \
  while(cld_) {                                                         \
  if(cld_->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){            \
  SgNode* Y = this->visit(cld_,Z);

#define SUBTREE_VISIT_END()     } cld_=cld_->getNextSibling();}}

#define FUNCTION_HEADER_VISIT_BEGIN(X,Y,Z)                              \
  {                                                                     \
  xercesc::DOMNode* cld_ = (X)->getFirstChild();                        \
  while(cld_) {                                                         \
  if(cld_->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){            \
  char* buf = xe::XMLString::transcode(cld_->getNodeName());            \
  string nname = buf;                                                   \
  xe::XMLString::release(&buf);                                         \
  if(nname != "SgFunctionDefinition" && nname != "SgBasicBlock"){       \
    SgNode* Y = this->visit(cld_,Z);

#define FUNCTION_HEADER_VISIT_END()     }} cld_=cld_->getNextSibling();}}

#define FUNCTION_BODY_VISIT_BEGIN(X,Y,Z)                                \
  {                                                                     \
  xercesc::DOMNode* cld_ = (X)->getFirstChild();                        \
  while(cld_) {                                                         \
  if(cld_->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){            \
  char* buf = xe::XMLString::transcode(cld_->getNodeName());            \
  string nname = buf;                                                   \
  xe::XMLString::release(&buf);                                         \
  if(nname == "SgFunctionDefinition" || nname == "SgBasicBlock"){       \
    SgNode* Y = this->visit(cld_,Z);

#define FUNCTION_BODY_VISIT_END()     }} cld_=cld_->getNextSibling();}}

// flag to check if the current node is within a compound literal
// - defined in type.cpp
// - used in inodeSgClassType and inodeCompundLiteralExp
extern bool g_withinCompoundLiteral;

#endif /* ___COMMON_HPP___ */
