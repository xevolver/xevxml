/**
 * @file     xml2ast.cpp
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
#include "xml2ast.hpp"

//* ROSE headers */
#include "Cxx_Grammar.h"
#include "StorageClasses.h"
#include "fortran_support.h"

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;

//#define XEVXML_DEBUG
#ifdef XEVXML_DEBUG
int g_count=0;
#define VISIT(x) if(nname==#x) { cerr << #x << "(" << g_count++ << ")" << endl; ret = visit##x (node,astParent);}
#else
#define VISIT(x) if(nname==#x) { ret = visit##x (node,astParent);}
#endif

#define SUBTREE_VISIT_BEGIN(X,Y,Z)					\
  {									\
  xercesc::DOMNode* cld_ = (X)->getFirstChild();			\
  while(cld_) {								\
  if(cld_->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){		\
  SgNode* Y = this->visit(cld_,Z);

#define SUBTREE_VISIT_END()     } cld_=cld_->getNextSibling();}}



static bool
TraverseXercesDOMDocument(stringstream& tr, SgProject** prj, XevXML::XevConversionHelper* help)
{
  try {
    xe::DOMDocument* doc = 0;
    xe::XercesDOMParser parser;
    string buf = tr.str();
    xe::MemBufInputSource 
      membuf((const XMLByte*)buf.c_str(), buf.length(), "memory_buffer");
    parser.parse(membuf);
    doc = parser.getDocument();

    class XevXML::XevXmlVisitor visit(*prj,help);
    visit.visit(doc,0);
    *prj = visit.getSgProject();

#if 0
#ifdef XEVXML_DEBUG    
    XevXML::OrphanTest test;
    test.traverse(&(*prj)->get_file(0)->get_globalScope(),preorder);
#endif
#endif
    //XevXML::PrintSymTable symtbl;
    //symtbl.visit(&((*prj)->get_file(0)));
    //AstTests::runAllTests(*prj);
  }
  catch(...) {
    return false;
  }
  return true;
}



namespace XevXML {

bool XevConvertXmlToAst(stringstream& str, SgProject** prj, XevConversionHelper* help)
{
  if(prj == 0){ // if (*prj == 0) then *prj is set later.
    XEV_WARN("Invalid SgProject pointer. Conversion failed.");
    return false;
  }
  // set default configuration
  if(help==0) help = new XevConversionHelper();
  
  if(TraverseXercesDOMDocument(str,prj,help)==false){
    XEV_WARN("XML document parsing failed.");
    return false;
  }
  return true;
}}

using namespace XevXML;
XevXmlVisitor::XevXmlVisitor(SgProject* prj, XevConversionHelper* help)
{
  if(help==0) XEV_ABORT();
  _help = help;

  if(prj) {
    _prj = prj;
    _file = isSgSourceFile(&(prj->get_file(0)));
  }
  else {
    _prj = new SgProject();
    _file = new SgSourceFile();
    _prj->set_file(*_file);
    _file->set_parent(_prj);
  }
  if(_file==0){ XEV_ABORT(); }
  Sg_File_Info* info 
    = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  _file->set_file_info(info);
  info->set_parent(_file);
}

XevXmlVisitor::
~XevXmlVisitor() {}


SgNode* 
XevXmlVisitor::visit(xe::DOMNode* node, SgNode* astParent)
{
  SgNode* ret = 0;

  if(node) {
    if (node->getNodeType() == xe::DOMNode::DOCUMENT_NODE){
      xe::DOMNode* child=node->getFirstChild();
      while(child) {
        this->visit(child);
        xe::DOMNode* next=child->getNextSibling();
        child=next;
      } 
    }
    else {
      char* buf = xe::XMLString::transcode(node->getNodeName());
      string nname = buf;
      xe::XMLString::release(&buf);

#if 0
      if(astParent == 0){
	buf = xe::XMLString::transcode(node->getParentNode()->getNodeName());
	cerr << buf << " calls visit method without 2nd argument" <<endl;
	xe::XMLString::release(&buf);
      }
#endif

#if 0
      if(nname=="SgVarRefExp")
	for(std::list<SgScopeStatement*>::iterator it=sb::ScopeStack.begin();
	    it != sb::ScopeStack.end();++it){
	  cerr << (*it)->class_name() << endl;
	}
#endif

      VISIT(SgSourceFile);
      VISIT(SgGlobal);
      VISIT(SgBasicBlock);
      VISIT(SgPragmaDeclaration);
      VISIT(SgPragma);
      VISIT(SgVariableDeclaration);
      VISIT(SgFunctionDeclaration);
      VISIT(SgFunctionParameterList);
      VISIT(SgInitializedName);
      VISIT(SgAssignInitializer);

      VISIT(SgBoolValExp);
      VISIT(SgCharVal);
      VISIT(SgWcharVal);
      VISIT(SgDoubleVal);
      VISIT(SgFloatVal);
      VISIT(SgIntVal);
      VISIT(SgLongIntVal);
      VISIT(SgLongLongIntVal);
      //VISIT(SgEnumVal);
      VISIT(SgLongDoubleVal);
      VISIT(SgShortVal);
      VISIT(SgStringVal);
      VISIT(SgUnsignedCharVal);
      VISIT(SgUnsignedShortVal);
      VISIT(SgUnsignedIntVal);
      VISIT(SgUnsignedLongVal);
      VISIT(SgUnsignedLongLongIntVal);

      VISIT(SgTypeInt);
      VISIT(SgTypeFloat);
      VISIT(SgTypeDouble);
      VISIT(SgModifierType);

      VISIT(SgExprStatement);
      VISIT(SgForStatement);
      VISIT(SgForInitStatement);
      VISIT(SgIfStmt);
      VISIT(SgReturnStmt);
      VISIT(SgFunctionDefinition);
      VISIT(SgNullExpression);

      VISIT(SgVarRefExp);
      VISIT(SgCastExp);
      VISIT(SgColonShapeExp);

      VISIT(SgPlusPlusOp);
      VISIT(SgMinusOp);
      VISIT(SgAssignOp);
      VISIT(SgPlusAssignOp);
      VISIT(SgMultAssignOp);
      VISIT(SgAddOp);
      VISIT(SgLessThanOp);
      VISIT(SgConcatenationOp);

      VISIT(SgTypeChar);                            // 0701
      VISIT(SgTypeUnsignedChar);                    // 0701
      VISIT(SgTypeSignedChar);                      // 0701
      VISIT(SgTypeShort);                           // 0701
      VISIT(SgTypeUnsignedShort);                   // 0701
      VISIT(SgTypeSignedShort);                     // 0701
      VISIT(SgTypeUnsignedInt);                     // 0701
      VISIT(SgTypeSignedInt);                       // 0701
      VISIT(SgTypeLong);                            // 0701
      VISIT(SgTypeUnsignedLong);                    // 0701
      VISIT(SgTypeSignedLong);                      // 0701
      VISIT(SgTypeLongLong);                        // 0701
      VISIT(SgTypeUnsignedLongLong);                // 0701
      VISIT(SgTypeSignedLongLong);                  // 0701
      VISIT(SgTypeVoid);                            // 0701
      VISIT(SgTypeBool);                            // 0701
      VISIT(SgTypeLongDouble);                      // 0701
      VISIT(SgWhileStmt);                           // 0701
      VISIT(SgDoWhileStmt);                         // 0701
      VISIT(SgSwitchStatement);                     // 0701
      VISIT(SgCaseOptionStmt);                      // 0701
      VISIT(SgBreakStmt);                           // 0701
      VISIT(SgDefaultOptionStmt);                   // 0701
      VISIT(SgContinueStmt);                        // 0701
      VISIT(SgGreaterThanOp);                       // 0701
      VISIT(SgLessOrEqualOp);                       // 0701
      VISIT(SgGreaterOrEqualOp);                    // 0701
      VISIT(SgNotEqualOp);                          // 0701
      VISIT(SgMinusMinusOp);                        // 0701
      VISIT(SgNotOp);                               // 0701
      VISIT(SgOrOp);                                // 0701
      VISIT(SgEqualityOp);                          // 0701
      VISIT(SgAndOp);                               // 0701
      VISIT(SgModOp);                               // 0701
      VISIT(SgDivideOp);                            // 0701
      VISIT(SgMultiplyOp);                          // 0701
      VISIT(SgMinusAssignOp);                       // 0701
      VISIT(SgIorAssignOp);                         // 0701
      VISIT(SgAndAssignOp);                         // 0701
      VISIT(SgXorAssignOp);                         // 0701
      VISIT(SgBitComplementOp);                     // 0701
      VISIT(SgLshiftOp);                            // 0701
      VISIT(SgRshiftOp);                            // 0701
      VISIT(SgConditionalExp);                      // 0701
      VISIT(SgSubtractOp);                          // 0701
      VISIT(SgDivAssignOp);                         // 0701
      VISIT(SgModAssignOp);                         // 0701
      VISIT(SgBitAndOp);                            // 0701
      VISIT(SgBitOrOp);                             // 0701
      VISIT(SgBitXorOp);                            // 0701
      VISIT(SgCommaOpExp);                          // 0701
      VISIT(SgSizeOfOp);                            // 0701
      VISIT(SgArrayType);                           // 0701
      VISIT(SgAddressOfOp);                         // 0701
      VISIT(SgUnaryAddOp);                          // 0701
      VISIT(SgLabelStatement);                      // 0701
      VISIT(SgGotoStatement);                       // 0701
      VISIT(SgExprListExp);                         // 0701

      VISIT(SgTypedefType);                         // 0702
      VISIT(SgPntrArrRefExp);                       // 0702

      VISIT(SgNullStatement);                       // 0705

      VISIT(SgUseStatement);                        // 0708
      VISIT(SgFortranIncludeLine);                  // 0708

      VISIT(SgProcedureHeaderStatement);            // 0709
      VISIT(SgAttributeSpecificationStatement);     // 0709
      VISIT(SgImplicitStatement);                   // 0709

      VISIT(SgFortranDo);                           // 0710

      VISIT(SgFunctionCallExp);                     // 0711
      VISIT(SgFunctionRefExp);                      // 0711

      VISIT(SgTypeString);                          // 0717

      VISIT(SgTypeComplex);                         // 0718
      VISIT(SgComplexVal);                          // 0718

      VISIT(SgClassDeclaration);                    // 0719
      VISIT(SgClassDefinition);                     // 0719
      VISIT(SgClassType);                           // 0719

      VISIT(SgEnumDeclaration);                     // 0722
      VISIT(SgEnumType);                            // 0722
      VISIT(SgEnumVal);                             // 0722
      VISIT(SgTypedefDeclaration);                  // 0722
      VISIT(SgDotExp);                              // 0722
      VISIT(SgPointerType);                         // 0722

      VISIT(SgArrowExp);                            // 0723

      VISIT(SgProgramHeaderStatement);              // 0724
      VISIT(SgPrintStatement);                      // 0724
      VISIT(SgAsteriskShapeExp);                    // 0724
      VISIT(SgLabelRefExp);                         // 0724

      VISIT(SgFormatStatement);                     // 0725
      VISIT(SgFormatItem);                          // 0725
      VISIT(SgWriteStatement);                      // 0725
      VISIT(SgOpenStatement);                       // 0725
      VISIT(SgCloseStatement);                      // 0725
      VISIT(SgContainsStatement);                   // 0725
      VISIT(SgModuleStatement);                     // 0725
      VISIT(SgExponentiationOp);                    // 0725

      VISIT(SgCommonBlock);                         // 0726
      VISIT(SgCommonBlockObject);                   // 0726
      VISIT(SgForAllStatement);                     // 0726
      VISIT(SgSubscriptExpression);                 // 0726

      VISIT(SgInterfaceStatement);                  // 0730
      VISIT(SgInterfaceBody);                       // 0730
      VISIT(SgReadStatement);                       // 0730
      VISIT(SgEntryStatement);                      // 0730
      VISIT(SgAllocateStatement);                   // 0730
      VISIT(SgDeallocateStatement);                 // 0730
      VISIT(SgArithmeticIfStatement);               // 0730
      VISIT(SgStopOrPauseStatement);                // 0730

      VISIT(SgWhereStatement);                      // 0731
      VISIT(SgElseWhereStatement);                  // 0731
      VISIT(SgNullifyStatement);                    // 0731
      VISIT(SgBackspaceStatement);                  // 0731
      VISIT(SgEndfileStatement);                    // 0731
      VISIT(SgRewindStatement);                     // 0731
      VISIT(SgInquireStatement);                    // 0731
      VISIT(SgFlushStatement);                      // 0731

      VISIT(SgNamelistStatement);                   // 0801
      VISIT(SgDerivedTypeStatement);                // 0801
      VISIT(SgComputedGotoStatement);               // 0801
      VISIT(SgTypeEllipse);                         // 0801
      VISIT(SgTypeDefault);                         // 0801
      VISIT(SgPointerDerefExp);                     // 0801
      VISIT(SgVarArgStartOp);                       // 0801
      VISIT(SgVarArgOp);                            // 0801
      VISIT(SgVarArgEndOp);                         // 0801
      VISIT(SgEquivalenceStatement);                // 0801

      VISIT(SgAsmStmt);                             // 0802
      VISIT(SgAggregateInitializer);                // 0802
      VISIT(SgFunctionType);                        // 0802
      VISIT(SgFunctionParameterTypeList);           // 0802
      VISIT(SgPointerAssignOp);                     // 0802

      VISIT(SgTypeImaginary);                       // 0809
      VISIT(SgCompoundInitializer);                 // 0809
      VISIT(SgLshiftAssignOp);                      // 0819
      VISIT(SgRshiftAssignOp);                      // 0819

      VISIT(SgImpliedDo);                           // 0827
      VISIT(SgDataStatementGroup);                  // 0827
      VISIT(SgDataStatementObject);                 // 20141005
      VISIT(SgDataStatementValue);                  // 20141005

      VISIT(SgRenamePair);                          // 20140824
      VISIT(SgConstructorInitializer);              // 20140921


      if( ret == 0 && nname != "PreprocessingInfo" ) {
        XEV_WARN( "unknown AST node found " << nname );
        XEV_ABORT();
      }
      if(ret && ret->get_parent() != astParent) {
	ret->set_parent(astParent);
      }
      if(isSgSupport(ret)!=NULL) 
	ret->set_parent(NULL);

      checkPreprocInfo(node,ret);
      checkExpression(node,ret);
      checkStatement(node,ret);
      checkDeclStmt(node,ret);

      return ret;
    }
  }
  return 0;
}

void 
XevXmlVisitor::checkExpression(xe::DOMNode* node, SgNode* astNode)
{
  SgExpression* e = isSgExpression(astNode);
  int parenf=0;
  int lvalf=0;

  if(e && XmlGetAttributeValue(node,"paren",&parenf)) // probably paren="1"
    e->set_need_paren(parenf);
  if(e && XmlGetAttributeValue(node,"lvalue",&lvalf)) // probably lvalue="1"
    e->set_lvalue(lvalf);
}

void 
XevXmlVisitor::checkStatement(xe::DOMNode* node, SgNode* astNode)
{
  int ino = 0;
  SgStatement* stmt = isSgStatement(astNode);
  if(stmt && XmlGetAttributeValue(node,"s_nlabel",&ino)) {
    SgNode* astParent = astNode->get_parent();
    SgLabelSymbol*  s = new SgLabelSymbol();
    s->set_parent(stmt);
    if(astParent == 0) {
      XEV_WARN (astNode->class_name() << " does not have a parent node.");
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
    if(stmt->get_parent()==0 || stmt->get_scope()==0) XEV_ABORT();
    if(si::is_Fortran_language())
      isSgScopeStatement(stmt)->setCaseInsensitive(true);
  }
}

void 
XevXmlVisitor::checkDeclStmt(xe::DOMNode* node, SgNode* astNode)
{
  SgDeclarationStatement* decl = isSgDeclarationStatement(astNode);
  int enf=0;
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
  if(XmlGetAttributeValue(node,"type_modifier",&mod)){  
    vec = m.get_typeModifier().get_modifierVector();
    for(size_t i(0);i<vec.size();i++){
      vec[i] = (mod & 1);
      mod >>= 1;
    }
    m.get_typeModifier().set_modifierVector(vec);
  }
  if(XmlGetAttributeValue(node,"cv_modifier",&mod)) 
    m.get_typeModifier().get_constVolatileModifier().set_modifier((SgConstVolatileModifier::cv_modifier_enum)mod);
  if(XmlGetAttributeValue(node,"access_modifier",&mod)) 
    m.get_accessModifier().set_modifier((SgAccessModifier::access_modifier_enum)mod);
  if(XmlGetAttributeValue(node,"storage_modifier",&mod))  
    m.get_storageModifier().set_modifier((SgStorageModifier::storage_modifier_enum)mod);
  
#if 0
  XmlGetAttributeValue(node,"storage",&storage);  

  if(storage & (1U<<0))
    ((decl->get_declarationModifier()).get_storageModifier()).setUnknown();
  if(storage & (1U<<1))
    ((decl->get_declarationModifier()).get_storageModifier()).setStatic();
  if(storage & (1U<<2))
    ((decl->get_declarationModifier()).get_storageModifier()).setExtern();
  if(storage & (1U<<3))
    ((decl->get_declarationModifier()).get_storageModifier()).setAuto();
  if(storage & (1U<<4))
    ((decl->get_declarationModifier()).get_storageModifier()).setRegister();
  if(storage & (1U<<5))
    ((decl->get_declarationModifier()).get_storageModifier()).setTypedef();
  //if(storage==0)
  //((decl->get_declarationModifier()).get_storageModifier()).setDefault();
#endif

  SgFunctionDeclaration* fdecl = isSgFunctionDeclaration(decl);
  if(fdecl==NULL)return;

  if(XmlGetAttributeValue(node,"end_name",&enf))
    fdecl->set_named_in_end_statement(enf);

  if(fdecl->get_definition()!=NULL)
    fdecl->get_definition()->set_declaration(fdecl);
#if 1
  if( si::is_Fortran_language() && XmlGetAttributeValue(node,"result_name",&rname) ){

    SgFunctionDefinition* fdf = fdecl->get_definition();
    if(fdf==0) // for SgEntryStatement
      fdf = si::getEnclosingProcedure (sb::topScopeStack());
    if(fdf==0) XEV_ABORT();
    // the symbol table of fdf is not created yet. i don't know why.
    VardefSearch search(rname);
    SgInitializedName* ini = isSgInitializedName(search.visit(fdf));
    bool found = false;
    if(ini){
      //XEV_WARN("result variable found");
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
#if 0
      if(fdf->lookup_variable_symbol(SgName(rname))){
	fdf->remove_symbol(fdf->lookup_variable_symbol(SgName(rname)));
      }
      fdf->insert_symbol(ini->get_name(),sym);
#endif
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


SgNode* 
XevXmlVisitor::visitSgSourceFile(xe::DOMNode* node, SgNode* astParent)
{
  xe::DOMNode* child=node->getFirstChild();
  int langid;
  int fmtid;

  if(XmlGetAttributeValue(node,"language",&langid)==false)
    XEV_WARN("Language attribute is missing.");
  if(XmlGetAttributeValue(node,"format",&fmtid)==false)
    XEV_WARN("Format attribute is missing.");

  // 0:error, 1: unknown, 2:C, 3:C++, 4:Fortran
  if(langid==2){
    _file->set_C_only(true);
    _file->set_outputLanguage(SgFile::e_C_output_language);
  }
  else if(langid==4){
    _file->set_Fortran_only(true);
    _file->set_outputLanguage(SgFile::e_Fortran_output_language);
    sb::symbol_table_case_insensitive_semantics = true;
    if(fmtid==1) // 0:unknown, 1:fixed, 2:free
      _file->set_outputFormat(SgFile::e_fixed_form_output_format);
    else {
      _file->set_outputFormat(SgFile::e_free_form_output_format);
      _file->set_backendCompileFormat(SgFile::e_free_form_output_format);
    }
  } // C++ is not supported for now 

  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      this->visit(child,_file);
    }
    child=child->getNextSibling();
  } 
  
  return _file;
}

static 
void traverseStatementsAndTexts(XevXmlVisitor* vis, xe::DOMNode* node, SgNode* blk)
{
  xe::DOMNode* child=node->getFirstChild();
  SgStatement* prev=NULL;
  std::string remain ="";
  SgBasicBlock* bb = isSgBasicBlock(blk);

  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgStatement* astchild = isSgStatement(vis->visit(child,blk));
      if(astchild) {
	if(bb) bb->append_statement(isSgStatement(astchild));
	else 
	  si::appendStatement (astchild,isSgScopeStatement(blk));
	prev=isSgStatement(astchild);
	if(remain.length()){ //the text is located before the 1st statement
	  si::attachArbitraryText(prev,remain,PreprocessingInfo::before);  
	  remain="";
	}
      }
    }
    else if (child->getNodeType() == xe::DOMNode::TEXT_NODE){
      std::string tmp = xe::XMLString::transcode(child->getNodeValue());
      std::string text = "";
      std::string line = "";
      size_t i(0);
      bool flag = false;
      // skip white spaces
      for(i=0;i<tmp.length();i++) {
	if(!isspace(tmp[i])) flag = true;
	line += tmp[i];
	if(tmp[i]=='\n') {
	  if(flag)
	    text += line;
	  line = "";
	  flag = false;
	}
      }
      if(flag)
	text += line;
      if(text.length()){
	if(prev) 
	  si::attachArbitraryText(prev,text,PreprocessingInfo::after);  
	else // the text is located before the 1st statement
	  remain = text;
      }
    }
    child=child->getNextSibling();
  }
  if(remain.length()) // for empty basic block
    si::attachArbitraryText(isSgLocatedNode(blk),remain,PreprocessingInfo::inside);  
}

SgNode* 
XevXmlVisitor::visitSgGlobal(xe::DOMNode* node, SgNode* astParent)
{
  Sg_File_Info* info = DEFAULT_FILE_INFO;
  SgGlobal* ret = new SgGlobal(info);
  _file->set_globalScope(ret);
  ret->set_parent(_file);
  ret->set_file_info(info);
  ret->get_globalFunctionTypeTable()->set_parent(ret);
  if(si::is_Fortran_language())
    ret->setCaseInsensitive(true);

  sb::pushScopeStack(ret);
  traverseStatementsAndTexts(this,node,ret);
  sb::popScopeStack();

  //si::fixVariableReferences(ret);
  //CheckUndeclVars checker;
  //checker.visit(ret);
  //si::fixVariableReferences(ret);
  return ret;
}
  
SgNode* 
XevXmlVisitor::visitSgPragmaDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgPragmaDeclaration* ret = 0;
  SgPragma* pr = 0;

  SUBTREE_VISIT_BEGIN(node,child,0)
    {
      if(pr==0)
	pr = isSgPragma(child);
    }
  SUBTREE_VISIT_END();

  if(pr) {
    ret = sb::buildPragmaDeclaration(pr->get_pragma()); 
  }
  else XEV_ABORT();
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgPragma(xe::DOMNode* node, SgNode* astParent)
{
  SgPragma* ret = 0;
  string line;

  if(XmlGetAttributeValue(node,"pragma",&line)){
    std::string tmp;
    for(size_t i(0);i<line.size();i++){
      if( line[i] != '\\')
	tmp+=line[i];
      else if(line[i+1] !='"')
	tmp+=line[i];
    }

    ret = sb::buildPragma(tmp);
  }
  else XEV_ABORT();
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgVariableDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgVariableDeclaration*                    ret  = 0;
  SgInitializedName*                        name = 0;
  SgInitializedName*                        tmp  = 0;
  //SgClassDeclaration*                       cls  = 0;
  SgDeclarationStatement*                   cls  = 0;
  Rose_STL_Container<SgInitializedName*>    varList;
  unsigned long     storage=0U;
  string            bitstr;
  unsigned long     bit = 0;

  XmlGetAttributeValue(node,"modifier",&storage);
  XmlGetAttributeValue(node,"bitfield",&bitstr);

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      tmp = isSgInitializedName(astchild);
      if( tmp ) {
        varList.push_back( tmp );
        if(name==0) name = tmp;
	else tmp->set_scope(name->get_scope());
      }

      if( cls==0 ) {
        cls = isSgDeclarationStatement(astchild);
      }
    }
  SUBTREE_VISIT_END();


  if(name) {
    if( cls ) {                                         
      SgType * typ = name->get_type();
      SgNamedType *namedType = isSgNamedType(typ->findBaseType());
      cls->set_parent( typ );
      namedType->get_declaration()->set_definingDeclaration(cls);
      namedType->set_declaration (cls);
      ret = sb::buildVariableDeclaration(name->get_name(), 
					 namedType,
					 name->get_initializer());
      
      ret->set_baseTypeDefiningDeclaration( cls );
    }
    else
      ret = sb::buildVariableDeclaration(name->get_name(), 
					 name->get_type(),
					 name->get_initializer());
    if(ret==0) XEV_ABORT();
    ret->set_parent(astParent);
    ret->set_definingDeclaration(ret);
    // see buildVariableDeclaration in fortran_support.C 
    ret->set_variableDeclarationContainsBaseTypeDefiningDeclaration(false);

    if( varList.size() > 0 && si::is_Fortran_language() ) 
      // NOTE: in the case of Fortran, append_statement does not work? 
      // So this is necessary to define the second variable and later.
      ret->get_variables() = varList;
    for(size_t i(0);i<varList.size();i++){
      if(i>0 && si::is_Fortran_language() == false )
	ret->append_variable(varList[i],varList[i]->get_initializer());
      varList[i]->set_parent(ret);
      varList[i]->set_declptr(ret);
      varList[i]->set_scope(name->get_scope());
      //varList[i]->set_type(name->get_type());
    }

    // this is necessary because declaration is required for each variable.
    for(size_t i(1);i<varList.size();i++){
      sb::buildVariableDeclaration(varList[i]->get_name(), 
				   varList[i]->get_type(),
				   varList[i]->get_initializer());
    }
  }
  else XEV_ABORT();
#if 1
  bool isFortranParameter = false;
  // See buildVariableDeclaration
  if (ret->get_scope() && si::is_Fortran_language()){
    SgFunctionDefinition * f_def = si::getEnclosingProcedure (ret->get_scope());
    /* variables in a function parameter list are moved? */
    if (f_def != NULL){
      for(size_t i(0);i<varList.size();i++){
	name = varList[i];
	SgSymbolTable * st = f_def->get_symbol_table();
	SgVariableSymbol * v_symbol = st->find_variable(name->get_name());
	if (v_symbol != NULL){
	  SgInitializedName *default_initName = ret->get_decl_item (name->get_name());
	  SgInitializedName * new_initName = v_symbol->get_declaration();
	  SgInitializedNamePtrList&  n_list= ret->get_variables();
	  std::replace (n_list.begin(), n_list.end(),default_initName, new_initName );
	  XEV_WARN("replace:" << name->get_name());
	  SgNode * old_parent = new_initName->get_parent();
	  if(old_parent==0 || isSgFunctionParameterList(old_parent)==0) XEV_ABORT();
	  new_initName->set_parent(ret); 
	  SgVariableDefinition * var_def = isSgVariableDefinition(default_initName->get_declptr()) ;
	  var_def->set_parent(new_initName);
	  var_def->set_vardefn(new_initName);
	  new_initName->set_declptr(var_def);
	  delete (default_initName);
	  isFortranParameter = true;
	}
      }
    }
  }
  if (! isFortranParameter)
    si::fixVariableDeclaration(ret,ret->get_scope());
#endif

#if 0
  // Initialize SgAccessModifier (2014.04.16)
  ret->get_declarationModifier().get_accessModifier().setUndefined();

  if(storage & (1U<<0) )
    ((ret->get_declarationModifier()).get_storageModifier()).setUnknown();
  if(storage & (1U<<1) )
    ((ret->get_declarationModifier()).get_storageModifier()).setStatic();
  if(storage & (1U<<2) )
    ((ret->get_declarationModifier()).get_storageModifier()).setExtern();
  if(storage & (1U<<3) )
    ((ret->get_declarationModifier()).get_storageModifier()).setAuto();
  if(storage & (1U<<4) )
    ((ret->get_declarationModifier()).get_storageModifier()).setRegister();
  if(storage & (1U<<5) )
    ((ret->get_declarationModifier()).get_storageModifier()).setTypedef();
  if(storage & (1U<<6) )
    ret->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setConst();
  if(storage & (1U<<7) )
    ret->get_declarationModifier().get_typeModifier().setAllocatable();
  if(storage & (1U<<8) )
    ret->get_declarationModifier().get_typeModifier().setAsynchronous();
  if(storage & (1U<<9) )
    ret->get_declarationModifier().get_typeModifier().setIntent_in();
  if(storage & (1U<<10) )
    ret->get_declarationModifier().get_typeModifier().setIntent_out();
  if(storage & (1U<<11) )
    ret->get_declarationModifier().get_typeModifier().setIntent_inout();
  if(storage & (1U<<12) )
    ret->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setVolatile();
  if(storage & (1U<<13) )
    ret->get_declarationModifier().get_storageModifier().setExtern();
  if(storage & (1U<<14) ) 
    ret->get_declarationModifier().get_accessModifier().setPublic();
  if(storage & (1U<<15) )
    ret->get_declarationModifier().get_accessModifier().setPrivate();
  if(storage & (1U<<16) )
    ret->get_declarationModifier().get_typeModifier().setIntrinsic();
  if(storage & (1U<<17) )
    ret->get_declarationModifier().get_typeModifier().setOptional();
  if(storage & (1U<<18) )
    ret->get_declarationModifier().get_typeModifier().setSave();
  if(storage & (1U<<19) )
    ret->get_declarationModifier().get_typeModifier().setTarget();
  if(storage & (1U<<20) )
    ret->get_declarationModifier().get_typeModifier().setValue();
  if(storage==0)
    ((ret->get_declarationModifier()).get_storageModifier()).setDefault();
  //checkPreprocInfo(node,ret);
#endif
  // set bitfield (2013.08.06)
  if( bitstr.size() ) {
    bit = strtoul( bitstr.c_str(),0,0 );
    SgUnsignedLongVal* val = new SgUnsignedLongVal( bit,bitstr );
    val->set_startOfConstruct( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
    ret->set_bitfield (val);
    val->set_parent(ret);
  }
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgFunctionDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgFunctionDeclaration*   ret = 0;
  SgFunctionParameterList* lst = 0;
  SgFunctionDefinition*    def = 0;
  //SgBasicBlock*            def = 0;
  SgType*                  typ = 0;

  string name;

  if( XmlGetAttributeValue(node,"name",&name) == false )
    XEV_ABORT();

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(lst==0)
	lst = isSgFunctionParameterList(astchild);
      if(def==0)
	def = isSgFunctionDefinition(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
  SUBTREE_VISIT_END();

  if(lst) {
    if(def)
      ret 
	= sb::buildDefiningFunctionDeclaration(SgName(name.c_str()), typ, lst);
    else 
      ret 
	= sb::buildNondefiningFunctionDeclaration(SgName(name.c_str()), typ, lst);
  }
  else XEV_ABORT();

  if( def == 0 ) {
    Sg_File_Info* info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
    info->setOutputInCodeGeneration();
    def = new SgFunctionDefinition(info);
    ret->get_declarationModifier().get_storageModifier().setExtern();
  }
  lst->set_parent(ret);
  def->set_parent(ret);
  ret->set_definition(def);
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgProcedureHeaderStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgProcedureHeaderStatement*     ret = 0;
  SgFunctionType*                 typ = 0;
  SgFunctionParameterList*        lst = 0;
  SgScopeStatement*               scope = 0;
  SgBasicBlock*                   def = 0;
  SgFunctionDefinition*           fdf = 0;
  SgInitializedName*              var = 0;

  string                          name,rname;
  int                             kind=0;
  bool                            f_pure  = false;
  bool                            f_elem  = false;
  bool                            f_recur = false;
  Sg_File_Info* info = DEFAULT_FILE_INFO;
  info->setOutputInCodeGeneration();

  XmlGetAttributeValue(node,"name",&name);
  XmlGetAttributeValue(node,"subprogram_kind",&kind);
  XmlGetAttributeValue(node,"recursive",&f_recur);
  XmlGetAttributeValue(node,"pure",&f_pure);
  XmlGetAttributeValue(node,"elemental",&f_elem);

  // the scope must be SgGlobal of this SgProject.
  //scope = isSgScopeStatement(_file->get_globalScope());
  scope = sb::topScopeStack();
  while(scope && scope->containsOnlyDeclarations()==false){
    //cerr << scope->class_name() <<"->";;
    scope = isSgStatement(scope->get_parent())->get_scope();
    //cerr << scope->class_name() << endl;
  }
  xe::DOMNode* cld_ = (node)->getFirstChild();
  while(cld_) {
    if(cld_->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){
      char* buf = xe::XMLString::transcode(cld_->getNodeName());
      string nname = buf;
      xe::XMLString::release(&buf);
      if(nname != "SgFunctionDefinition" && nname != "SgBasicBlock"){
	SgNode* astchild = this->visit(cld_,0);
	if(typ==0)
	  typ = isSgFunctionType(astchild);
	if(lst==0)
	  lst = isSgFunctionParameterList(astchild);
	if(var==0)
	  var = isSgInitializedName(astchild);
      }
    }
    cld_=cld_->getNextSibling();
  }
  
  if(lst && typ){
    if( kind != SgProcedureHeaderStatement::e_block_data_subprogram_kind ){
      ret = sb::buildProcedureHeaderStatement( (const char*)(name.c_str()), 
					       typ->get_return_type(), lst,
					       (SgProcedureHeaderStatement::subprogram_kind_enum)kind, scope);
    }
    else {
      // add (block data) 0828
      SgFunctionType* functionType = new SgFunctionType(SgTypeVoid::createType(), false);
      ret = new SgProcedureHeaderStatement( info,SgName(name), functionType,  fdf/*=NULL*/ );
      ret->set_type(functionType);
      ret->set_subprogram_kind( (SgProcedureHeaderStatement::subprogram_kind_enum)kind );
    }
    lst->set_parent(ret);
  }
  else XEV_ABORT();

  if(var){
    var->set_parent(ret);
    var->set_scope(scope); // is this OK??
    //var->set_scope(sb::topScopeStack());
  }
  ret->set_scope(scope); 
  ret->set_parent(astParent);

  cld_ = (node)->getFirstChild();
  while(cld_) {
    if(cld_->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){
      char* buf = xe::XMLString::transcode(cld_->getNodeName());
      string nname = buf;
      xe::XMLString::release(&buf);
      if(nname == "SgFunctionDefinition" || nname == "SgBasicBlock"){
	SgNode* astchild = this->visit(cld_,ret);
	if(fdf==0)
	  fdf = isSgFunctionDefinition(astchild);
	// for block data
	if(kind == SgProcedureHeaderStatement::e_block_data_subprogram_kind && def==0) 
	  def = isSgBasicBlock(astchild);
      }
    }
    cld_=cld_->getNextSibling();
  }
  if( kind == SgProcedureHeaderStatement::e_block_data_subprogram_kind ){
    fdf = new SgFunctionDefinition( info,def );
    def->set_parent(fdf);
    fdf->set_parent(ret);
  }

  if(fdf){
    //si::replaceStatement( ret->get_definition()->get_body(),def,true );
    ret->set_definition(fdf);
    fdf->set_declaration(ret);
    fdf->set_parent(ret);
    VardefSearch search(name);
    SgInitializedName* ini = isSgInitializedName(search.visit(fdf));
    if(ini){
      ini->set_declptr(ret);
      // This will remove type description before the function name
      ret->get_type()->set_return_type(SgTypeVoid::createType());
    }
    var->set_scope(fdf);

#if 1
    SgBasicBlock* body = fdf->get_body();
    XEV_ASSERT(body!=NULL);
    if(lst && si::is_Fortran_language()==true){
      for(size_t i(0);i<lst->get_args().size();i++){
	ini = lst->get_args()[i];
	ini->set_parent(fdf);
	XEV_ASSERT(body!=NULL);
	SgVariableSymbol* sym = body->lookup_variable_symbol(ini->get_name());
	if(sym!=NULL){
	  ini->set_scope(body);
	  ini->set_type(sym->get_type());
	  ini->set_declptr(sym->get_declaration()->get_declptr());
	}
	else{
	  SgVariableSymbol* vsym = new SgVariableSymbol(ini);
	  fdf->insert_symbol(ini->get_name(),vsym);
	  ini->set_scope(fdf);
	  //ini->set_type(sym->get_type());
	}
      }
    } // if lst 
#endif
    if( isSgClassType(typ->get_return_type()) ){
      XEV_WARN("hoge");
      SgClassType* ctyp = isSgClassType(typ->get_return_type());
      SgClassSymbol* csym 
	= si::lookupClassSymbolInParentScopes(ctyp->get_name(),body);
      body->get_symbol_table()->print();
      if(csym!=NULL){
	XEV_WARN("hoge");
	ctyp->set_declaration(csym->get_declaration());
      }
    }
  }
  else XEV_ABORT();
  
  if(f_pure)
    ret->get_functionModifier().setPure();
  if(f_elem)
    ret->get_functionModifier().setElemental();
  if(f_recur)
    ret->get_functionModifier().setRecursive();
  ret->set_parent(astParent);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgFunctionParameterList(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFunctionParameterList* ret = sb::buildFunctionParameterList();
  SgInitializedName* ini=0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if((ini = isSgInitializedName(astchild)) != NULL ){
	si::appendArg(ret,ini);
	ini->set_scope(sb::topScopeStack());
      }
    }
  SUBTREE_VISIT_END();
  ret->set_parent(astParent);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgBasicBlock(xe::DOMNode* node, SgNode* astParent)
{
  SgBasicBlock* ret = sb::buildBasicBlock();
  //SgScopeStatement* scope = sb::topScopeStack();
  ret->set_parent(astParent);
  //if(si::is_Fortran_language()==false)
  sb::pushScopeStack(ret);
  //sb::pushScopeStack(sb::topScopeStack());
  traverseStatementsAndTexts(this, node,ret);
  //if(si::is_Fortran_language()==false)
  sb::popScopeStack();
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgExprStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgExpression* exp = 0;
  SgExprStatement* ret = sb::buildExprStatement(exp);
  //SgExprStatement* ret = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(exp==0)
	exp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_expression(exp);
  ret->set_parent(astParent);
  exp->set_parent(ret);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgForStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForStatement* ret = 0;
  SgStatement*    ini = 0;
  SgStatement*    tst = 0;
  SgExpression*   inc = 0;
  SgStatement*    bdy = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      /* assuming these stmts appear in this order */
      if(ini==0)
	ini = isSgStatement(astchild);
      else if (tst==0)
	tst = isSgStatement(astchild);
      else if (inc==0)
	inc = isSgExpression(astchild);
      else if (bdy==0)
	bdy = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildForStatement(ini,tst,inc,bdy);
  ret->set_parent(astParent);
  if(ini)ini->set_parent(ret);
  if(tst)tst->set_parent(ret);
  if(inc)inc->set_parent(ret);
  if(bdy)bdy->set_parent(ret);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgForInitStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForInitStatement* ret = 0;
  SgStatement* stmt  = 0;
  SgStatementPtrList lst;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if((stmt = isSgStatement(astchild))!=0)
	lst.push_back(stmt);
    }
  SUBTREE_VISIT_END();

  //lst could be empty
  ret = sb::buildForInitStatement(lst);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgIfStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgIfStmt* ret = 0;
  SgStatement* tstmt  = 0;
  SgStatement* fstmt  = 0;
  SgExprStatement* cond  = 0;
  int estmt = 1;
  int ukey  = 1;

  XmlGetAttributeValue(node,"use",&ukey);
  XmlGetAttributeValue(node,"end",&estmt);
  
  ret = new SgIfStmt(DEFAULT_FILE_INFO);
  ret->setCaseInsensitive(true);
  ret->set_parent(astParent);
  sb::pushScopeStack(ret);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      /* assuming these stmts appear in this order */
      if(cond==0)
	cond = isSgExprStatement(astchild);
      else if (tstmt==0)
	tstmt = isSgStatement(astchild);
      else if (fstmt==0)
	fstmt = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();
  sb::popScopeStack();

  ret->set_conditional(cond);
  ret->set_true_body(tstmt);
  ret->set_false_body(fstmt);

  //ret = sb::buildIfStmt(cond,tstmt,fstmt);
  ret->set_has_end_statement(estmt);
  ret->set_use_then_keyword(ukey);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgWhileStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgWhileStmt*          ret   = 0;
  SgExprStatement*      cond  = 0;
  SgStatement*          tstmt = 0;
  SgStatement*          fstmt = 0;
  int enddo=0;
  int nlabel=0;
  string slabel;

  XmlGetAttributeValue(node,"end",&enddo);
  XmlGetAttributeValue(node,"slabel",&slabel);
  XmlGetAttributeValue(node,"nlabel",&nlabel);

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      /* assuming these stmts appear in this order */
      if(cond==0)
        cond = isSgExprStatement(astchild);
      else if (tstmt==0)
        tstmt = isSgStatement(astchild);
      else if (fstmt==0)
        fstmt = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildWhileStmt(cond,tstmt,fstmt);
  if(cond)  cond->set_parent(ret);
  if(tstmt) tstmt->set_parent(ret);
  if(fstmt) fstmt->set_parent(ret);

  if(enddo){
    ret->set_has_end_statement(true);
  }
  if( slabel.size() )
    ret->set_string_label( slabel );
  else if( nlabel) {
    SgLabelSymbol*  s = new SgLabelSymbol();
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    s->get_fortran_statement()->set_parent(s);
    s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    s->set_numeric_label_value( nlabel );
    SgLabelRefExp*  l = new SgLabelRefExp( s );
    ret->set_end_numeric_label( l );
  }

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgDoWhileStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDoWhileStmt*        ret  = 0;
  SgStatement*          body = 0;
  SgExprStatement*      cond = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      /* assuming these stmts appear in this order */
      if(body==0)
        body = isSgStatement(astchild);
      else if (cond==0)
        cond = isSgExprStatement(astchild);
    }
  SUBTREE_VISIT_END();
  ret = sb::buildDoWhileStmt(body,cond);
  if(cond)  cond->set_parent(ret);
  if(body)  body->set_parent(ret);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgConditionalExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgConditionalExp*     ret   = 0;
  SgExpression*         cond  = 0;
  SgExpression*         tstmt = 0;
  SgExpression*         fstmt = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      /* assuming these stmts appear in this order */
      if(cond==0)
        cond = isSgExpression(astchild);
      else if (tstmt==0)
        tstmt = isSgExpression(astchild);
      else if (fstmt==0)
        fstmt = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildConditionalExp(cond,tstmt,fstmt);
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgSizeOfOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgSizeOfOp*           ret = 0;
  SgExpression*         exp = 0;
  SgType*               typ = 0;
  string class_name;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(exp==0)
	exp = isSgExpression(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
  SUBTREE_VISIT_END();
  
  if( typ )
   ret = sb::buildSizeOfOp( typ );
  else
   ret = sb::buildSizeOfOp( exp );

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgSwitchStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgSwitchStatement*    ret  = 0;
  SgStatement*          item = 0;
  SgStatement*          body = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      // assuming the order
      if(item==0)
        item = isSgStatement(astchild);
      else if (body==0)
        body = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildSwitchStatement(item,body);
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgCaseOptionStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgCaseOptionStmt*     ret  = 0;
  SgExpression*         key  = 0;
  SgStatement*          body = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(key==0)
        key = isSgExpression(astchild);
      else if (body==0)
        body = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildCaseOptionStmt(key,body);
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgBreakStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  string do_label;
  SgBreakStmt* ret = 0;
  ret = sb::buildBreakStmt();

  if(XmlGetAttributeValue(node,"label",&do_label)){
    ret->set_do_string_label(do_label);
  }
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgContinueStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  string do_label;
  SgContinueStmt* ret = sb::buildContinueStmt();

  if(XmlGetAttributeValue(node,"label",&do_label)){
    ret->set_do_string_label(do_label);
  }

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgDefaultOptionStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDefaultOptionStmt*      ret  = 0;
  SgStatement*              body = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if (body==0)
        body = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildDefaultOptionStmt(body);
  
  return ret;
}




#define VISIT_BINARY_OP(op)						\
  SgNode*								\
  XevXmlVisitor::visitSg##op(xercesc::DOMNode* node, SgNode* astParent) \
  {									\
    SgExpression* lhs = 0;						\
    SgExpression* rhs = 0;						\
    xe::DOMNode* child=node->getFirstChild();				\
    Sg##op* ret = sb::build##op(lhs,rhs);				\
    ret->set_parent(astParent);						\
    while(child) {							\
      if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){	\
	SgNode* astchild = this->visit(child,ret);			\
	if(lhs==0)							\
	  lhs = isSgExpression(astchild);				\
	else if(rhs==0)							\
	  rhs = isSgExpression(astchild);				\
      }									\
      child=child->getNextSibling();					\
    }									\
    if( lhs && rhs ){							\
      ret->set_lhs_operand(lhs);					\
      ret->set_rhs_operand(rhs);					\
      return ret;							\
    }									\
    else {								\
      XEV_ABORT();								\
    }									\
  }									



#define VISIT_UNARY_OP(op)						\
  SgNode*								\
  XevXmlVisitor::visitSg##op(xercesc::DOMNode* node, SgNode* astParent) \
  {									\
    SgExpression* exp = 0;						\
    int imode = 0;							\
    XmlGetAttributeValue(node,"mode",&imode);				\
    xe::DOMNode* child=node->getFirstChild();				\
    while(child) {							\
      if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){	\
	SgNode* astchild = this->visit(child);				\
	if(exp==0)							\
	  exp = isSgExpression(astchild);				\
      }									\
      child=child->getNextSibling();					\
    }									\
    if( exp ) {								\
      SgNode* ret = sb::build##op(exp,					\
				  imode?SgUnaryOp::postfix:SgUnaryOp::prefix); \
      ret->set_parent(astParent);					\
      return ret;							\
    }									\
    else								\
      XEV_ABORT();								\
  }									
  
#define VISIT_UNARY_P1_OP(op)						\
  SgNode*								\
  XevXmlVisitor::visitSg##op(xercesc::DOMNode* node, SgNode* astParent) \
  {									\
    SgExpression* exp = 0;						\
    xe::DOMNode* child=node->getFirstChild();				\
    while(child) {							\
      if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){	\
	SgNode* astchild = this->visit(child);				\
	if(exp==0)							\
	  exp = isSgExpression(astchild);				\
      }									\
      child=child->getNextSibling();					\
    }									\
    if( exp )								\
      return sb::build##op(exp);					\
    else								\
      XEV_ABORT();								\
  }									



