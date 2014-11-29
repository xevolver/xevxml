/**
 * @file     rose2xml.hpp
 * @brief    ROSE Sage III AST is converted to an XML document.
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

#ifndef ___ROSE2XML_HPP___
#define ___ROSE2XML_HPP___
#include "common.hpp"
#include <xevxml.hpp>
#include <xmlutils.hpp>

namespace XevXml {

#define XEV_PRAGMA_PREFIX "!$xev"
#define XEV_ENCODE "XEV_ENCODE"

class XevSageVisitorOptions
{
  /* string stream for writing XML data */
  std::ostream& sstr_;

  int  lang_;    // output language
  bool faddr_;   // print address attribute
  bool fparen_;  // remove compiler-generated parentheses
  bool fpragma_; // analyze Fortran pragmas
  int  depth_;
  SgFile* file_;

  void init() {
    lang_    = 0;
    faddr_   = false;
    fparen_  = false;
    fpragma_ = false;
    depth_   = 0;
    file_    = NULL;
  }
public:
  XevSageVisitorOptions(std::ostream& s):sstr_(s) {init();}
  ~XevSageVisitorOptions() {}

  std::ostream& getStream(void) {
    return sstr_;
  }

  int getIndent() {
    return depth_;
  }

  int setIndent(int i) {
    depth_=i;
    return depth_;
  }

  int writeIndent(){
    for(int i(0);i<depth_;i++)
      sstr_ << ' ';
    return depth_;
  }

  SgFile* getSgFile() {return file_;}
  void setSgFile(SgFile* f) {file_=f;}
};

class XevSageVisitor {
  XevSageVisitorOptions* opt_;

protected:
  std::ostream& sstr() {
    return opt_->getStream();
  }

public:
  void unsetStream(void){
    if(opt_!=NULL)
      delete opt_;
    opt_=NULL;
  }
  void setStream(std::ostream& s){
    unsetStream();
    opt_=new XevSageVisitorOptions(s);
  }
  std::ostream& getStream() {
    return this->sstr();
  }

  int getIndent()      { return opt_->getIndent()  ;}
  int setIndent(int i) { return opt_->setIndent(i) ;}
  int writeIndent()    { return opt_->writeIndent();}

  XevSageVisitor (void):opt_(NULL){}
  XevSageVisitor (std::ostream& s) {
    opt_ = NULL;
    setStream(s);
  }
  ~XevSageVisitor() {
  }

  bool hasInode(SgNode* node );

  SgFile* getSgFileToVisit() {return opt_->getSgFile();}
  void  setSgFileToVisit(SgFile* f){opt_->setSgFile(f);}

  void visit(SgNode* node);
#define SAGE3(op)                                       \
  void attribSg##op(SgNode* node);                      \
  void inodeSg##op(SgNode* node);
#include "sgnode.hpp"
};

} // XevXml
#endif
