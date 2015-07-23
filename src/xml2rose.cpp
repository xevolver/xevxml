/**
 * @file     xml2rose.cpp
 * @brief    An XML document is converted to ROSE Sage III AST.
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

#include <iostream>
#include "common.hpp"
#include "xml2rose.hpp"

//* ROSE headers */
#include "Cxx_Grammar.h"
#include "StorageClasses.h"
#include "fortran_support.h"
#include "xmlparser.hpp"
namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;


namespace XevXml {

bool XevConvertXmlToRose(istream& str, SgProject** prj)
{
  XevXmlVisitor visitor;
  return visitor.read(str,prj);
}}


using namespace XevXml;
XevXmlVisitor::XevXmlVisitor()
{
  _prj = new SgProject();
  if(_prj==0){ XEV_ABORT(); }
  _file = new SgSourceFile();
  if(_file==0){ XEV_ABORT(); }
  _prj->set_file(*_file); // set_file() is obsolete.
  _file->set_parent(_prj);

  Sg_File_Info* info = DEFAULT_FILE_INFO;
  _file->set_file_info(info);
  info->set_parent(_file);
}

XevXmlVisitor::~XevXmlVisitor() {}

bool XevXmlVisitor::read(std::istream& is, SgProject** prj) {
  if(prj == 0){ // if (*prj == 0) then *prj is set later.
    XEV_WARN("Invalid SgProject pointer. Conversion failed.");
    return false;
  }

  try {
    xe::DOMDocument* doc = 0;
    //xe::XercesDOMParser parser;
    XevDOMParser parser;
    std::istreambuf_iterator<char> begin(is);
    std::istreambuf_iterator<char> end;
    std::string buf(begin,end);
    //string buf = tr.str();
    xe::MemBufInputSource
      membuf((const XMLByte*)buf.c_str(), buf.length(), "memory_buffer");
    parser.parse(membuf);
    doc = parser.getDocument();
    buf.clear();

    visit(doc,0);
    *prj = getSgProject();
    //AstTests::runAllTests(*prj);
  }
  catch(std::exception& e) {
    XEV_WARN("Exception thrown. Conversion failed. " << e.what());
    return false;
  }
  catch(...) {
    XEV_WARN("Exception thrown. Conversion failed. Unknown exception");
    return false;
  }
  return true;
}

void checkLocatedNode(xe::DOMNode* node, SgNode* astNode)
{
  SgLocatedNode* n = isSgLocatedNode(astNode);
  string file_info;
  int same=0;

  if(n==0) return;
  //if(isSgCastExp(n) == 0)
    si::setSourcePositionAsTransformation(n);
  if(XmlGetAttributeValue(node, "samefile", &same) && same==0 ) {
    n->get_file_info()->set_file_id(Sg_File_Info::COMPILER_GENERATED_FILE_ID);
    if( n->get_parent()==0 || isSgType(n->get_parent()) || isSgGlobal(n->get_parent())
	|| isSgNamespaceDefinitionStatement(n->get_parent()) || isSgCastExp(n) ){
      si::setSourcePosition(n);
      n->get_file_info()->setCompilerGenerated();
      n->get_startOfConstruct()->setCompilerGenerated();
      n->get_endOfConstruct()->setCompilerGenerated();
      n->get_file_info()->setFrontendSpecific();
      n->get_startOfConstruct()->setFrontendSpecific();
      n->get_endOfConstruct()->setFrontendSpecific();
      n->get_file_info()->unsetOutputInCodeGeneration();
      n->get_startOfConstruct()->unsetOutputInCodeGeneration();
      n->get_endOfConstruct()->unsetOutputInCodeGeneration();
      n->get_file_info()->unsetTransformation();
      n->get_startOfConstruct()->unsetTransformation();
      n->get_endOfConstruct()->unsetTransformation();
      //std::cerr << "NOTE: " << n->class_name() << " is ignored"<<std::endl;
      //si::dumpPreprocInfo(n);
    }
  }
  if(XmlGetAttributeValue(node,"file_info",&file_info)) {
    int fid, line, col;
    stringstream ss;
    ss << file_info;
    ss >> fid;
    ss >> line;
    ss >> col;
    Sg_File_Info* info = n->get_file_info();
    if(info==NULL){
      n->set_file_info(DEFAULT_FILE_INFO);
      if(n->get_file_info()->isCompilerGenerated()==false){
        // does this happen??
        n->get_file_info()->set_line(line);
        n->get_file_info()->set_col(col);
      }
    }
    //cerr << n->class_name() << ": line=" << line << ", col=" << col << endl;
  }
}

