/**
 * @file     ast2xml.hpp
 * @brief    Classes for converting ROSE Sage III AST to an XML document.
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
#ifndef ___AST2XML_HPP___
#define ___AST2XML_HPP___

//#define XEV_USE_ROSEHPCT // experimental
#include <rose.h>
#ifdef XEV_USE_ROSEHPCT
#include <rosehpct/rosehpct.hh>
#endif

#define XEV_PRAGMA_PREFIX "!$xev"

namespace xevxml {
class Ast2XmlOpt {
public:
  bool address;
  bool rosehpct;
#ifdef XEV_USE_ROSEHPCT
  RoseHPCT::ProgramTreeList_t profiles;
#endif
  Ast2XmlOpt() {address=false;rosehpct=false;}
  ~Ast2XmlOpt() {}
};

class Ast2XmlInheritedAttribute
{
public:
  int level;
  Ast2XmlOpt* opt;
  Ast2XmlInheritedAttribute (Ast2XmlOpt* o):level(0),opt(o){};
};

class Ast2XmlVisitorInternal: public AstTopDownProcessing <Ast2XmlInheritedAttribute>
{
protected:
  std::stringstream& sstr_;
  int outLang_;

  Ast2XmlInheritedAttribute 
  evaluateInheritedAttribute(SgNode* node,
			     Ast2XmlInheritedAttribute att);
  
  void 
  destroyInheritedValue (SgNode* node,
			 Ast2XmlInheritedAttribute att);
  
public:
  Ast2XmlVisitorInternal(std::stringstream& s): sstr_(s),outLang_(0) {}
  ~Ast2XmlVisitorInternal() {}
};

class Ast2XmlVisitor:public Ast2XmlVisitorInternal
{
public:
  Ast2XmlVisitor(std::stringstream& s): Ast2XmlVisitorInternal(s){
    sstr_ << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  }
  ~Ast2XmlVisitor() {}
protected:
  void atTraversalStart() {/* do nothing */}
  void atTraversalEnd() 
  {
    sstr_<<std::endl<<std::ends;
  }
};


extern void Ast2Xml(std::stringstream& str, SgFile* file, Ast2XmlOpt* opt = NULL);
}


#endif


