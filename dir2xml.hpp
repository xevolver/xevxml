/**
 * @file     dir2xml.hpp
 * @brief    Converting OpenMP-like Compiler Directives to XML.
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
#ifndef ___DIR2XML_H___
#define ___DIR2XML_H___
#include <iostream>
#include <string>
#include <vector>
#include <boost/fusion/tuple.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include "xevxml.hpp"
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>


namespace xevxml {

struct DirAST {
public:
  std::vector<DirAST> succ;
  std::string str;
  std::string cname;

  DirAST() {succ.clear();str.clear();cname="";}
  DirAST(const std::string& c, const std::string& s) {str = s; cname = c;}
  DirAST(const DirAST& dir) {*this=dir;}
  ~DirAST() {}

  DirAST& operator+= (DirAST& dir) { 
    succ.push_back(dir); 
    return *this;
  }
  DirAST& operator= (const DirAST& dir) { 
    succ = dir.succ;
    str  = dir.str;
    cname = dir.cname;
    return *this;
  }
  DirAST& operator= (const std::string& s) { 
    str = s;
    return *this;
  }
  DirAST& operator << (const std::string& s) { 
    cname = s;
    return *this;
  }

  void print(int indent=0) 
  {
    std::cout << succ.size() <<": " << cname << " ";
    if(str.size())
      std::cout << str << std::endl;
    else
      std::cout << std::endl;

    std::vector<DirAST>::iterator it=succ.begin();
    for(;it!=succ.end();++it){
      for(int j(0);j<indent+1;j++) 
	std::cout << '+';
      (*it).print(indent+1);
    }
  }
};


  
class Dir2XmlVisitor {
  std::vector<xercesc::DOMNode*> defs_;
  std::vector<std::string> dnames_;
  xercesc::DOMDocument * doc_;
public:
  Dir2XmlVisitor(xercesc::DOMDocument * d) {doc_=d;} 
  ~Dir2XmlVisitor() {}
  
  void visit() {visit(doc_);}
  void getDefs(xercesc::DOMNode* node);
  void printDefs()
  {
    for(size_t i(0);i<dnames_.size();i++){
      std::cerr << dnames_[i] << ":" << defs_[i] << std::endl;
    }
  }
protected:
  void visit(xercesc::DOMNode* node);
  void visitSgPragma(xercesc::DOMNode* node);

};

};

#endif