SgNode*
XevXmlVisitor::visit(xe::DOMNode* node, SgNode* astParent)
{
  SgNode* ret = 0;

  if(node) {
    char* buf = xe::XMLString::transcode(node->getNodeName());
    string nname = buf;
    xe::XMLString::release(&buf);

    if (node->getNodeType() == xe::DOMNode::DOCUMENT_NODE){
      xe::DOMNode* child=node->getFirstChild();
      while(child) {
        this->visit(child);
        xe::DOMNode* next=child->getNextSibling();
        child=next;
      }
    }
    else {
      //#define XEVXML_DEBUG
#ifdef XEVXML_DEBUG
      static int g_count=0;
#define SAGE3(x) if(nname=="Sg"#x) { cerr << #x << "(" << g_count++ << ")" << endl; ret = visitSg##x (node,astParent);}
#else
#define SAGE3(x) if(nname=="Sg"#x) { ret = visitSg##x (node,astParent);}
#endif
#include "sgnode.hpp"

      if(ret==NULL && nname != "PreprocessingInfo") {
        XEV_WARN( "unknown AST node found: " << nname );
        XEV_ABORT();
      }
      if(ret!=NULL && ret->get_parent() != astParent) {
        ret->set_parent(astParent);
      }
      if(isSgSupport(ret)!=NULL)
        ret->set_parent(NULL);

      checkPreprocInfo(node,ret);
      checkExpression(node,ret);
      checkStatement(node,ret);
      checkDeclStmt(node,ret);
      checkLocatedNode(node,ret);
      if(isSgType(ret)==0 && isSgSourceFile(ret)==0 && astParent==0 ){
	XEV_WARN("SgNode with no parent is found\n");
	XEV_DEBUG_INFO(node);
        XEV_ABORT();
      }
      return ret;
    }
  }
  return 0;
}


void
XevXmlVisitor::checkExpression(xe::DOMNode* node, SgNode* astNode)
{
  SgExpression* e = isSgExpression(astNode);

  if(e) {
    int parenf=0;
    int lvalf=0;

    XmlGetAttributeValue(node,"paren",&parenf);
    XmlGetAttributeValue(node,"lvalue",&lvalf);
    e->set_need_paren(parenf);
    e->set_lvalue(lvalf);
  }
#if 1
  SgAssignInitializer* ini = isSgAssignInitializer(astNode);

  if(ini){
    int expl = 0;
    if(XmlGetAttributeValue(node,"cast",&expl)){
      //SgCastExp* c = isSgCastExp(ini->get_originalExpressionTree());
      //SgCastExp* c = isSgCastExp(ini->get_operand());
      //if(c)
      ini->set_is_explicit_cast(expl);
    }
  }
#endif
}

void
XevXmlVisitor::checkStatement(xe::DOMNode* node, SgNode* astNode)
{
  int ino = 0;
  SgStatement* stmt = isSgStatement(astNode);
  if(stmt && XmlGetAttributeValue(node,"label",&ino)) {
    SgNode* astParent = astNode->get_parent();
    SgLabelSymbol*  s = new SgLabelSymbol();
    s->set_parent(stmt);
    if(astParent == 0) {
      XEV_WARN (astNode->class_name() << " does not have a parent node.");
      XEV_DEBUG_INFO(node);
      XEV_ABORT();
    }
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()));
    s->get_fortran_statement()->set_parent(s);
    //s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    s->set_label_type(SgLabelSymbol::e_start_label_type);
    s->set_numeric_label_value( ino );
    SgLabelRefExp*  l = new SgLabelRefExp( s );
    stmt->set_numeric_label(l);
    l->set_parent(stmt);
  }
  if(isSgScopeStatement(stmt) ) {
    if(stmt->get_parent()==0 || stmt->get_scope()==0) {
#if XEV_DEBUG
      XEV_WARN( stmt->class_name() << " does not have parent or scope.");
      XEV_DEBUG_INFO(node);
      //XEV_ABORT();
#endif
    }
    if(si::is_Fortran_language())
      isSgScopeStatement(stmt)->setCaseInsensitive(true);
  }
}