VISIT_UNARY_OP(PlusPlusOp);
VISIT_UNARY_OP(MinusOp);
VISIT_UNARY_OP(MinusMinusOp);

VISIT_UNARY_P1_OP(NotOp);
VISIT_UNARY_P1_OP(BitComplementOp);
VISIT_UNARY_P1_OP(AddressOfOp);
VISIT_UNARY_P1_OP(UnaryAddOp);

VISIT_BINARY_OP(AssignOp);
VISIT_BINARY_OP(PlusAssignOp);
VISIT_BINARY_OP(MultAssignOp);
VISIT_BINARY_OP(AddOp);
VISIT_BINARY_OP(LessThanOp);

VISIT_BINARY_OP(GreaterThanOp);
VISIT_BINARY_OP(LessOrEqualOp);
VISIT_BINARY_OP(GreaterOrEqualOp);
VISIT_BINARY_OP(NotEqualOp);
VISIT_BINARY_OP(OrOp);
VISIT_BINARY_OP(EqualityOp);
VISIT_BINARY_OP(AndOp);
VISIT_BINARY_OP(ModOp);
VISIT_BINARY_OP(DivideOp);
VISIT_BINARY_OP(MultiplyOp);
VISIT_BINARY_OP(MinusAssignOp);
VISIT_BINARY_OP(IorAssignOp);
VISIT_BINARY_OP(AndAssignOp);
VISIT_BINARY_OP(XorAssignOp);
VISIT_BINARY_OP(LshiftOp);
VISIT_BINARY_OP(RshiftOp);
VISIT_BINARY_OP(SubtractOp);
VISIT_BINARY_OP(DivAssignOp);
VISIT_BINARY_OP(ModAssignOp);
VISIT_BINARY_OP(BitAndOp);
VISIT_BINARY_OP(BitOrOp);
VISIT_BINARY_OP(BitXorOp);
VISIT_BINARY_OP(CommaOpExp);
//VISIT_BINARY_OP(PntrArrRefExp);
VISIT_BINARY_OP(ExponentiationOp);
VISIT_BINARY_OP(LshiftAssignOp);
VISIT_BINARY_OP(RshiftAssignOp);
VISIT_BINARY_OP(ConcatenationOp);

