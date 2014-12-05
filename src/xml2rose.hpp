/**
 * @file     xml2rose.hpp
 * @brief    Classes for converting an XML document to ROSE AST.
 * \license This project is released under the BSD 2-clause license
 *
 * Copyright (C) 2010-2014 Hiroyuki TAKIZAWA. All rights reserved.
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
#ifndef ___XML2AST_H___
#define ___XML2AST_H___
#include "common.hpp"
#include <xevxml.hpp>
#include <xmlutils.hpp>

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
/*#include <xercesc/dom/DOMLSParser.hpp>*/
#include <xercesc/dom/DOMNamedNodeMap.hpp>
/*#include <xercesc/dom/DOMLSInput.hpp>*/
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

namespace XevXml {

  /// (internal) Visitor class for checking if there is a orphan node
class OrphanTest : public AstSimpleProcessing
{
public:
  OrphanTest() {}
  ~OrphanTest() {}

  void visit(SgNode* n){
    if(n){
      //cerr << n->class_name() << ": ";
      void* p = n->get_parent();
      //cerr << n->unparseToString();
      //cerr << p <<endl;
      if(p==0) {
        std::cerr << n->class_name() << ": ";
        std::cerr << n->unparseToString()       << " \n";
        XEV_ABORT();
      }
      //cerr << n->unparseToString();
    }
  }
};

  /// (internal) Visitor class for searching SgVariableDeclaration in a subtree
class VardefSearch
{
  std::string varname;
public:
  VardefSearch(const std::string& name):varname(name) {}
  ~VardefSearch() {}

  SgNode* visit(SgNode* n) {
    SgNode* ret = NULL;
    if(n==NULL) return ret;

    SgVariableDeclaration *decl = isSgVariableDeclaration(n);
    SgVariableDefinition  *def  = isSgVariableDefinition(n);
    /*
    if(def==NULL && decl != NULL )
      def = decl->get_definition();
    */
    if(def){
      if(def->get_vardefn()->get_name().getString() == varname )
        return def->get_vardefn();
    }
    else if (decl){
      SgInitializedNamePtrList& lst = decl->get_variables();
      for(size_t i(0);i<lst.size();++i)
        if( lst[i]->get_name().getString()  == varname )
          return lst[i];
    }
    else {
      for(size_t i(0);i<n->get_numberOfTraversalSuccessors();++i){
        if ( (ret = this->visit(n->get_traversalSuccessorByIndex(i))) != NULL)
          return ret;
      }
    }
    return NULL;
  }
};

  /// (internal) Visitor class for printing out all symbol tables in an AST
class PrintSymTable
{
public:
  PrintSymTable(){}
  ~PrintSymTable(){}

  SgNode* visit(SgNode* n) {
    //SgNode* ret = NULL;
    SgScopeStatement* scope = isSgScopeStatement(n);
    if(scope){
      SgSymbolTable* tbl = scope->get_symbol_table();
      if(tbl==0) XEV_ABORT();
      printf("\n Symbol table of %s (parent=%s)\n",
             n->class_name().c_str(),n->get_parent()->class_name().c_str());
      printf("=== BEGIN ====\n");
      tbl->print();
      printf("===  END  ====\n");
    }
    for(size_t i(0);i<n->get_numberOfTraversalSuccessors();++i){
      SgNode* child = n->get_traversalSuccessorByIndex(i);
      if(child!=NULL)
        this->visit(child);
    }
    return NULL;
  }
};

  /// (internal) Visitor class for finding a reference to a variable not found in symbol tables.
class CheckUndeclVars
{
public:
  CheckUndeclVars(){}
  ~CheckUndeclVars(){}
  void visit(SgNode*);
};


#define DEFAULT_FILE_INFO (Sg_File_Info::generateDefaultFileInfoForTransformationNode())

#define SUBTREE_VISIT_BEGIN(X,Y,Z)                                      \
  {                                                                     \
  xercesc::DOMNode* cld_ = (X)->getFirstChild();                        \
  while(cld_) {                                                         \
  if(cld_->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){            \
  SgNode* Y = this->visit(cld_,Z);

#define SUBTREE_VISIT_END()     } cld_=cld_->getNextSibling();}}

}

#endif