void
XevXmlVisitor::checkDeclStmt(xe::DOMNode* node, SgNode* astNode)
{
  SgDeclarationStatement* decl = isSgDeclarationStatement(astNode);
  int enf=0;
  int regparm=0;
  unsigned long mod=0;
  string rname;
  if(decl==0) return;
  SgBitVector vec;
  SgDeclarationModifier& m = decl->get_declarationModifier();

  if(XmlGetAttributeValue(node,"declaration_modifier",&mod)){
    vec = m.get_modifierVector();
    for(size_t i(0);i<vec.size();i++){
      vec[i] = (mod & 1);
      mod >>= 1;
    }
    m.set_modifierVector(vec);
  }

  mod = 0;
  if(XmlGetAttributeValue(node,"type_modifier",&mod)){
    vec = m.get_typeModifier().get_modifierVector();
    for(size_t i(0);i<vec.size();i++){
      vec[i] = (mod & 1);
      mod >>= 1;
    }
    m.get_typeModifier().set_modifierVector(vec);
  }

  mod=SgConstVolatileModifier::e_default;
  XmlGetAttributeValue(node,"cv_modifier",&mod);
  m.get_typeModifier().get_constVolatileModifier().set_modifier((SgConstVolatileModifier::cv_modifier_enum)mod);
  mod=-1;
  if(XmlGetAttributeValue(node,"alignment",&mod))
    m.get_typeModifier().set_gnu_attribute_alignment(mod);
  mod=SgAccessModifier::e_default;
  XmlGetAttributeValue(node,"access_modifier",&mod);
  m.get_accessModifier().set_modifier((SgAccessModifier::access_modifier_enum)mod);

  mod =SgStorageModifier::e_default;
  XmlGetAttributeValue(node,"storage_modifier",&mod);
  m.get_storageModifier().set_modifier((SgStorageModifier::storage_modifier_enum)mod);

  mod=0;
  XmlGetAttributeValue(node,"thread_local",&mod);
  m.get_storageModifier().set_thread_local_storage(mod);

  string bind;
  if(XmlGetAttributeValue(node,"bind",&bind)){
    decl->set_binding_label(bind);
  }
  string link;
  string asmn;
  if(XmlGetAttributeValue(node,"link",&link)){
    decl->set_linkage(link);
  }

  mod = SgDeclarationModifier::e_default_visibility;
  if(XmlGetAttributeValue(node,"visibility",&mod)){
    m.set_gnu_attribute_visability
      ((SgDeclarationModifier::gnu_declaration_visability_enum) mod);
  }

  SgFunctionDeclaration* fdecl = isSgFunctionDeclaration(decl);
  if(fdecl==NULL)return;

  if(XmlGetAttributeValue(node,"function_modifier",&mod)){
    vec = fdecl->get_functionModifier().get_modifierVector();
    for(size_t i(0);i<vec.size();i++){
      vec[i] = (mod & 1);
      mod >>= 1;
    }
    fdecl->get_functionModifier().set_modifierVector(vec);
  }

  if(XmlGetAttributeValue(node,"end_name",&enf))
    fdecl->set_named_in_end_statement(enf);

  if(XmlGetAttributeValue(node,"asm_name",&asmn))
    fdecl->set_asm_name(asmn);

  if(XmlGetAttributeValue(node,"regparm",&regparm))
    fdecl->set_gnu_regparm_attribute(regparm);

  if(fdecl->get_definition()!=NULL)
    fdecl->get_definition()->set_declaration(fdecl);


#if 1
  if( si::is_Fortran_language() && XmlGetAttributeValue(node,"result",&rname) ){

    SgFunctionDefinition* fdf = fdecl->get_definition();
    if(fdf==0) // for SgEntryStatement
      fdf = si::getEnclosingProcedure (sb::topScopeStack());
    if(fdf==0) {
      XEV_DEBUG_INFO(node);
      XEV_ABORT();
    }
    // the symbol table of fdf is not created yet. i don't know why.
    VardefSearch search(rname);
    SgInitializedName* ini = isSgInitializedName(search.visit(fdf));
    bool found = false;
    if(ini){
      found = true;
      ini->set_declptr(fdecl);
    }
    else {
      //XEV_WARN("result variable not found");
      ini = sb::buildInitializedName(rname,fdecl->get_type()->get_return_type());
      ini->set_parent(fdecl);
      //ini->set_definition(fdf);
      //ini->set_declptr(ret); // s005.f90:mismatch if uncommented, but needed for h025.f90
      ini->set_type(fdecl->get_type()->get_return_type());
      ini->set_scope(fdf);

      SgVariableSymbol* sym = new SgVariableSymbol(ini);
      sym->set_parent(fdf);
    }
    if(isSgProcedureHeaderStatement(decl)){
      SgProcedureHeaderStatement* prc = isSgProcedureHeaderStatement(decl);
      prc->set_result_name(ini);
      if(found)
        prc->get_result_name()->set_definition(ini->get_declaration());
    }
    else if (isSgEntryStatement(decl)){
      SgEntryStatement* ent = isSgEntryStatement(decl);
      ini->set_type(ent->get_type()->get_return_type());
      ent->set_result_name(ini);
      if(found)
        ent->get_result_name()->set_definition(ini->get_declaration());
    }
  }
#endif


}