SgNode* 
XevXmlVisitor::visitSgNullExpression(xe::DOMNode* node, SgNode* astParent)
{
  SgNullExpression* ret = sb::buildNullExpression();
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgNullStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgNullStatement* ret = sb::buildNullStatement();
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgVarRefExp(xe::DOMNode* node, SgNode* astParent)
{
  string       name;
  SgVarRefExp* ret=0;
  SgScopeStatement* scope = sb::topScopeStack();

  if(XmlGetAttributeValue(node,"name",&name)==false)
    XEV_ABORT();

  SgVariableSymbol* vsym =0;
  if(isSgClassDefinition(scope)){
    vsym=scope->lookup_variable_symbol(name);
    //scope->get_symbol_table()->print();
  }
  else 
    vsym = si::lookupVariableSymbolInParentScopes(name);

#if XEV_DEBUG
  // debugging symbol tables
  if(vsym){
    cerr << vsym->get_name().getString() <<" is found in the symbol table" << endl;
    cerr << "type is " << vsym->get_type()->class_name() << endl;
  }
  else 
    cerr << name  <<" is NOT found in the symbol table" << endl;

  while(scope!=NULL){
    cerr << scope->class_name();
    if(isSgClassDefinition(scope))
      cerr << "(" 
	   << isSgClassDefinition(scope)->get_declaration()->get_name().getString()
	   << ")";
    
    cerr << "->";
    if(scope->get_parent() != NULL ){
      scope = isSgGlobal(scope)?NULL:scope->get_scope();
    }
    else{
      cerr << "(parent is not set)";
      scope = NULL;
    }
  }
  cerr << "NULL" << endl;
  scope = sb::topScopeStack();
#endif

  if(vsym==NULL){
    // NOTE: See sb::buildVarRefExp() in sageBuilder.C.
    // There is a symbol but not a variable symbol.
    // In this situation, buildVarRefExp() will crash.
    // So a dummy variable is created as a placeholder.
    // This will be fixed later when fixVariablereferences is called.
    SgFunctionSymbol* fsym  = si::lookupFunctionSymbolInParentScopes(name);
    if(fsym){
      SgFunctionType* ftype = isSgFunctionType(fsym->get_type());
      if(ftype==0) XEV_ABORT();
      SgInitializedName * name1 
	= sb::buildInitializedName(name,ftype->get_return_type());
      name1->set_scope(scope); //buildInitializedName() does not set scope for various reasons
      vsym= new SgVariableSymbol(name1);
      vsym->set_parent(scope);
      ret = new SgVarRefExp(vsym);
      ret->set_symbol(vsym);
      ret->get_symbol()->set_declaration(name1);
      si::setOneSourcePositionForTransformation(ret);
    }
    else {
      SgInitializedName * name1 =0;
      if (isSgClassDefinition(scope)){
	// unknown class member variables
	// -- take a bottomup approach to building this AST
	name1 = sb::buildInitializedName(name,SgTypeUnknown::createType());
      }
      else{
	// implicit variables
	scope = si::getEnclosingProcedure (sb::topScopeStack());
	if(scope==NULL) scope = _file->get_globalScope();
	name1  = sb::buildInitializedName(name,generateImplicitType(name));
      }
      name1->set_scope(scope); 
      vsym= new SgVariableSymbol(name1);
      vsym->set_parent(scope);
      ret = new SgVarRefExp(vsym);
      ret->set_symbol(vsym);
      ret->get_symbol()->set_declaration(name1);
      si::setOneSourcePositionForTransformation(ret);
    }
  }
  //vsym = si::lookupVariableSymbolInParentScopes(name);
  //if(vsym==0)XEV_ABORT();
  else {
    ret= sb::buildVarRefExp(name);
  }
  ret->set_parent(astParent);

  //if(isSgClassDefinition(scope))
  //sb::popScopeStack(); //pushed in visitSgDotExp()

#if 0
  cerr << ret->get_symbol()->get_name().getString() << "==" << name << endl
       << ret->get_symbol()->get_type()->class_name() <<  endl
       << ret->get_symbol()->get_scope()->class_name() << "==" << sb::topScopeStack()->class_name() 
       << endl;
#endif

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgCastExp(xe::DOMNode* node, SgNode* astParent)
{
  SgCastExp*     ret   = 0;
  SgType*        typ   = 0;
  SgExpression*  exp   = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(exp==0)
	exp = isSgExpression(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
  SUBTREE_VISIT_END();

  if(typ && exp){
    ret = sb::buildCastExp(exp,typ);
    ret->set_parent(astParent);
  }
  else if(exp){
    // ignore implicit type conversions
    exp->set_parent(astParent);
    return exp;
  }
  else 
    XEV_ABORT();

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgColonShapeExp(xe::DOMNode* node, SgNode* astParent)
{

#if 0
  SgType*        typ   = 0;
  SgExpression*  exp   = 0;
  xe::DOMNode*   child = node->getFirstChild();

  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if(exp==0)
	exp = isSgExpression(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
    child=child->getNextSibling();
  } 
  if(typ && exp)
    return sb::buildCastExp(exp,typ);
  else 
    XEV_ABORT();
#endif
  SgExpression * ret = new SgColonShapeExp();
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgReturnStmt(xe::DOMNode* node, SgNode* astParent)
{
  SgReturnStmt*        ret   = 0;
  SgExpression*        exp   = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(exp==0)
	exp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildReturnStmt(exp);

  return ret;
}

/* NOTE: this function returns SgBasicBlock */
SgNode* 
XevXmlVisitor::visitSgFunctionDefinition(xe::DOMNode* node, SgNode* astParent)
{
  //SgBasicBlock*   ret   = 0;
  SgBasicBlock* blk = 0;
  Sg_File_Info* info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  info->setOutputInCodeGeneration();
  SgFunctionDefinition* ret  = new SgFunctionDefinition(info);
  ret->set_parent(astParent);
  sb::pushScopeStack(ret);
  if(si::is_Fortran_language())
    ret->setCaseInsensitive(true);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(blk == 0 ){
	blk = isSgBasicBlock(astchild);
      }
    }
  SUBTREE_VISIT_END();
  if(blk) 
    ret->set_body(blk);
  sb::popScopeStack();
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgInitializedName(xe::DOMNode* node, SgNode* astParent)
{
  SgInitializedName* ret = 0;
  SgInitializer*     ini = 0;
  SgType*            typ = 0;
  
  string               name;
  
  XmlGetAttributeValue(node,"name",&name);

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(ini==0)
	ini = isSgInitializer(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
  SUBTREE_VISIT_END();

//  if(isSgArrayType(typ))
  //  typ = isSgArrayType(typ)->get_base_type();
  ret = sb::buildInitializedName(name.c_str(),typ,ini);
  ret->set_parent(astParent);
  ret->set_scope(sb::topScopeStack());// This was NG for s009 but needed by s005
  if(typ) {
    typ->set_parent(ret); // this must be true
  }
  if(ini) ini->set_parent(ret);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgAssignInitializer(xe::DOMNode* node, SgNode* astParent)
{
  SgAssignInitializer* ret = 0;
  SgExpression*        exp = 0;
  SgType*              typ = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)  
    {
      if(exp==0)
	exp = isSgExpression(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
  SUBTREE_VISIT_END();

  if(exp && typ){
    ret = sb::buildAssignInitializer(exp,typ);
    exp->set_parent(ret);
    typ->set_parent(ret);
  }
  else XEV_ABORT();
  
  return ret;
}

#define VISIT_VAL(valType,baseType,buildVal)				\
  SgNode* XevXmlVisitor::						\
  visit##valType(xe::DOMNode* node, SgNode* astParent)			\
  {									\
    valType* ret = 0;							\
    baseType ival;							\
    std::string vstr;							\
    if(XmlGetAttributeValue(node,"value",&ival)==false) XEV_ABORT();	\
    ret = sb::build##buildVal(ival);					\
    ret->set_parent(astParent);					        \
    if(XmlGetAttributeValue(node,"string",&vstr))			\
      ret->set_valueString(vstr);					\
    return ret;								\
  }

#define VISIT_VAL_NO_STRING(valType,baseType,buildVal)			\
  SgNode* XevXmlVisitor::						\
  visit##valType(xe::DOMNode* node, SgNode* astParent)			\
  {									\
    valType* ret = 0;							\
    baseType ival;							\
    std::string vstr;							\
    if(XmlGetAttributeValue(node,"value",&ival)==false) XEV_ABORT();	\
    ret = sb::build##buildVal(ival);					\
    ret->set_parent(astParent);					        \
    return ret;								\
  }


VISIT_VAL_NO_STRING(SgBoolValExp,bool,BoolValExp);
VISIT_VAL(SgCharVal,char,CharVal);
VISIT_VAL(SgWcharVal,unsigned short,WcharVal);
//VISIT_VAL(SgComplexVal);
VISIT_VAL(SgDoubleVal,double,DoubleVal);
VISIT_VAL(SgFloatVal,float,FloatVal);
VISIT_VAL(SgIntVal,int,IntVal);
VISIT_VAL(SgLongIntVal,long int,LongIntVal);
VISIT_VAL(SgLongLongIntVal,long long int,LongLongIntVal);
//VISIT_VAL(SgEnumVal);
VISIT_VAL(SgLongDoubleVal,long double,LongDoubleVal);
VISIT_VAL(SgShortVal,short,ShortVal);
//VISIT_VAL(SgStringVal,string,StringVal);
VISIT_VAL(SgUnsignedCharVal,unsigned char,UnsignedCharVal);
VISIT_VAL(SgUnsignedShortVal,unsigned short,UnsignedShortVal);
VISIT_VAL(SgUnsignedIntVal,unsigned int, UnsignedIntVal);
VISIT_VAL(SgUnsignedLongVal,unsigned long, UnsignedLongVal);
VISIT_VAL(SgUnsignedLongLongIntVal,unsigned long long,UnsignedLongLongIntVal);

SgNode* 
XevXmlVisitor::visitSgLabelStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgLabelStatement*         ret   = 0;
  SgStatement*              body  = 0;
  SgScopeStatement*         scope = 0;
  
  //xe::DOMNamedNodeMap* amap = node->getAttributes();
  //xe::DOMNode* nameatt = 0;
  string                    slabel,nlabel;
  stringstream              val;
  int                       ino   = 0;

  XmlGetAttributeValue(node,"slabel",&slabel);
  XmlGetAttributeValue(node,"nlabel",&nlabel);

  SUBTREE_VISIT_BEGIN(node,astchild,0)  
    {
      if (body==0)
        body = isSgStatement(astchild);
      if(scope==0)
        scope = isSgScopeStatement(astchild);
    }
  SUBTREE_VISIT_END();  

  if( nlabel.size() ){
    ret = sb::buildLabelStatement( nlabel.c_str(), body, scope );
    SgLabelSymbol*  s = new SgLabelSymbol();
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    s->get_fortran_statement()->set_parent(s);
    //s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    s->set_label_type(SgLabelSymbol::e_start_label_type);
    val << nlabel;
    val >> ino;
    s->set_numeric_label_value( ino );
    SgLabelRefExp*  l = new SgLabelRefExp( s );
    ret->set_numeric_label( l );
  }
  /*
  else if( slabel.size() ){
    ret = sb::buildLabelStatement( slabel.c_str(), body, scope );
  }
  else XEV_ABORT();
  */
  else // slabel is allowed to be empty
    ret = sb::buildLabelStatement( slabel.c_str(), body, scope );

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgGotoStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgGotoStatement*      ret   = 0;
  SgLabelStatement*     label = 0;
  SgStatement*          body  = 0;
  SgScopeStatement*     scope = 0;
  SgLabelSymbol*        s     = 0;
  SgLabelRefExp*        l     = 0;

  string                s_name,n_name;
  int                   ino = 0;

  if(XmlGetAttributeValue(node,"s_label",&s_name))
    label = sb::buildLabelStatement( s_name.c_str(), body, scope );
  if(XmlGetAttributeValue(node,"n_label",&n_name)){
    label = sb::buildLabelStatement( n_name.c_str(), body, scope );
    s = new SgLabelSymbol();
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    s->set_label_type(SgLabelSymbol::e_start_label_type);
    // s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    ino = atoi(n_name.c_str());
    s->set_numeric_label_value( ino );
    l = new SgLabelRefExp( s );
    s->set_parent(l);
    label->set_numeric_label( l );
  }

  if( n_name.size() ){
    /**/
    ret = sb::buildGotoStatement( label );
    ret->set_label_expression(l);

    //printf( "l=%p\n",l );
    l->set_parent(ret);
    /* */
    /*
    ret= new SgGotoStatement(label);
    //SgLabelRefExp* labelRefExp = new SgLabelRefExp(s);
        ret->set_label_expression(l);
        l->set_parent(ret);
    */
    //printf( "get_label_expression=%p\n",ret->get_label_expression() );

  }
  else
    ret = sb::buildGotoStatement( label );
  
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgTypedefType(xe::DOMNode* node, SgNode* astParent)
{
  SgType* ret = 0;
  SgScopeStatement* scope = sb::topScopeStack();    //?
  string name;

  if( XmlGetAttributeValue(node,"type_name",&name) ){
    ret = sb::buildOpaqueType( name.c_str(), scope );
  }
  else XEV_ABORT();

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgExprListExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgExprListExp*                ret = 0;
  SgExpression*                 exp = 0;
  std::vector< SgExpression * > exprs;

  ret = sb::buildExprListExp( exprs );
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret) 
    {
      if((exp = isSgExpression(astchild))!=0) {
	exp->set_parent(ret);
	exp->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
	ret->append_expression(exp);
      }
    }
  SUBTREE_VISIT_END();
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgFunctionRefExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFunctionRefExp* ret = 0;

  //xe::DOMNamedNodeMap* amap = node->getAttributes();
  //xe::DOMNode*         satt = 0;
  string               name;
  stringstream         val;
  int                  kind;
  
  //sb::pushScopeStack(_file->get_globalScope());
  if( XmlGetAttributeValue(node,"symbol",&name) )
    ret = sb::buildFunctionRefExp( SgName(name),_file->get_globalScope());
  else
    XEV_ABORT();

  if(XmlGetAttributeValue(node,"subprogram_kind",&kind )){
    // set subprogram_kind (2014.04.14)
    SgFunctionRefExp* functionRefExp = ret;
    SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
    SgFunctionDeclaration* functionDeclaration =
      functionSymbol->get_declaration();
    SgProcedureHeaderStatement* procedureHeaderStatement =
      isSgProcedureHeaderStatement(functionDeclaration);
    if(procedureHeaderStatement)
      procedureHeaderStatement
	->set_subprogram_kind((SgProcedureHeaderStatement::subprogram_kind_enum)kind );
    else XEV_ABORT();
  }
  //sb::popScopeStack();
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgFunctionCallExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFunctionCallExp*        ret  = 0;
  SgExpression*             exp  = 0;
  SgExprListExp*            para = 0;


  std::vector< SgExpression * > exprs;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if( exp==0 )
        exp = isSgExpression(astchild);
      if( para==0 )
        para = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildFunctionCallExp( exp, para );
  ret->set_parent(astParent);
  if(exp) exp->set_parent(ret);
  if(para) para->set_parent(ret);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgUseStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgUseStatement* ret = 0;
  string name;
  int only=0;
  SgRenamePair* rn = 0;

  XEV_ASSERT(astParent!=0);
  if(XmlGetAttributeValue(node,"name",&name) && XmlGetAttributeValue(node,"only",&only)) 
    ret = new SgUseStatement(astParent->get_file_info(),name,only);
  else 
    XEV_ABORT();

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      //this->visit(child,ret);
      if((rn=isSgRenamePair(astchild))!=0){
	ret->get_rename_list().push_back(rn);
      }
    }
    child=child->getNextSibling();
  } 

  // SgModuleSymbol is not used??
  SgClassSymbol* csym = si::lookupClassSymbolInParentScopes(name);
  if(csym) {
    //XEV_WARN("found");
    // TODO: we have to fully support R1109 in SageBuiler.C (but now only partially).
    SgClassDeclaration* decl = csym->get_declaration();
    SgModuleStatement* mstmt = isSgModuleStatement(decl);
    if(mstmt==0) XEV_ABORT();
    ret->set_module(mstmt);
    SgClassDefinition* defn = isSgClassDeclaration(decl->get_definingDeclaration())->get_definition();
    if(defn==0) XEV_ABORT();
    SgSymbol* sym = defn->first_any_symbol();
    while(sym!=0){
      SgAliasSymbol* asym = new SgAliasSymbol(sym,false);
      sb::topScopeStack()->insert_symbol(sym->get_name(),asym);
      sym= defn->next_any_symbol();
    }
  }
  else {
    //XEV_WARN("External module file?");
  }
  ret->set_definingDeclaration(ret); 
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgFortranIncludeLine(xe::DOMNode* node, SgNode* astParent)
{
  SgFortranIncludeLine* ret = 0;
  //xe::DOMNamedNodeMap* amap = node->getAttributes();
  //xe::DOMNode* nameatt = 0;
  std::string name;

  if( XmlGetAttributeValue(node,"filename",&name) ){
    ret = sb::buildFortranIncludeLine( name );
  }
  else XEV_ABORT();
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgAttributeSpecificationStatement(xe::DOMNode* node, SgNode* astParent)
{
    SgAttributeSpecificationStatement*  ret = 0;
    SgExpression*                       exp = 0;

    int                                 kind;

    if(XmlGetAttributeValue(node,"kind",&kind)==false)
      XEV_ABORT();

    SgPntrArrRefExp*                aref = 0;
    SgVarRefExp*                    vref = 0;
    SgVariableSymbol*               simb = 0;
    SgInitializedName*              inam = 0;
    SgInitializedName*              cnam = 0;
    SgNode*                         prnt = 0;

    //SgExpressionPtrList           lst;
    std::vector< SgExpression * >   lst;
    SgExprListExp*                  elst = 0;
    SgDataStatementGroup*           dataGroup = 0;
    SgDataStatementGroupPtrList     localList;
    SgStringVal*                    str = 0;
    Rose_STL_Container<std::string> slst;


    //xe::DOMNode* child=node->getFirstChild();

    switch (kind){
    case SgAttributeSpecificationStatement::e_parameterStatement:
    case SgAttributeSpecificationStatement::e_externalStatement:
    case SgAttributeSpecificationStatement::e_dimensionStatement:
    case SgAttributeSpecificationStatement::e_allocatableStatement:
      SUBTREE_VISIT_BEGIN(node,astchild,0)
	{
	  exp = isSgExpression(astchild);
	  if( exp ) {
	    
	    // search SgVariableDeclaration            add (0828)
	    if( (aref=isSgPntrArrRefExp(exp)) != 0 ){
	      if( (vref=isSgVarRefExp(aref->get_lhs_operand())) != 0 ){
		if( (simb=vref->get_symbol() ) != 0 ){
		  inam = simb->get_declaration();
		  prnt = inam->get_parent();
		  if( prnt == 0 ){
		    SgBasicBlock* b = isSgBasicBlock(astParent);
		    if( b ){
		      SgStatementPtrList& l =  b->get_statements();
		      for( size_t i=0; i<l.size(); i++ ){
			SgVariableDeclaration* v = isSgVariableDeclaration(l[i]);
			if( v == 0 )
			  continue;
			SgInitializedNamePtrList& n = v->get_variables();
			for(size_t j=0;j<n.size();j++){
			  cnam = n[j];
			  if( cnam->get_name().getString() == inam->get_name().getString() ){
			    inam->set_parent(v);
			    break;
			  }
			}
		      }
		    }
		  }
		}
	      }
	    }
	    lst.push_back(exp);
	    //ret->get_parameter_list()->prepend_expression(exp);
	  }
	}
      SUBTREE_VISIT_END();
      ret = sb::buildAttributeSpecificationStatement( (SgAttributeSpecificationStatement::attribute_spec_enum)  kind  );
      elst = sb::buildExprListExp( lst );
      elst->set_parent(ret);
      ret->set_parameter_list( elst );
      break;
      
    case SgAttributeSpecificationStatement::e_dataStatement :
      ret = new SgAttributeSpecificationStatement( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
      ret->set_attribute_kind((SgAttributeSpecificationStatement::attribute_spec_enum)  kind);
      SUBTREE_VISIT_BEGIN(node,astchild,0)      
	{
	  dataGroup = isSgDataStatementGroup(astchild);
	  if( dataGroup ) {
	    
	    localList.push_back(dataGroup);
	    //dataGroup->set_parent(ret);
	  }
	}
      SUBTREE_VISIT_END();
      for (size_t i = 0; i<localList.size(); i++)
	ret->get_data_statement_group_list().push_back(localList[i]);
      break;
    case SgAttributeSpecificationStatement::e_pointerStatement:      
    case SgAttributeSpecificationStatement::e_saveStatement :
      ret = new SgAttributeSpecificationStatement( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
      ret->set_attribute_kind((SgAttributeSpecificationStatement::attribute_spec_enum)  kind);
      SUBTREE_VISIT_BEGIN(node,astchild,0)      
	{
	  str = isSgStringVal(astchild);
	  if( str ) {
	    slst.push_back(  str->get_value() );
	  }
	}
      SUBTREE_VISIT_END();
      ret->get_name_list() = slst;
      break;

    case SgAttributeSpecificationStatement::e_accessStatement_private:
    case SgAttributeSpecificationStatement::e_accessStatement_public:
      ret = new SgAttributeSpecificationStatement( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
      ret->set_attribute_kind((SgAttributeSpecificationStatement::attribute_spec_enum)  kind);
      
      break;
    default:
      XEV_ABORT();
      break;
    }
    //TODO: need to consider forward declaration?
    if(ret) ret->set_definingDeclaration(ret);
    return ret;
}

SgNode* 
XevXmlVisitor::visitSgImplicitStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgImplicitStatement*      ret   = new SgImplicitStatement( astParent->get_file_info() , true );
  SgInitializedName*        inam  = 0;
  SgInitializedNamePtrList  lst;

  ret->set_parent(astParent);
  ret->set_definingDeclaration(ret);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)      
    {
      if((inam = isSgInitializedName(astchild))!=0)
        lst.push_back(inam);
    }
  SUBTREE_VISIT_END();

  if(!lst.empty()) {
    ret->set_implicit_none( false );
    ret->get_variables() = lst;
    for(size_t i(0);i<lst.size();i++)
      lst[i]->set_parent(ret);
  }

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgFortranDo(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFortranDo*              ret  = 0;
  SgExpression*             ini  = 0;
  SgExpression*             bnd  = 0;
  SgExpression*             inc  = 0;
  SgBasicBlock*             body = 0;

  int                       style = 0;
  int                       enddo = 0;
  int                       ino   = 0;
  string                    slabel,nlabel;
  stringstream              val;

  XmlGetAttributeValue(node,"style",&style);
  XmlGetAttributeValue(node,"end",&enddo);
  XmlGetAttributeValue(node,"slabel",&slabel);
  XmlGetAttributeValue(node,"nlabel",&nlabel);

  Sg_File_Info* info 
    = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  
  ret = new SgFortranDo(info);
  ret->set_parent(astParent);
  ret->setCaseInsensitive(true);
  sb::pushScopeStack(ret);
  SUBTREE_VISIT_BEGIN(node,astchild,ret) 
    {
      /* assuming these stmts appear in this order */
      if(ini==0)
        ini = isSgExpression(astchild);
      else if (bnd==0)
        bnd = isSgExpression(astchild);
      else if (inc==0)
        inc = isSgExpression(astchild);
      else if(body==0)
        body = isSgBasicBlock(astchild);
    }
  SUBTREE_VISIT_END();
  sb::popScopeStack();

  //ret = new SgFortranDo( astParent->get_file_info(), ini,bnd,inc,body);
  ret->set_initialization(ini);
  ret->set_bound(bnd);
  ret->set_increment(inc);
  ret->set_body(body);

  ret->set_old_style( style );
  ret->set_has_end_statement( enddo );
  ret->set_parent(astParent);

  if(ini) ini->set_parent(ret);
  if(bnd) bnd->set_parent(ret);
  if(inc) inc->set_parent(ret);
  if(body) body->set_parent(ret);

  // they are special labels of SgFortranDo
  if( slabel.size() )
    ret->set_string_label( slabel );
  else if( nlabel.size() ) {
    SgLabelSymbol*  s = new SgLabelSymbol();
    //s->set_label_type( SgLabelSymbol::label_type_enum.e_non_numeric_label_type );
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    s->get_fortran_statement()->set_parent(s);
    s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    val.str("");
    val.clear(stringstream::goodbit);
    val << nlabel;
    val >> ino;
    s->set_numeric_label_value( ino );
    ret->set_end_numeric_label( new SgLabelRefExp( s ) );
  }

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgComplexVal(xercesc::DOMNode* node, SgNode* astParent)
{
  SgComplexVal* ret = 0;
    
  SgValueExp*   real  = 0;
  SgValueExp*   imag  = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      /* assuming these exprs appear in this order */
      if(real==0)
        real = isSgValueExp(astchild);
      else if (imag==0)
        imag = isSgValueExp(astchild);
    }
  SUBTREE_VISIT_END();
  ret = sb::buildComplexVal( real, imag );
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgClassDeclaration(xercesc::DOMNode* node, SgNode* astParent)
{
  SgClassDeclaration*   ret=0;
  SgClassDefinition*    exp=0;
  SgClassDeclaration*   nondefn=0;

  SgScopeStatement* scope = sb::topScopeStack();    //?
  //SgScopeStatement* scope = sb::ScopeStack.front(); // true

  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt=0;
  string                name,val;
  int                   typ=0;

  XmlGetAttributeValue(node,"tag_name",&name);
  XmlGetAttributeValue(node,"type",&typ);

  SgClassSymbol* csym = si::lookupClassSymbolInParentScopes(name);
  if(csym==NULL) {
    nondefn = sb::buildClassDeclaration( SgName(name.c_str()), scope );
    nondefn->set_firstNondefiningDeclaration(nondefn);
    nondefn->set_definition(NULL);
    nondefn->set_definingDeclaration(NULL);
    nondefn->set_class_type( (SgClassDeclaration::class_types)typ  );
    nondefn->set_parent(scope);
    nondefn->set_scope(scope);
    csym = new SgClassSymbol(nondefn);
    scope->insert_symbol(name,csym);
  }
  else {
    nondefn = csym->get_declaration();
    nondefn->set_firstNondefiningDeclaration(nondefn);
    nondefn->set_definition(NULL);
    nondefn->set_definingDeclaration(NULL);
    nondefn->set_class_type( (SgClassDeclaration::class_types)typ  );
  }
  /*
  ret = sb::buildClassDeclaration( SgName(name.c_str()), scope );
  ret->set_class_type( (SgClassDeclaration::class_types)typ  );
  */
  ret = new SgClassDeclaration( DEFAULT_FILE_INFO, name, 
				(SgClassDeclaration::class_types)typ, 
				SgClassType::createType(nondefn));

  ret->set_parent(astParent);  
  ret->set_scope(scope);
  ret->set_firstNondefiningDeclaration(nondefn);
  XEV_WARN("nondefn: " << nondefn << " ret " << ret);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( exp==NULL )
        exp = isSgClassDefinition( astchild );
    }
  SUBTREE_VISIT_END();

  if(exp!=NULL){
    exp->set_declaration(nondefn);
    exp->set_parent(ret);
    ret->set_definition( exp );
    ret->set_definingDeclaration(ret);
    nondefn->set_definingDeclaration(ret);
  }
  else XEV_ABORT();
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgClassDefinition(xercesc::DOMNode* node, SgNode* astParent)
{
  SgClassDefinition*        ret = 0;
  SgClassDeclaration*       dec = isSgClassDeclaration( astParent );
  SgDeclarationStatement*   fld = 0;
  int seq=0;
  int pri=0;
  int abs=0;

  XmlGetAttributeValue(node,"sequence",&seq);
  XmlGetAttributeValue(node,"private",&pri);
  XmlGetAttributeValue(node,"abstract",&abs);
  if(dec==0) XEV_ABORT();
  ret = sb::buildClassDefinition( dec );
  ret->set_parent(astParent);
  sb::pushScopeStack(ret);

  if(si::is_Fortran_language())
    ret->setCaseInsensitive(true);
  if(seq)
    ret->set_isSequence(seq);
  if(pri)
    ret->set_isPrivate(pri);
  if(abs)
    ret->set_isAbstract(abs);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      fld = isSgDeclarationStatement(astchild);
      if( fld ) {
        ret->append_member(fld);
        fld->set_parent(ret);
      }
    }
  SUBTREE_VISIT_END();
  sb::popScopeStack();

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgClassType(xe::DOMNode* node, SgNode* astParent)
{
  SgClassType*          ret = 0;
  //SgScopeStatement*     scope = sb::topScopeStack();    //?
  //SgScopeStatement* scope = sb::ScopeStack.front();
  //SgClassDeclaration*   dec;

  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt=0;
  string                name,val;
  int                   typ=0;
  SgDeclarationStatement* ds = 0;

  XmlGetAttributeValue(node,"tag_name",&name);
  XmlGetAttributeValue(node,"type",&typ);

  /*
  dec = sb::buildClassDeclaration( SgName(name.c_str()), scope );
  dec->set_class_type( (SgClassDeclaration::class_types)typ  );
  */
  SgClassSymbol* csym = si::lookupClassSymbolInParentScopes(name);
  if(csym)
    ds = isSgDeclarationStatement( csym->get_declaration() );
  else {
    //SgScopeStatement* scope = si::getEnclosingProcedure (sb::topScopeStack());
    //if(scope==NULL) scope = _file->get_globalScope();

    // A dummy object is created and is not registered in the symbol table.
    // It is not used by other objects. It is put on the current scope.
    SgScopeStatement* scope = sb::topScopeStack();
    SgClassDeclaration* dec 
      = new SgClassDeclaration(DEFAULT_FILE_INFO); //don't use high-level build function

    dec->set_class_type( (SgClassDeclaration::class_types)typ  );
    dec->set_name(name);
    dec->set_parent(scope);
    dec->set_scope(scope);
    dec->set_firstNondefiningDeclaration(NULL);
    dec->set_definition(NULL);
    dec->set_definingDeclaration(NULL);
    ds = isSgDeclarationStatement( dec );
    ds->setForward();
#if 0
    // don't insert symbol!
    csym = new SgClassSymbol(dec);
    scope->insert_symbol(name,csym);
#endif
  }
  XEV_ASSERT(ds!=NULL); 
  ret = new SgClassType( ds );
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgTypedefDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgTypedefDeclaration*     ret = 0;
  SgClassDeclaration*       cls = 0;
  SgType*                   typ = 0;

  //xe::DOMNamedNodeMap* amap = node->getAttributes();
  //xe::DOMNode* satt = 0;
  string name;
  
  XmlGetAttributeValue(node,"tag_name",&name);


  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(typ==0)
        typ = isSgType(astchild);
      if(cls==0)
        cls = isSgClassDeclaration(astchild);
    }
  SUBTREE_VISIT_END();
    
  if(cls) {
    SgType * type = cls->get_type();
    SgNamedType *namedType = isSgNamedType(type->findBaseType());
    namedType->get_declaration()->set_definingDeclaration(cls);
    namedType->set_declaration (cls);
    ret = sb::buildTypedefDeclaration( name.c_str(), 
                                       namedType,
                                       sb::topScopeStack());
    XEV_ASSERT(ret!=NULL);
    cls->set_parent(ret);
    ret->set_declaration( isSgDeclarationStatement(cls->get_firstNondefiningDeclaration()) );
    ret->set_requiresGlobalNameQualificationOnType(true);
    if(cls->get_definingDeclaration() == cls )
      ret->set_typedefBaseTypeContainsDefiningDeclaration(true);
  }
  else if(typ) {                                         // add (0819)
    ret = sb::buildTypedefDeclaration( name.c_str(), 
                                       typ,
                                       sb::topScopeStack());
  }
  else XEV_ABORT();
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgEnumDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgEnumDeclaration*    ret = 0;
  SgScopeStatement*     scope = sb::topScopeStack();    //?

  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt=0;
  string                name,val;

  XmlGetAttributeValue(node,"tag_name",&name);

  SgInitializedName*        inam  = 0;
  SgInitializedNamePtrList  lst;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if((inam = isSgInitializedName(astchild))!=0)
        lst.push_back(inam);
    }
  SUBTREE_VISIT_END();

  if( name.size() ){
    ret = sb::buildEnumDeclaration( SgName( name.c_str() ), scope );
    ret->set_name(SgName( name.c_str()));
    ret->set_isUnNamed( false );
    
  }
  else {
    ret = sb::buildEnumDeclaration( SgName( name.c_str() ), scope );
    ret->set_isUnNamed( true );

  }

  if( name.size() ){
    ret->set_name(SgName( name.c_str()));
    ret->set_isUnNamed( false );
  }
  else {
    ret->set_isUnNamed( true );

  }

  if(!lst.empty())
    ret->get_enumerators() = lst;

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgEnumType(xe::DOMNode* node, SgNode* astParent)
{
  SgEnumType* ret =  new SgEnumType(0);
  ret->set_parent(astParent);
  return ret;
}

SgNode*
XevXmlVisitor::visitSgEnumVal(xe::DOMNode* node, SgNode* astParent)
{
  SgIntVal* ret = 0;
  int ival;

  if(XmlGetAttributeValue(node,"value",&ival))
    ret = sb::buildIntVal(ival);
  else XEV_ABORT();

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgDotExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDotExp*     ret = 0;
  SgExpression* lhs = 0;
  SgExpression* rhs = 0;
  SgClassDefinition*   defn=0;
  ret = sb::buildDotExp( lhs,rhs);
  bool pushed = false;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( lhs==0 ){
        lhs = isSgExpression(astchild);
	if(lhs) {
	  SgClassType* ctype = isSgClassType(lhs->get_type());
	  // The following line is needed for h032.f90. I don't know why...
	  if(ctype==0) ctype = isSgClassType(si::getElementType(lhs->get_type()));
	  if(ctype){
	    SgClassDeclaration* decl = isSgClassDeclaration(ctype->get_declaration());
	    if(decl->get_definition()==0) 
	      decl = isSgClassDeclaration(decl->get_definingDeclaration());
	    if(decl && decl->get_definition()){
	      defn = decl->get_definition();
	      sb::pushScopeStack(defn);
	      pushed = true;
	    }
	    else {
	      //XEV_WARN(decl->class_name()<<decl->get_definition()<<decl->get_name());
	      //XEV_ABORT();
	    }
	  }
	  else {
	    //XEV_WARN( lhs->get_type()->class_name() );
	    //XEV_ABORT(); // a class instance must be lhs of this op.
	  }
	}
      }
      else if( rhs==0 ){
	if(pushed==true)
	  sb::popScopeStack();
        rhs = isSgExpression(astchild);
      }
    }
  SUBTREE_VISIT_END();
  //ret = new SgDotExp( lhs->get_file_info(), lhs, rhs, lhs->get_type() );
  if(lhs == 0 || rhs == 0) XEV_ABORT();
  ret->set_lhs_operand(lhs);
  ret->set_rhs_operand(rhs);
  lhs->set_parent(ret);
  rhs->set_parent(ret);
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgPntrArrRefExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgPntrArrRefExp*     ret = 0;
  SgExpression* lhs = 0;
  SgExpression* rhs = 0;

  ret = sb::buildPntrArrRefExp( lhs,rhs);
  ret->set_file_info(DEFAULT_FILE_INFO);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( lhs==0 ){
        lhs = isSgExpression(astchild);
      }
      else if( rhs==0 ){
        rhs = isSgExpression(astchild);
      }
    }
  SUBTREE_VISIT_END();

  if(lhs == 0 || rhs == 0) XEV_ABORT();
  ret->set_lhs_operand(lhs);
  ret->set_rhs_operand(rhs);

  lhs->set_parent(ret);
  rhs->set_parent(ret);
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgArrowExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgArrowExp*     ret = 0;
  SgExpression* lhs = 0;
  SgExpression* rhs = 0;


  std::vector< SgExpression * > exprs;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if( lhs==0 )
        lhs = isSgExpression(astchild);
      else if( rhs==0 )
        rhs = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = new SgArrowExp( lhs->get_file_info(), lhs, rhs, lhs->get_type() );
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgProgramHeaderStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgType*               typ = 0;
  SgFunctionDefinition* fdf = 0;
  SgBasicBlock*         def = 0;
  SgFunctionParameterList*      lst = 0;

  string              name;

  if(XmlGetAttributeValue(node,"name",&name)==false)
    XEV_ABORT();

  SgFunctionType* ftyp = new SgFunctionType(SgTypeVoid::createType(), false);
  SgProgramHeaderStatement* ret 
    = new SgProgramHeaderStatement(astParent->get_file_info(),SgName(name), ftyp, NULL);

  ret->set_definingDeclaration(ret);
  ret->set_scope(sb::topScopeStack());
  ret->set_parent(astParent);

  //SgGlobal* globalScope = isSgGlobal(sb::topScopeStack());
  //globalScope->append_statement(ret);
  //SgFunctionSymbol* symbol = new SgFunctionSymbol(ret);
  //globalScope->insert_symbol(SgName(name.c_str()), symbol);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( typ==0 )
        typ = isSgType(astchild);
      if( fdf==0 )
        fdf = isSgFunctionDefinition(astchild);
      if(def==0) {
        def = isSgBasicBlock(astchild);
      }
      if(lst==0)
        lst = isSgFunctionParameterList(astchild);
    }
  SUBTREE_VISIT_END();

  //SgFunctionDefinition* programDefinition = new SgFunctionDefinition(ret, def);
  if(fdf==0 && def){
    fdf = new SgFunctionDefinition(ret, def);
    fdf->setCaseInsensitive(true);
    def->setCaseInsensitive(true);
    def->set_parent(fdf);
  }
  fdf->set_parent(ret);

  Sg_File_Info* info = DEFAULT_FILE_INFO;
  info->setOutputInCodeGeneration();
  fdf->set_file_info(info);

  ret->set_name(SgName(name.c_str()));
  ret->set_definition(fdf);
  if(fdf->get_file_info()==0) XEV_ABORT();
  if(def && def->get_file_info()==0) XEV_ABORT();

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgPrintStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgPrintStatement*     ret = new SgPrintStatement(astParent->get_file_info());
  SgExprListExp*        exp = 0;
  SgExpression*         fmt = 0;
  
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( exp==0 )
	exp = isSgExprListExp(astchild);
      else if( fmt==0 )
	fmt = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();
  
  ret->set_io_stmt_list(exp);
  ret->set_format( fmt );
  if(exp)exp->set_parent(ret);
  if(fmt)fmt->set_parent(ret);
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgAsteriskShapeExp(xe::DOMNode* node, SgNode* astParent)
{
  //SgAsteriskShapeExp* ret = new SgAsteriskShapeExp(astParent->get_file_info());
  SgAsteriskShapeExp* ret = new SgAsteriskShapeExp(DEFAULT_FILE_INFO);   // 0822
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgLabelRefExp(xe::DOMNode* node, SgNode* astParent)
{
  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt = 0;
  int                   ino   = 0;
  int                   type  = 0;
  string                tmp;


  XmlGetAttributeValue(node,"nlabel",&ino);
  XmlGetAttributeValue(node,"type"  ,&type);

  SgLabelSymbol*  s = new SgLabelSymbol();
  //s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
  s->set_fortran_statement( new SgStatement(DEFAULT_FILE_INFO) );
  s->get_fortran_statement()->set_parent(s);
  s->set_label_type( (SgLabelSymbol::label_type_enum)type );
  s->set_numeric_label_value( ino );
  //SgLabelRefExp*  ret = new SgLabelRefExp( s );
  SgLabelRefExp*  ret = sb::buildLabelRefExp( s );
  s->set_parent(ret);
  //ret->set_startOfConstruct(astParent->get_file_info());
  ret->set_startOfConstruct(DEFAULT_FILE_INFO);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgFormatStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgFormatItem*           itm = 0;
  SgFormatItemList*       lst = new SgFormatItemList();

  SUBTREE_VISIT_BEGIN(node,astchild,0)  
    {
      if((itm = isSgFormatItem(astchild))!=0){
	lst->get_format_item_list().insert(
              //lst->get_format_item_list().begin(), itm );
              lst->get_format_item_list().end(), itm );
      }
    }
  SUBTREE_VISIT_END();

  SgFormatStatement*    ret = new SgFormatStatement(astParent->get_file_info(),lst);
  lst->set_parent( ret );
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgFormatItem(xe::DOMNode* node, SgNode* astParent)
{
  SgStringVal*          val;
  //SgExpression*         val;
  string                fmt;
  string                sgl;
  string                dbl;
  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt = 0;
  
  XmlGetAttributeValue(node,"fmt",&fmt);
  XmlGetAttributeValue(node,"SingleQuote",&sgl);
  XmlGetAttributeValue(node,"DoubleQuote",&dbl);
  //cerr << "SgFormatItem |" << fmt << "| end" << endl;
  val = sb::buildStringVal( fmt );
  
  if( sgl == "1" )                                  // add (0821)
    val->set_usesSingleQuotes(true);
  else if( dbl == "1" )                             // add (0821)
    val->set_usesDoubleQuotes(true);
  
  SgFormatItem*     ret = new SgFormatItem();
  
  ret->set_data( (SgExpression*)val );
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgWriteStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgWriteStatement*     ret = new SgWriteStatement(astParent->get_file_info());
  SgExprListExp*        exp = 0;
  SgExpression*         fmt = 0;
  SgExpression*         iost = 0;
  SgExpression*         rec = 0;
  SgExpression*         err = 0;
  SgExpression*         unt = 0;
  
  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt = 0;
  string                nlabel;

  int f_fmt = 0;
  int f_ios = 0;
  int f_rec = 0;
  int f_err = 0;

  XmlGetAttributeValue(node,"fmt",   &f_fmt);
  XmlGetAttributeValue(node,"iostat",&f_ios);
  XmlGetAttributeValue(node,"rec",   &f_rec);
  XmlGetAttributeValue(node,"err",   &f_err);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)  
    {
      //assuming these stmts appear in this order
      if( exp==0 )
	exp = isSgExprListExp(astchild);
      else if( unt==0 )
	unt = isSgExpression(astchild);
      else if( f_fmt && fmt==0 )
	fmt = isSgExpression(astchild);
      else if( f_ios && iost==0 )
	iost = isSgExpression(astchild);
      else if( f_rec && rec==0 )
	rec = isSgExpression(astchild);
      else if( f_err && err==0 )
	err = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_io_stmt_list(exp);
  ret->set_format( fmt );
  ret->set_iostat( iost );
  ret->set_rec( rec );
  ret->set_err( err );
  ret->set_io_statement(SgIOStatement::e_write);
  ret->set_unit(unt);
  if(exp)exp->set_parent(ret);
  if(fmt)fmt->set_parent(ret);
  if(iost)iost->set_parent(ret);
  if(rec)rec->set_parent(ret);
  if(err)err->set_parent(ret);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgOpenStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgOpenStatement*        ret = new SgOpenStatement(astParent->get_file_info());
  SgExpression*           exp = 0;
  SgLabelRefExp*          lbl = 0;
  
  //xe::DOMNamedNodeMap*    amap = node->getAttributes();
  //xe::DOMNode*            nameatt = 0;
  //string                  nlabel;
  int                     ino = 0;
  int                     flg[17];
  int                     val;
  const char* open_arg [] = {
    "unit",    // 1
    "iostat",  // 2
    "err",     // 3
    "file",    // 4
    "status",  // 5
    "access",  // 6
    "form",    // 7
    "recl",    // 8
    "blank",   // 9
    "position",// 10
    "action",  // 11
    "delim",   // 12
    "pad",     // 13
    "iomsg",   // 14
    "round",   // 15
    "sign",    // 16
    "asynchronous" // 17
  };

  ino = 0;
  memset( flg,0,sizeof(flg) );
  for(int i=0;i<17;i++){
    if(XmlGetAttributeValue(node,open_arg[i],&val)){
      flg[ino] = i+1;
      ino++;
    }
  }

  ino = 0;
  SUBTREE_VISIT_BEGIN(node,astchild,ret)  
    {
      exp = isSgExpression(astchild);
      if(exp) {
	switch( flg[ino] ){
	case 1:
	  ret->set_unit( exp );
	  break;
	case 2:
	  ret->set_iostat( exp );
	  break;
	case 3:
	  lbl = isSgLabelRefExp(astchild);
	  ret->set_err( lbl );
	  lbl->set_parent(ret);
	  break;
	case 4:
	  ret->set_file( exp );
	  break;
	case 5:
	  ret->set_status( exp );
	  break;
	case 6:
	  ret->set_access( exp );
	  break;
	case 7:
	  ret->set_form( exp );
	  break;
	case 8:
	  ret->set_recl( exp );
	  break;
	case 9:
	  ret->set_blank( exp );
	  break;
	case 10:
	  ret->set_position( exp );
	  break;
	case 11:
	  ret->set_action( exp );
	  break;
	case 12:
	  ret->set_delim( exp );
	  break;
	case 13:
	  ret->set_pad( exp );
	  break;
	case 14:
	  ret->set_iomsg( exp );
	  break;
	case 15:
	  ret->set_round( exp );
	  break;
	case 16:
	  ret->set_sign( exp );
	  break;
	case 17:
	  ret->set_asynchronous( exp );
	  break;
	}
	ino++;
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgCloseStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgCloseStatement*     ret = new SgCloseStatement(astParent->get_file_info());
  SgExpression*         unt = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)    
    {
      if( unt==0 )
	unt = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_unit(unt);
  ret->set_parent(astParent);
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgContainsStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgContainsStatement* ret = new SgContainsStatement(astParent->get_file_info());
  //si::setSourcePosition(ret);
  ret->set_definingDeclaration(ret);
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgModuleStatement(xe::DOMNode* node, SgNode* astParent)
{
  string                name;
  int                   typ=0;
  SgClassDefinition*    exp=0;
  SgModuleStatement*    non=0;
  XmlGetAttributeValue(node,"tag_name",&name);
  XmlGetAttributeValue(node,"type",&typ);
  
  SgModuleStatement* ret = buildModuleStatementAndDefinition(
                SgName( name.c_str() ), sb::topScopeStack());
  Sg_File_Info* info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  info->setOutputInCodeGeneration();
  ret->set_file_info(info);
  ret->set_parent(astParent);
  ret->set_scope(sb::topScopeStack());
  ret->set_class_type( (SgClassDeclaration::class_types)typ  );
  ret->set_name( SgName( name.c_str() ) );

  SgClassSymbol* csym = si::lookupClassSymbolInParentScopes(name);
  if(csym!=NULL){
    non = isSgModuleStatement(csym->get_declaration());
  }
  else XEV_ABORT(); //TODO

  SUBTREE_VISIT_BEGIN(node,astchild,ret)    
    {
      if( exp==0 )
        exp = isSgClassDefinition( astchild );
    }
  SUBTREE_VISIT_END();

  if(exp){
    exp->set_parent(ret);
    exp->set_declaration(ret);
    ret->set_definition( exp );
    non->set_definition( NULL );
    ret->set_definingDeclaration( ret );
    non->set_definingDeclaration(ret);
    ret->set_firstNondefiningDeclaration( non );
    non->set_firstNondefiningDeclaration(non);
  }
  else XEV_ABORT();

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgCommonBlockObject(xercesc::DOMNode* node, SgNode* astParent)
{
  SgCommonBlockObject*  ret = 0;
  SgExprListExp*        para = 0;
  string name;

  XmlGetAttributeValue(node,"name",&name);
  ret = sb::buildCommonBlockObject(name);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( para==0 )
        para = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  if(para==0) XEV_ABORT();
  ret->set_variable_reference_list(para);
  para->set_parent(ret);
  ret->set_parent(astParent);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgCommonBlock(xercesc::DOMNode* node, SgNode* astParent)
{
  SgCommonBlock*        ret=sb::buildCommonBlock();
  SgCommonBlockObject*  obj=0;
  SgCommonBlockObjectPtrList lst;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      obj = isSgCommonBlockObject(astchild);
      if(obj){
	lst.push_back(obj);
	obj->set_parent(ret);
      }
    }
  SUBTREE_VISIT_END();
  for (size_t i = 0; i< lst.size();i++){
    ret->get_block_list().push_back(lst[i]);
  }
  ret->set_parent(astParent);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgStringVal(xercesc::DOMNode* node, SgNode* astParent)
{
  SgStringVal*            ret = 0;
  string                  str;
  int                     flag=0;

  if(XmlGetAttributeValue(node,"value",&str) == false)
    XEV_ABORT();

  //if(str.size())                                // del (0821)
  ret = sb::buildStringVal(str);
  //else XEV_ABORT();                                 // del (0821)
  ret->set_parent(astParent);

  if(XmlGetAttributeValue(node,"SingleQuote",&flag))
    ret->set_usesSingleQuotes(flag);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgSubscriptExpression(xercesc::DOMNode* node, SgNode* astParent)
{
  SgSubscriptExpression*      ret=0;
  SgExpression*               low=0;
  SgExpression*               upp=0;
  SgExpression*               str=0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)  
    {
      //assuming these stmts appear in this order
      if( low==0 )
	low = isSgExpression(astchild);
      else if( upp==0 )
	upp = isSgExpression(astchild);
      else if( str==0 )
	str = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();
  
  ret = new SgSubscriptExpression(low,upp,str);
  ret->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  low->set_parent(ret);
  upp->set_parent(ret);
  str->set_parent(ret);
  ret->set_parent(astParent);
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgForAllStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForAllStatement*      ret 
    = new SgForAllStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExprListExp*          hed = 0;
  SgBasicBlock*           bdy = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)    
    {
      if( hed==0 )
	hed = isSgExprListExp(astchild);
      else if( bdy==0 )
	bdy = isSgBasicBlock(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_parent(astParent);
  hed->set_parent(ret);
  bdy->set_parent(ret);
  bdy->setCaseInsensitive(true);

  //ret->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  hed->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  bdy->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  
  ret->set_forall_header(hed);
  ret->set_body(bdy);
  //ret->set_has_end_statement(true);
  //ret->set_has_end_statement(false);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgInterfaceStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgInterfaceStatement*      ret=0;
  SgInterfaceBody*           bdy=0;
  
  string                name;
  int                   typ=0;

  XmlGetAttributeValue(node,"name",&name);
  XmlGetAttributeValue(node,"type",&typ);

  ret = new SgInterfaceStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
         	 SgName( name.c_str() ),(SgInterfaceStatement::generic_spec_enum)typ );

  ret->set_generic_spec( (SgInterfaceStatement::generic_spec_enum) typ );
  ret->set_parent(astParent);
  ret->set_definingDeclaration(ret);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)    
    {
      bdy = isSgInterfaceBody(astchild);
      if( bdy ) {
	bdy->set_parent(ret);
	ret->get_interface_body_list().push_back(bdy);
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}


SgNode* 
XevXmlVisitor::visitSgInterfaceBody(xercesc::DOMNode* node, SgNode* astParent)
{
  SgInterfaceBody*                ret=0;
  SgFunctionDeclaration*          bdy=0;
  //SgProcedureHeaderStatement*     def=0;
  
  
  string                          name;
  
  XmlGetAttributeValue(node,"fnc_name",&name);
  //ret = new SgInterfaceBody(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  //ret = new SgInterfaceBody( astParent->get_file_info() );
  ret = new SgInterfaceBody(name, NULL, true );
  ret->set_file_info(astParent->get_file_info());
  ret->set_parent(astParent);
#if 0
  /*--- 2013.08.05 delete --*/
  if( name.size() ) {
    ret->set_function_name( SgName( name.c_str() ) );
    ret->set_use_function_name( true );
  }
  /*---*/
#endif 
  SUBTREE_VISIT_BEGIN(node,astchild,ret)            
    {
      if( bdy==0 )
	bdy = isSgFunctionDeclaration(astchild);
      //if(def==0)
      //def = isSgProcedureHeaderStatement(astchild);
    }
  SUBTREE_VISIT_END();

  if(bdy){
    ret->set_functionDeclaration( bdy );
    ret->set_use_function_name( false );
    bdy->set_parent(ret);
  }
  else if( name.size() ) {
    ret->set_function_name( SgName( name.c_str() ) );
    ret->set_use_function_name( true );
  }
  else XEV_ABORT();
    
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgReadStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgReadStatement*        ret = new SgReadStatement(astParent->get_file_info());
  SgExprListExp*          exp = 0;
  SgExpression*           fmt = 0;
  SgExpression*           iost = 0;
  SgExpression*           rec = 0;
  SgExpression*           end = 0;
  SgExpression*           err = 0;
  SgExpression*           unt = 0;
  
  int f_fmt = 0;
  int f_ios = 0;
  int f_rec = 0;
  int f_end = 0;
  int f_err = 0;

  XmlGetAttributeValue(node,"fmt"   ,&f_fmt);
  XmlGetAttributeValue(node,"iostat",&f_ios);
  XmlGetAttributeValue(node,"rec",   &f_rec);
  XmlGetAttributeValue(node,"end",   &f_end);
  XmlGetAttributeValue(node,"err",   &f_err);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)            
    {
      //assuming these stmts appear in this order
      if( exp==0 )
	exp = isSgExprListExp(astchild);
      else if( unt==0 )
	unt = isSgExpression(astchild);
      else if( f_fmt && fmt==0 )
	fmt = isSgExpression(astchild);
      else if( f_ios && iost==0 )
	iost = isSgExpression(astchild);
      else if( f_rec && rec==0 )
	rec = isSgExpression(astchild);
      else if( f_end && end==0 )
	end = isSgExpression(astchild);
      else if( f_err && err==0 )
	err = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_parent(astParent);  
  ret->set_io_stmt_list(exp);
  ret->set_format( fmt );
  ret->set_unit(unt);
  if(exp)exp->set_parent(ret);
  if(fmt)fmt->set_parent(ret);
  if(iost)iost->set_parent(ret);
  if(rec)rec->set_parent(ret);
  if(end)end->set_parent(ret);
  if(err)err->set_parent(ret);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgEntryStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgEntryStatement*       ret = 0;
  SgFunctionType*        ftyp = 0;
  SgFunctionDefinition*   def = 0;
  SgFunctionParameterList* lst = 0;
  string                  name;
  string                  rname;
  
  XmlGetAttributeValue(node,"name",&name);

  SUBTREE_VISIT_BEGIN(node,astchild,0)            
    {
      if( ftyp==0 )
	ftyp = isSgFunctionType(astchild);
      if( lst==0 )
	lst = isSgFunctionParameterList(astchild);
      if( def==0 )
	def = isSgFunctionDefinition(astchild);
    }
  SUBTREE_VISIT_END();

  ret = new SgEntryStatement( astParent->get_file_info(),
			      SgName( name.c_str() ),
			      ftyp,
			      def);
  //ret->set_scope(def);
  ret->set_scope(sb::topScopeStack());
  ret->set_parent(astParent);  
  if(lst){
    ret->set_definingDeclaration(ret);
    ret->set_parameterList(lst);
    lst->set_parent(ret);
  }
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgAllocateStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgAllocateStatement*    ret=0;
  SgExprListExp*          exp=0;
  
  ret = new SgAllocateStatement(astParent->get_file_info());

  SUBTREE_VISIT_BEGIN(node,astchild,ret)   
    {
      if( exp==0 )
	exp = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_parent(astParent);
  ret->set_expr_list(exp);
  //exp->set_parent(ret);
  
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgDeallocateStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDeallocateStatement*    ret=0;
  SgExprListExp*          exp=0;
  
  ret = new SgDeallocateStatement(astParent->get_file_info());
  
  SUBTREE_VISIT_BEGIN(node,astchild,ret)   
    {
      if( exp==0 )
	exp = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();
  
  //printf( "ret=%p,exp=%p\n",ret,exp);
  ret->set_expr_list(exp);
  ret->set_parent(astParent);
  //exp->set_parent(ret);
  
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgArithmeticIfStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgArithmeticIfStatement*    ret 
    = new SgArithmeticIfStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*               cnd = 0;
  SgLabelRefExp*              les = 0;
  SgLabelRefExp*              eql = 0;
  SgLabelRefExp*              grt = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)     
    {
      //assuming these stmts appear in this order
      if( les==0 )
	les = isSgLabelRefExp(astchild);
      else if( eql==0 )
	eql = isSgLabelRefExp(astchild);
      else if( grt==0 )
	grt = isSgLabelRefExp(astchild);
      else if( cnd==0 )
	cnd = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_parent(astParent);

  ret->set_less_label( les );
  les->set_parent( ret );
  
  ret->set_equal_label( eql );
  eql->set_parent( ret );
  
  ret->set_greater_label( grt );
  grt->set_parent( ret );
  
  ret->set_conditional( cnd );
  cnd->set_parent( ret );
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgStopOrPauseStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgStopOrPauseStatement*     ret=0;
  SgExpression*               cod=0;
  
  int                         typ=0;
    
  XmlGetAttributeValue(node,"type",&typ);
  
  ret = new SgStopOrPauseStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  ret->set_stop_or_pause( (SgStopOrPauseStatement::stop_or_pause_enum) typ );
  ret->set_parent(astParent);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)       
    {
      if( cod )
	cod = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();
  if( cod==0 )
    cod = new SgNullExpression(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  
  ret->set_code( cod );
  cod->set_parent( ret );
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgElseWhereStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgElseWhereStatement* ret =  
    new SgElseWhereStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  SgExpression*         cond  = 0;
  SgBasicBlock*         body  = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)       
    {
      if(cond==0)
        cond = isSgExpression(astchild);
      else if (body==0)
        body = isSgBasicBlock(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_condition( cond );
  ret->set_body( body );
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgWhereStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgWhereStatement* ret =  
    new SgWhereStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  ret->set_parent(astParent);    
  SgExpression*         cond = 0;
  SgBasicBlock*         body = 0;
  SgElseWhereStatement* elsw = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)       
    {
      /* assuming these stmts appear in this order */
      if(cond==0)
        cond = isSgExpression(astchild);
      else if (body==0)
        body = isSgBasicBlock(astchild);
      else if (elsw==0)
        elsw = isSgElseWhereStatement(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_condition( cond );
  ret->set_body( body );
  ret->set_elsewhere( elsw );
  ret->set_has_end_statement(true);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgNullifyStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgNullifyStatement*   ret = 
    new SgNullifyStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  SgExprListExp*        plst = 0;


  std::vector< SgExpression * > exprs;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)       
    {
      if( plst==0 )
        plst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_parent(astParent);
  ret->set_pointer_list( plst );
  ret->set_parent(astParent);
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgBackspaceStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgBackspaceStatement*   ret = 
    new SgBackspaceStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*           unt = 0;
  SgLabelRefExp*          err = 0;
  
  string                  nlabel;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)         
    {
	//assuming these stmts appear in this order
      if( unt==0 )
	unt = isSgExpression(astchild);
      else if( err==0 )
	err = isSgLabelRefExp(astchild);
    }
  SUBTREE_VISIT_END();
  
  if( unt ) {
    ret->set_unit( unt );
    unt->set_parent(ret);
    unt->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  }
  
  if( err ) {
    ret->set_err( err );
    err->set_parent(ret);
    err->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  }
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgEndfileStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgEndfileStatement*     ret = 
    new SgEndfileStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*           unt = 0;
  SgLabelRefExp*          err = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)           
    {
      if( unt==0 )
	unt = isSgExpression(astchild);
      else if( err==0 )
	err = isSgLabelRefExp(astchild);
    }
  SUBTREE_VISIT_END();

  if( unt ) {
    ret->set_unit( unt );
    unt->set_parent(ret);
    unt->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  }
  
  if( err ) {
    ret->set_err( err );
    err->set_parent(ret);
    err->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  }
  
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgRewindStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgRewindStatement*      ret = 
    new SgRewindStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*           unt = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)             
    {
      if( unt==0 )
	unt = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();
  
  if( unt ) {
    ret->set_unit( unt );
    unt->set_parent(ret);
    unt->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  }
  
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgInquireStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgInquireStatement*     ret = 
    new SgInquireStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*           exp = 0;
  SgLabelRefExp*          lbl = 0;
  SgVarRefExp*            var = 0;
  SgExprListExp*          ele = 0;
  //xe::DOMNamedNodeMap*    amap = node->getAttributes();
  //xe::DOMNode*            nameatt = 0;
  //string                  nlabel;
  int                     ino = 0;
  int                     flg[33];
  const char* arg_type[] ={
    "iolength",    // 1
    "unit",        // 2
    "iostat",      // 3
    "err",         // 4
    "iomsg",       // 5
    "file",        // 6
    "access",      // 7 
    "form",        // 8
    "recl",        // 9
    "blank",       //10
    "exist",       //11
    "opened",      //12
    "number",      //13
    "named",       //14
    "name",        //15
    "sequential",  //16
    "direct",      //17
    "formatted",   //18
    "unformatted", //19
    "nextrec",     //20
    "position",    //21
    "action",      //22
    "access",      //23
    "read",        //24
    "write",       //25
    "readwrite",   //26
    "delim",       //27
    "pad",         //28
    "asynchronous",//29
    "decimal",     //30
    "stream",      //31
    "size",        //32
    "pending"      //33
  };

  ret->set_parent(astParent);
  ino = 0;
  memset( flg,0,sizeof(flg) );
  for(int i(0);i<33;i++){
    int val;
    if( XmlGetAttributeValue(node,arg_type[i],&val) ){
      flg[ino] = i+1;
      ino++;
    }
  }
  
  ino = 0;
  SUBTREE_VISIT_BEGIN(node,astchild,ret)             
    {      
      exp = isSgExpression(astchild);
      switch( flg[ino] ){
      case 1:
	ele = isSgExprListExp(astchild);
	var = isSgVarRefExp(astchild);
	if( ele ) {
	  ret->set_io_stmt_list(ele);
	  ele->set_parent(ret);
	}
	if( var ) {
	  ret->set_iolengthExp(var);
	  var->set_parent(ret);
	}
	ino--;
	break;
        
      case 2:
	ret->set_unit( exp );
	break;
      case 3:
	ret->set_iostat( exp );
	break;
      case 4:
	lbl = isSgLabelRefExp(astchild);
	ret->set_err( lbl );
	lbl->set_parent(ret);
	break;
      case 5:
	ret->set_iomsg( exp );
	break;
      case 6:
	ret->set_file( exp );
	break;
      case 7:
	ret->set_access( exp );
	break;
      case 8:
	ret->set_form( exp );
	break;
      case 9:
	ret->set_recl( exp );
	break;
      case 10:
	ret->set_blank( exp );
	break;
      case 11:
	ret->set_exist( exp );
	break;
      case 12:
	ret->set_opened( exp );
	break;
      case 13:
	ret->set_number( exp );
	break;
      case 14:
	ret->set_named( exp );
	break;
      case 15:
	ret->set_name( exp );
	break;
      case 16:
	ret->set_sequential( exp );
	break;
      case 17:
	ret->set_direct( exp );
	break;
      case 18:
	ret->set_formatted( exp );
	break;
      case 19:
	ret->set_unformatted( exp );
	break;
      case 20:
	ret->set_nextrec( exp );
	break;
      case 21:
	ret->set_position( exp );
	break;
      case 22:
	ret->set_action( exp );
	break;
      case 23:
	ret->set_read( exp );
	break;
      case 24:
	ret->set_write( exp );
	break;
      case 25:
	ret->set_readwrite( exp );
	break;
      case 26:
	ret->set_delim( exp );
	break;
      case 27:
	ret->set_pad( exp );
	break;
      case 28:
	ret->set_asynchronous( exp );
	break;
      case 29:
	ret->set_opened( exp );
	break;
      case 30:
	ret->set_decimal( exp );
	break;
      case 31:
	ret->set_stream( exp );
	break;
      case 32:
	ret->set_size( exp );
	break;
      case 33:
	ret->set_pending( exp );
	break;
      }
      ino++;
    }
  SUBTREE_VISIT_END();
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgFlushStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgFlushStatement*      ret =
    new SgFlushStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*           unt = 0;
  
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( unt==0 )
	unt = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  if( unt ) {
    ret->set_unit( unt );
    unt->set_parent(ret);
    unt->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  }
  ret->set_parent(astParent);
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgNamelistStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgNamelistStatement*    ret = 
    new SgNamelistStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgNameGroup*            grp = new SgNameGroup();
  SgNameGroupPtrList      glst;
  SgStringVal*            str = 0;
  
  string                  name;

  XmlGetAttributeValue(node,"group",&name);
  
  Rose_STL_Container<std::string> slst;
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      str = isSgStringVal(astchild);
      if( str ) {
	slst.push_back(  str->get_value() );
      }
    }
  SUBTREE_VISIT_END();

  grp->set_group_name( name );
  grp->get_name_list() = slst;
  grp->set_parent(ret);
  ret->get_group_list().push_back( grp );
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgDerivedTypeStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgDerivedTypeStatement*     ret = 0;
  SgClassDefinition*          exp = 0;
  SgScopeStatement* scope = sb::topScopeStack();    //?
  //SgScopeStatement* scope = sb::ScopeStack.front();
  SgDerivedTypeStatement*     nondefn = 0;  
  string                      name;
  int                         typ=0;
  
  XmlGetAttributeValue(node,"tag_name",&name);
  XmlGetAttributeValue(node,"type",&typ);

  ret = new SgDerivedTypeStatement(DEFAULT_FILE_INFO,
				   name,SgClassDeclaration::e_struct,NULL,NULL );
  ret->set_parent(scope);
  ret->set_scope(scope);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( exp==0 )
	exp = isSgClassDefinition( astchild );
    }
  SUBTREE_VISIT_END();

  SgClassSymbol* sym = si::lookupClassSymbolInParentScopes(name);
  if(exp){
    ret->set_definition(exp);
    ret->set_definingDeclaration(ret);
    exp->setCaseInsensitive(true);
    exp->set_parent(ret);
    exp->set_declaration(ret);

    if(sym==NULL){ // this is the first decl.
      nondefn = new SgDerivedTypeStatement(DEFAULT_FILE_INFO,
					   name,SgClassDeclaration::e_struct,NULL,NULL );
      SgClassSymbol* newsym = new SgClassSymbol(nondefn);
      scope->insert_symbol(name,newsym);
      nondefn->set_parent(scope);
      nondefn->set_scope(scope);
      nondefn->set_firstNondefiningDeclaration(nondefn);
      nondefn->setForward();
      nondefn->set_definingDeclaration(ret);
      //nondefn->set_definition(exp);
      newsym->set_declaration(nondefn);
      ret->set_firstNondefiningDeclaration(nondefn);
    }
    else{
      SgClassDeclaration* firstnondefn 
	= isSgClassDeclaration(sym->get_declaration());
      XEV_ASSERT(firstnondefn!=NULL);
      ret->set_firstNondefiningDeclaration(firstnondefn);
      firstnondefn->set_firstNondefiningDeclaration(firstnondefn);
      firstnondefn->set_definingDeclaration(ret);
      //firstnondefn->set_definition(exp);
    }
  }
  else if(sym==NULL){ // this is the first decl.
    SgClassSymbol* newsym = new SgClassSymbol(ret);
    scope->insert_symbol(name,newsym);
    ret->set_definition(NULL);
    ret->set_definingDeclaration(NULL);
    ret->set_firstNondefiningDeclaration(ret);
    ret->setForward();
    newsym->set_declaration(ret);
  }

  if (ret->get_type () == NULL) {
    if(nondefn!=NULL){
      nondefn->set_type (SgClassType::createType(nondefn));
      ret->set_type (nondefn->get_type());
    }
    else
      ret->set_type (SgClassType::createType(ret));
  }
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgComputedGotoStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgExprListExp*          exp = 0;
  SgExpression*           var = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)  
    {
      if( exp==0 )
	exp = isSgExprListExp(astchild);
      else if( var==0 )
	var = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  SgComputedGotoStatement*    ret = 
    new SgComputedGotoStatement(astParent->get_file_info(), exp,var );
  
  exp->set_parent(ret);
  var->set_parent(ret);
  ret->set_parent(astParent);
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgPointerDerefExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgPointerDerefExp*    ret = 0;
  SgExpression*         exp = 0;
  SgType*               typ = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)  
    {
      if(typ==0)
	typ = isSgType(astchild);
      if(exp==0)
	exp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = new SgPointerDerefExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
			      exp, typ );
  ret->set_parent(astParent);
  exp->set_parent(ret);
  typ->set_parent(ret);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgVarArgStartOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgVarArgStartOp*  ret = 0;
  SgExpression*     lhs = 0;
  SgExpression*     rhs = 0;
  
  SUBTREE_VISIT_BEGIN(node,astchild,0)  
    {
      if( lhs==0 )
        lhs = isSgExpression(astchild);
      else if( rhs==0 )
        rhs = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = new SgVarArgStartOp( Sg_File_Info::generateDefaultFileInfoForTransformationNode(), lhs, rhs, lhs->get_type() );
  ret->set_parent(astParent);
  lhs->set_parent(ret);
  rhs->set_parent(ret);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgVarArgOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgVarArgOp*       ret = 0;
  SgExpression*     lhs = 0;
  SgType*           typ = 0;
  

  SUBTREE_VISIT_BEGIN(node,astchild,0)    
    {
      if( typ==0 )
        typ = isSgType(astchild);
      else if( lhs==0 )
        lhs = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = new SgVarArgOp( Sg_File_Info::generateDefaultFileInfoForTransformationNode(), lhs, typ );
  //ret = sb::buildVarArgOp_nfi( lhs,lhs->get_type() );
  ret->set_parent(astParent);
  lhs->set_parent(ret);
  typ->set_parent(ret);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgVarArgEndOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgVarArgEndOp*  ret = 0;
  SgExpression*     lhs = 0;
  

  SUBTREE_VISIT_BEGIN(node,astchild,0)      
    {
      if( lhs==0 )
        lhs = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = new SgVarArgEndOp( Sg_File_Info::generateDefaultFileInfoForTransformationNode(), lhs, lhs->get_type() );
  ret->set_parent(astParent);
  lhs->set_parent(ret);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgEquivalenceStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgEquivalenceStatement* ret = new SgEquivalenceStatement(DEFAULT_FILE_INFO);
  SgExprListExp*          lst = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)        
    {
      if( lst==0 )
	lst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  if( lst ) {
    ret->set_equivalence_set_list(lst);
    ret->set_definingDeclaration(ret);
    lst->set_parent(ret);
    lst->set_startOfConstruct(DEFAULT_FILE_INFO);
  }
  else XEV_ABORT();
  ret->set_parent(astParent);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgAsmStmt(xe::DOMNode* node, SgNode* astParent)
{
  SgAsmStmt*     ret = 0;
  
  //xe::DOMNamedNodeMap*        amap = node->getAttributes();
  //xe::DOMNode*                nameatt=0;
  string                      asm_code;
  string                      vol;
  int                         typ=0;
  
  if(XmlGetAttributeValue(node,"asm_code",&asm_code) == false
     ||   XmlGetAttributeValue(node,"volatile",&typ) == false )
    XEV_ABORT();

  ret = sb::buildAsmStatement( asm_code );
  ret->set_isVolatile( typ );
  ret->set_parent(astParent);

  return ret;
}


SgNode* 
XevXmlVisitor::visitSgAggregateInitializer(xe::DOMNode* node, SgNode* astParent)
{
  SgAggregateInitializer* ret = 0;
  SgExprListExp*          lst = 0;
  SgType*                 typ = 0;
  
  SUBTREE_VISIT_BEGIN(node,astchild,0)        
    {
      if( typ==0 )
	typ = isSgType(astchild);
      if( lst==0 )
	lst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildAggregateInitializer( lst,typ );
  ret->set_parent(astParent);
  lst->set_parent(ret);
  typ->set_parent(ret);
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgFunctionType(xe::DOMNode* node, SgNode* astParent)
{
  SgFunctionType*                 ret = 0;
  SgType*                         typ = 0;
  SgFunctionParameterTypeList*    lst = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)          
    {
      if( typ==0 )
	typ = isSgType(astchild);
      if( lst==0 )
	lst = isSgFunctionParameterTypeList(astchild);
    }
  SUBTREE_VISIT_END();

  if(lst==0)XEV_ABORT();
  if( typ==0 ) {
    typ = isSgType( sb::buildIntType() );
  }
  ret = sb::buildFunctionType( typ,lst );
  ret->set_return_type(typ);
  ret->set_parent(astParent);
  lst->set_parent(ret);
  typ->set_parent(ret);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgFunctionParameterTypeList(xe::DOMNode* node, SgNode* astParent)
{
  SgFunctionParameterTypeList*    ret = 0;
  SgExprListExp*                  exp = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)            
    {
      if( exp==0 )
	exp = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();
  
  if( exp == 0) // modified (2014.09.20)
    exp = new SgExprListExp( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  
  ret =  sb::buildFunctionParameterTypeList( exp );
  ret->set_parent(astParent);
  exp->set_parent(ret);
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgPointerAssignOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgPointerAssignOp*    ret = 0;
  SgExpression*         lhs = 0;
  SgExpression*         rhs = 0;
  
  
  std::vector< SgExpression * > exprs;
  
  SUBTREE_VISIT_BEGIN(node,astchild,0)            
    {
      if( lhs==0 )
        lhs = isSgExpression(astchild);
      else if( rhs==0 )
        rhs = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = new SgPointerAssignOp( Sg_File_Info::generateDefaultFileInfoForTransformationNode(), lhs, rhs, rhs->get_type() );
  ret->set_parent(astParent);
  lhs->set_parent(ret);
  rhs->set_parent(ret);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgCompoundInitializer(xe::DOMNode* node, SgNode* astParent)
{
  SgCompoundInitializer*  ret = 0;
  SgExprListExp*          lst = 0;
  SgType*                 typ = 0;
  
  SUBTREE_VISIT_BEGIN(node,astchild,0)            
    {
      if( typ==0 )
	typ = isSgType(astchild);
      if( lst==0 )
	lst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildCompoundInitializer( lst,typ );
  ret->set_parent(astParent);
  lst->set_parent(ret);
  typ->set_parent(ret);
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgImpliedDo(xercesc::DOMNode* node, SgNode* astParent)
{
  SgImpliedDo*        ret = 0;
  SgExpression*       ini = 0;
  SgExpression*       las = 0;
  SgExpression*       inc = 0;
  SgExprListExp*      lst = 0;
  SgScopeStatement*   scp = 0;
  
  SUBTREE_VISIT_BEGIN(node,astchild,0)            
    {
      /* assuming these stmts appear in this order */
      if(ini==0)
	ini = isSgExpression(astchild);
      else if (las==0)
	las = isSgExpression(astchild);
      else if (inc==0)
	inc = isSgExpression(astchild);
      else if(lst==0)
	lst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();
  
  ret = new SgImpliedDo(  Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
			  ini,las,inc,lst,scp);
  ini->set_parent(ret);
  las->set_parent(ret);
  inc->set_parent(ret);
  lst->set_parent(ret);
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgDataStatementGroup(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDataStatementGroup*   ret = new SgDataStatementGroup();
  //SgDataStatementObject*  obj = new SgDataStatementObject();
  //SgDataStatementValue*   val = new SgDataStatementValue();
  //SgExprListExp*          nam = 0;
  //SgExpression*           exp = 0;
  //SgExprListExp*          exprList = new SgExprListExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  
  //val->set_initializer_list( exprList );
  //exprList->set_parent(val);
  //val->set_data_initialization_format( SgDataStatementValue::e_explict_list );
  ret->set_parent(NULL);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)              
    {
      SgDataStatementObject* obj = isSgDataStatementObject(astchild);
      SgDataStatementValue*  val = isSgDataStatementValue (astchild);
      
      if(obj)
	ret->get_object_list().push_back(obj);
      if(val)
	ret->get_value_list() .push_back(val);
#if 0
      /* assuming these stmts appear in this order */
      if(nam==0) {
	if( (nam = isSgExprListExp(astchild)) != 0 ) {
	  obj->set_variableReference_list( nam );
	  nam->set_parent( obj );
	}
      }
      else {
	//if( (exp = isSgExprListExp(astchild)) != 0 )
	if( (exp = isSgExpression(astchild)) != 0 ) {
	  //obj->get_variableReference_list()->append_expression( exp );
	  val->get_initializer_list()->append_expression( exp );
	  exp->set_parent( obj );
	}
      }
#endif
    }
  SUBTREE_VISIT_END();
  //obj->set_parent(ret);
  //val->set_parent(ret);

  return ret;
}


SgNode* 
XevXmlVisitor::visitSgDataStatementObject(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDataStatementObject*  ret = new SgDataStatementObject();
  SgExprListExp* lst = 0;
  SUBTREE_VISIT_BEGIN(node,astchild,ret)              
    {
      if(lst==0)
	lst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();
  if(lst)
    ret->set_variableReference_list(lst);
  else
    XEV_ABORT();
  //ret->set_parent(astParent);
  ret->set_parent(NULL);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgDataStatementValue(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDataStatementValue*  ret = new SgDataStatementValue();
  SgExprListExp* ilst = 0;
  SgExpression*  rept = 0;
  SgExpression*  cnst = 0;
  int format = 0;

  XmlGetAttributeValue(node,"format",&format);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)              
    {
      // assuming the order 
      if(ilst==0)
	ilst = isSgExprListExp(astchild);
      else if(rept==0)
	rept = isSgExpression(astchild);
      else if(cnst==0)
	cnst = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();
  if(ilst)
    ret->set_initializer_list(ilst);
  if(rept)
    ret->set_repeat_expression(rept);
  if(cnst)
    ret->set_constant_expression(cnst);
  ret->set_data_initialization_format((SgDataStatementValue::data_statement_value_enum)format);
  ret->set_parent(astParent);
  return ret;
}



SgNode* 
XevXmlVisitor::visitSgRenamePair(xercesc::DOMNode* node, SgNode* astParent)
{
  SgRenamePair*     ret = 0;
  string                      lname;
  string                      uname;

  if(XmlGetAttributeValue(node,"lname",&lname)==false
     || XmlGetAttributeValue(node,"uname",&uname)==false )
    XEV_ABORT();

  ret = new SgRenamePair(lname,uname);
  if(ret) {
    ret->set_parent(astParent);
#if 0
    // add this SgRenamePair object to * its parent node *
    SgUseStatement* useStmt = isSgUseStatement(astParent);
    if(useStmt){ // this should be true
      useStmt->get_rename_list().push_back(ret);
    }
#endif
  }
  else XEV_ABORT();
  Sg_File_Info* info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  info->setOutputInCodeGeneration();
  ret->set_file_info(info);
  return ret;
}



SgNode* 
XevXmlVisitor::visitSgConstructorInitializer(xercesc::DOMNode* node, SgNode* astParent)
{
  SgConstructorInitializer* ret      = 0;
  SgMemberFunctionDeclaration* mdecl = 0;
  SgExprListExp* elst                = 0;
  SgType*        typ                 =0;
  int name     =0;
  int qual     =0;
  int paren    =0;
  int unkc     =0;

  XmlGetAttributeValue(node,"need_name",    &name);
  XmlGetAttributeValue(node,"need_qual",    &qual);
  XmlGetAttributeValue(node,"need_paren",   &paren);
  XmlGetAttributeValue(node,"unknown_class",&unkc);

  SUBTREE_VISIT_BEGIN(node,child,0)
    {
      if(mdecl==0)
	mdecl = isSgMemberFunctionDeclaration(child);
      if(elst==0)
	elst = isSgExprListExp(child);
      if(typ==0)
	typ = isSgType(child);
    }    
  SUBTREE_VISIT_END();
  
  ret = sb::buildConstructorInitializer(mdecl,elst,typ,name,qual,paren,unkc);
  if(mdecl) mdecl->set_parent(ret);
  if(elst) elst->set_parent(ret);
  if(typ) typ->set_parent(ret);

  return ret;
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
