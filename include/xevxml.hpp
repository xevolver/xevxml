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
#ifndef ___XEVXML_H___
#define ___XEVXML_H___
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <rose.h>
#include <xercesc/dom/DOMNode.hpp>

//#define XEV_USE_ROSEHPCT // experimental

#ifdef XEV_USE_ROSEHPCT
#include <rosehpct/rosehpct.hh>
#endif

namespace XevXml {
  // prototype declaration of option classes
  class XevXmlOption;

  extern void XevInitialize(void);
  extern void XevFinalize(void);
  extern bool XevConvertXmlToRose(std::istream& str, SgProject** prj);
  extern bool XevConvertRoseToXml(std::ostream& str, SgProject** prj, XevXmlOption* opt=NULL);
  extern bool XevUnparseToStream (std::ostream& str, SgProject** prj, UnparseFormatHelp* unparseHelp=NULL,
                                  UnparseDelegate *repl=NULL, SgScopeStatement* unparseScope=NULL );

  // --- XML utility functions ---
  extern void XmlInitialize(void);
  extern void XmlFinalize(void);
  extern std::string XmlStr2Entity( std::string);
  extern std::string XmlEntity2Str( std::string);

  /// Visitor class for traversing XML AST nodes to generate a Sage AST
  class XevXmlVisitor
  {
    SgSourceFile* _file;  /// SgFile to visit
    SgProject*    _prj;   /// SgProject to be built

  public:
    XevXmlVisitor();
    ~XevXmlVisitor();

    /// Visiting all XML elements in a subtree whose root is given as the 1st argument.
    virtual SgNode* visit(xercesc::DOMNode* node, SgNode* astParent=0);

#define SAGE3(NodeType)                                                 \
    virtual SgNode* visitSg##NodeType(xercesc::DOMNode* node, SgNode* astParent=0);
#include "sgnode.hpp"
    virtual SgNode* visitPreprocessingInfo(xercesc::DOMNode* node, SgNode* astParent=0);

    virtual void checkPreprocInfo(xercesc::DOMNode* node, SgNode* astNode);
    virtual void checkExpression (xercesc::DOMNode* node, SgNode* astNode);
    virtual void checkStatement  (xercesc::DOMNode* node, SgNode* astNode);
    virtual void checkDeclStmt  (xercesc::DOMNode* node, SgNode* astNode);

    bool read(std::istream& is, SgProject** prj);
    virtual SgProject* getSgProject() {return _prj;}
  };

  /// Encapsulate options of XevSageVisitor
  class XevXmlOption
  {
    int  lang_;    /// output language
    bool faddr_;   /// print address attribute
    bool fparen_;  /// remove compiler-generated parentheses
    bool fpragma_; /// analyze Fortran pragmas



    void init() {
      lang_    = 0;
      faddr_   = false;
      fparen_  = false;
      fpragma_ = false;
    }
  public:
    XevXmlOption(){init();}
    ~XevXmlOption() {}

    int    getOutoutLanguage()     {return lang_;}
    bool&  getPrintAddressFlag()   {return faddr_;}
    bool&  getRemoveParenFlag()    {return fparen_;}
    bool&  getFortranPragmaFlag()  {return fpragma_;}
  };


  /// Visitor class for traversing Sage AST nodes to generate an XML AST
  class XevSageVisitor {
    XevXmlOption* opt_;
    std::ostream* ostr_;

    SgFile* file_;
    int  depth_;
  protected:
    std::ostream& sstr() {
      return *ostr_;
    }

    void writeIndent()    {
      for(int i(0);i<depth_;i++)
        sstr() << ' ';
    }

    /// return true if a given node has an internal node
    virtual bool hasInode(SgNode* node );

    SgFile* getSgFileToVisit()         {return file_;}
    void    setSgFileToVisit(SgFile* f){file_=f;}

  public:
    void setXmlOption(XevXmlOption* o)  {opt_ = o;}
    XevXmlOption* getXmlOption(){return opt_;}
    std::ostream& getOutputStream() {return sstr();}

    XevSageVisitor (XevXmlOption* o=NULL):opt_(o),ostr_(NULL),depth_(0){}
    ~XevSageVisitor() {}

    /// Visiting all AST nodes in a subtree whose root is given as the argument.
    virtual void visit(SgNode* node);
#define SAGE3(op)                               \
    virtual void attribSg##op(SgNode* node);    \
    virtual void inodeSg##op(SgNode* node);
#include "sgnode.hpp"

    bool write(std::ostream& os, SgProject** prj);
  };

}

#endif