void CheckUndeclVars::visit(SgNode* n) {
  SgVarRefExp* exp = isSgVarRefExp(n);
  SgScopeStatement* scopestmt = isSgScopeStatement(n);
  if(n==0) return;
  if(exp){
    //SgScopeStatement* scope = SageBuildertopScopeStack();
    SgInitializedName* ini = exp->get_symbol()->get_declaration();
    if(ini==0){
      std::cerr << exp->class_name() << " does not have ininame" << std::endl;
      std::cerr << "parent " << exp->get_parent()->class_name() << std::endl;
      std::cerr << isSgBinaryOp(exp->get_parent())->get_lhs_operand()
                << ": " << exp
                << std::endl;
    }
    else {
      SgScopeStatement* scope = ini->get_scope();
      SgType* typ = ini->get_type();
      if(SageInterface::getElementType(typ))
        typ = SageInterface::getElementType(typ);
      std::cerr << ini->get_name().getString() << "("
                << typ->class_name()
                <<")@" << scope->class_name() << std::endl;
    }
#if 0
    scope = sb::topScopeStack();
    if(ini==0)XEV_ABORT();
    if(ini->get_declptr()==0){
      SgName name = ini->get_name();
      // Implicit variable found
      SgInitializedName* newini
        = SageBuilder::buildInitializedName(name,generateImplicitType(name));
      if(newini==0) XEV_ABORT();

      if(newini->get_declptr()==0){
        SgVariableDeclaration* decl =0;
        decl
          = SageBuilder::buildVariableDeclaration(name,newini->get_type(),NULL,scope);
        decl->set_parent(scope);
        decl->set_definingDeclaration(decl);
        newini->set_declptr(decl);
        newini->set_definition(decl);
        newini->set_parent(decl);
      }
      newini->set_scope(scope);
      SgVariableSymbol* sym = new SgVariableSymbol(newini);
      scope->insert_symbol(newini->get_name(),sym);
      sym->set_parent(scope);
      exp->set_symbol(sym);
    }
#endif
  }
  if(scopestmt)
    SageBuilder::pushScopeStack(scopestmt);
  for(size_t i(0);i<n->get_numberOfTraversalSuccessors();++i){
    if(n->get_traversalSuccessorByIndex(i))
      this->visit(n->get_traversalSuccessorByIndex(i));
  }
  if(scopestmt)
    SageBuilder::popScopeStack();

  return;
}
