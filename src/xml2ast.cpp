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

#ifdef XEVXML_DEBUG    
    XevXML::OrphanTest test;
    test.traverse(&(*prj)->get_file(0),preorder);
#endif
    
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
    WARN("Invalid SgProject pointer. Conversion failed.");
    return false;
  }
  // set default configuration
  if(help==0) help = new XevConversionHelper();
  
  if(TraverseXercesDOMDocument(str,prj,help)==false){
    WARN("XML document parsing failed.");
    return false;
  }
  return true;
}}

using namespace XevXML;
XevXmlVisitor::XevXmlVisitor(SgProject* prj, XevConversionHelper* help)
{
  if(help==0) ABORT();
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
  if(_file==0){ ABORT(); }
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
        cerr << "unknown AST node found " << nname << endl;
        ABORT();
      }
      if(ret && astParent) ret->set_parent(astParent);
      checkPreprocInfo(node,ret);
      checkExpression(node,ret);
      checkStatement(node,ret);
      checkFunctDecl(node,ret);
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
      cerr << astNode->class_name() << " does not have parent node." << endl;
      ABORT();
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
    if(si::is_Fortran_language())
      isSgScopeStatement(stmt)->setCaseInsensitive(true);
  }
}

void 
XevXmlVisitor::checkFunctDecl(xe::DOMNode* node, SgNode* astNode)
{
  SgFunctionDeclaration* decl = isSgFunctionDeclaration(astNode);
  int enf=0;
  string storage;
  if(decl==0) return;

  if(XmlGetAttributeValue(node,"end_name",&enf))
    decl->set_named_in_end_statement(enf);
  XmlGetAttributeValue(node,"modifier",&storage);  

  if(storage=="unknown")
    ((decl->get_declarationModifier()).get_storageModifier()).setUnknown();
  else if(storage=="static")
    ((decl->get_declarationModifier()).get_storageModifier()).setStatic();
  else if(storage=="extern")
    ((decl->get_declarationModifier()).get_storageModifier()).setExtern();
  else if(storage=="auto")
    ((decl->get_declarationModifier()).get_storageModifier()).setAuto();
  else if(storage=="register")
    ((decl->get_declarationModifier()).get_storageModifier()).setRegister();
  else if(storage=="typedef")
    ((decl->get_declarationModifier()).get_storageModifier()).setTypedef();
  else 
    ((decl->get_declarationModifier()).get_storageModifier()).setDefault();
  
  if(decl->get_definition())
    decl->get_definition()->set_declaration(decl);
}


SgNode* 
XevXmlVisitor::visitSgSourceFile(xe::DOMNode* node, SgNode* astParent)
{
  xe::DOMNode* child=node->getFirstChild();
  int langid;
  int fmtid;

  if(XmlGetAttributeValue(node,"language",&langid)==false)
    WARN("Language attribute is missing.");
  if(XmlGetAttributeValue(node,"format",&fmtid)==false)
    WARN("Format attribute is missing.");

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

  //cerr << _file->get_outputLanguage() << ":" << _file->get_outputFormat() << endl;

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
	else si::appendStatement (astchild,isSgScopeStatement(blk));
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
      //std::cerr << "DEBUG:" << tmp << ": " << tmp.length() << std::endl;
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
  SgGlobal* ret = new SgGlobal(_file->get_file_info());
  _file->set_globalScope(ret);
  ret->set_parent(_file);
  Sg_File_Info* info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  ret->set_file_info(info);
  if(si::is_Fortran_language())
    ret->setCaseInsensitive(true);

  sb::pushScopeStack(ret);

  traverseStatementsAndTexts(this,node,ret);

  sb::popScopeStack();

  /* prtine all symbol tables  for debugging */
  PrintSymTable test;
  test.visit(ret);
  
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
  else ABORT();
  
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
  else ABORT();
  
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
  string            storage;
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
      }

      if( cls==0 ) {
        cls = isSgDeclarationStatement(astchild);
      }
    }
  SUBTREE_VISIT_END();


  if(name) {
    if( cls ) {                                         
      SgType * type = name->get_type();
      SgNamedType *namedType = isSgNamedType(type->findBaseType());
      cls->set_parent( type );
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
    if(ret==0) ABORT();
    ret->set_parent(astParent);
    ret->set_definingDeclaration(ret);
    // see buildVariableDeclaration in fortran_support.C 
    ret->set_variableDeclarationContainsBaseTypeDefiningDeclaration(false);

    if( varList.size() > 0 && si::is_Fortran_language() ) 
      // NOTE: in the case of Fortran, append_statement does not work? 
      // So this is necessary to define the second variable and later.
      ret->get_variables() = varList;
    for(size_t i(0);i<varList.size();i++){
      //cerr << varList[i]->get_name() << ":" << varList[i]->get_type()->class_name() <<endl;
      if(i>0 && si::is_Fortran_language() == false )
	ret->append_variable(varList[i],varList[i]->get_initializer());
      varList[i]->set_parent(ret);
      varList[i]->set_declptr(ret);
    }
  }
  else ABORT();

  bool isFortranParameter = false;
  // See buildVariableDeclaration
  if (ret->get_scope() && si::is_Fortran_language()){
    SgFunctionDefinition * f_def = si::getEnclosingProcedure (ret->get_scope());
    /* variables in a function parameter list are moved? */
    if (f_def != NULL){
      SgSymbolTable * st = f_def->get_symbol_table();
      SgVariableSymbol * v_symbol = st->find_variable(name->get_name());
      if (v_symbol != NULL){
	SgInitializedName *default_initName = ret->get_decl_item (name->get_name());
	SgInitializedName * new_initName = v_symbol->get_declaration();
	SgInitializedNamePtrList&  n_list= ret->get_variables();
	std::replace (n_list.begin(), n_list.end(),default_initName, new_initName );
	
	SgNode * old_parent = new_initName->get_parent();
	if(old_parent==0 || isSgFunctionParameterList(old_parent)==0) ABORT();
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
  if (! isFortranParameter)
    si::fixVariableDeclaration(ret,ret->get_scope());

  // Initialize SgAccessModifier (2014.04.16)
  ret->get_declarationModifier().get_accessModifier().setUndefined();

  if(storage=="unknown")
    ((ret->get_declarationModifier()).get_storageModifier()).setUnknown();
  else if(storage=="static")
    ((ret->get_declarationModifier()).get_storageModifier()).setStatic();
  else if(storage=="extern")
    ((ret->get_declarationModifier()).get_storageModifier()).setExtern();
  else if(storage=="auto")
    ((ret->get_declarationModifier()).get_storageModifier()).setAuto();
  else if(storage=="register")
    ((ret->get_declarationModifier()).get_storageModifier()).setRegister();
  else if(storage=="typedef")
    ((ret->get_declarationModifier()).get_storageModifier()).setTypedef();
  else if(storage=="const")
    ret->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setConst();
  else if(storage=="ALLOCATABLE")
    ret->get_declarationModifier().get_typeModifier().setAllocatable();
  else if(storage=="ASYNCHRONOUS")
    ret->get_declarationModifier().get_typeModifier().setAsynchronous();
  else if(storage=="INTENT(IN)")
    ret->get_declarationModifier().get_typeModifier().setIntent_in();
  else if(storage=="INTENT(OUT)")
    ret->get_declarationModifier().get_typeModifier().setIntent_out();
  else if(storage=="INTENT(INOUT)")
    ret->get_declarationModifier().get_typeModifier().setIntent_inout();
  else if(storage=="VOLATILE")
    ret->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setVolatile();
  else if(storage=="EXTERNAL")
    ret->get_declarationModifier().get_storageModifier().setExtern();
  else if(storage=="PUBLIC")
    ret->get_declarationModifier().get_accessModifier().setPublic();
  else if(storage=="PRIVATE")
    ret->get_declarationModifier().get_accessModifier().setPrivate();
  else if(storage=="INTRINSIC")
    ret->get_declarationModifier().get_typeModifier().setIntrinsic();
  else if(storage=="OPTIONAL")
    ret->get_declarationModifier().get_typeModifier().setOptional();
  else if(storage=="SAVE")
    ret->get_declarationModifier().get_typeModifier().setSave();
  else if(storage=="TARGET")
    ret->get_declarationModifier().get_typeModifier().setTarget();
  else if(storage=="VALUE")
    ret->get_declarationModifier().get_typeModifier().setValue();
  else 
    ((ret->get_declarationModifier()).get_storageModifier()).setDefault();
  //checkPreprocInfo(node,ret);

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
    ABORT();

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
  else ABORT();

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
  SgType*                         typ = 0;
  SgFunctionParameterList*        lst = 0;
  SgScopeStatement*               scope = isSgScopeStatement(_file->get_globalScope());
  //SgScopeStatement*               scope = sb::topScopeStack();
  SgBasicBlock*                   def = 0;
  SgFunctionDefinition*           fdf = 0;
  
  string                          name,rname;
  int                             kind=0;
  bool                            f_pure  = false;
  bool                            f_elem  = false;
  bool                            f_recur = false;
  
  XmlGetAttributeValue(node,"name",&name);
  XmlGetAttributeValue(node,"subprogram_kind",&kind);
  XmlGetAttributeValue(node,"result_name",&rname);
  XmlGetAttributeValue(node,"recursive",&f_recur);
  XmlGetAttributeValue(node,"pure",&f_pure);
  XmlGetAttributeValue(node,"elemental",&f_elem);
  
  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(typ==0)
	typ = isSgType(astchild);
      if(lst==0)
	lst = isSgFunctionParameterList(astchild);
      if(fdf==0)
	fdf = isSgFunctionDefinition(astchild);
      if(kind == SgProcedureHeaderStatement::e_block_data_subprogram_kind && def==0) // for block data
	def = isSgBasicBlock(astchild);
    }
  SUBTREE_VISIT_END();
  
  if(lst){
    if( kind != SgProcedureHeaderStatement::e_block_data_subprogram_kind ){
      //SgFunctionParameterList* cpy = isSgFunctionParameterList(si::deepCopyNode(lst)); //error! why?
      ret = sb::buildProcedureHeaderStatement( (const char*)(name.c_str()), typ, lst,
					       (SgProcedureHeaderStatement::subprogram_kind_enum)kind, scope);
    }
    else {
      // add (block data) 0828
      SgFunctionType* functionType = new SgFunctionType(SgTypeVoid::createType(), false);
      Sg_File_Info* info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
      info->setOutputInCodeGeneration();
      fdf = new SgFunctionDefinition( info,def );
      ret = new SgProcedureHeaderStatement( info,SgName(name), functionType,  fdf );
      def->set_parent(fdf);
      fdf->set_parent(ret);
      ret->set_subprogram_kind( (SgProcedureHeaderStatement::subprogram_kind_enum)kind );
    }
  }
  else ABORT();
  
  if(fdf){
    //si::replaceStatement( ret->get_definition()->get_body(),def,true );
    ret->set_definition(fdf);
    fdf->set_declaration(ret);
    fdf->set_parent(ret);
    if(rname.size()) {
    // the symbol table of fdf is not created yet. i don't know why.
#if 0
      SgSymbolTable* tbl = fdf->get_symbol_table();
      SgVariableSymbol* sym = tbl->find_variable( SgName(rname.c_str()) );
      SgInitializedName* ini =  0;
      if(sym && sym->get_declaration()) {
        ini = sym->get_declaration();
	ini->set_declptr(ret); 
	WARN("TODO: implemented?");
	ret->get_result_name()->set_definition(ini->get_declaration());
      }
#endif 
      VardefSearch search(rname);
      SgInitializedName* ini = isSgInitializedName(search.visit(fdf));
      if(ini){
	//WARN("variable found");
	ret->set_result_name(ini);
	ini->set_declptr(ret); 
	ret->get_result_name()->set_definition(ini->get_declaration());
      }
      else {
	//WARN("variable not found");
        ini = sb::buildInitializedName(rname,ret->get_type()->get_return_type());
	ini->set_parent(ret);
	//ini->set_definition(fdf);
	//ini->set_declptr(ret); // s005.f90:mismatch if uncommented, but needed for h025.f90
	ini->set_type(ret->get_type()->get_return_type());
	ini->set_scope(fdf);
	ret->set_result_name(ini);
	SgVariableSymbol* sym = new SgVariableSymbol(ini);
	fdf->insert_symbol(ini->get_name(),sym);
      }

    }
  }
  else ABORT();
  
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
      if((ini = isSgInitializedName(astchild)) != 0 ){
	si::appendArg(ret,ini);
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

  ret = sb::buildIfStmt(cond,tstmt,fstmt);
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
      ABORT();								\
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
      ABORT();								\
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
      ABORT();								\
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
VISIT_BINARY_OP(PntrArrRefExp);
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
  string                 name;
  SgVarRefExp* ret=0;

  if(XmlGetAttributeValue(node,"name",&name))
    ret= sb::buildVarRefExp(name);
  else 
    ABORT();
  ret->set_parent(astParent);
  //cerr << ret->get_symbol()->get_name().getString() << "=" << ret->get_type()->class_name() <<endl; 
#if 0
  if(isSgTypeUnknown(ret->get_type()) && si::is_Fortran_language() ){
    //cerr << ret->get_symbol()->get_name().getString() << "=" << ret->get_type()->class_name() <<endl; 

    SgVariableDeclaration* decl 
      =  new SgVariableDeclaration(name,generateImplicitType(name),NULL,sb::topScopeStack());
    //= sb::buildVariableDeclaration(name,generateImplicitType(name),NULL,sb::topScopeStack());
    //NOTE: Implicit variable found (assumption)
    //SgInitializedName* ini = isSgInitializedName(ret->get_symbol()->get_declaration());
    decl->set_parent(sb::topScopeStack());
    decl->set_definingDeclaration(decl);

    //SgInitializedName* ini = isSgInitializedName(decl->get_decl_item(0));
    SgInitializedName* ini = new SgInitializedName(name,generateImplicitType(name));
    if(ini==0) ABORT();
    //ini->set_definition(decl);
    ini->set_parent(decl);
    ini->set_declptr(decl);
    decl->append_variable(ini);
    // buildImplicitVariableDeclaration(ret->get_symbol()->get_name());
    //ret->set_symbol(isSgVariableSymbol(ini->get_symbol_from_symbol_table()));
    //SgVariableDefinition* vdef = isSgVariableDefinition(decl) ;
    //if(vdef==0)ABORT();
    //vdef->set_parent(ini);
    //vdef->set_vardefn(ini);

  }
#endif
#if 0
  if(ret->get_symbol()->get_declaration()==0 
     || ret->get_symbol()->get_declaration()->get_declptr()==0){
    // Implicit variable found 
    SgInitializedName* ini = isSgInitializedName(ret->get_symbol()->get_declaration());
    SgVariableDeclaration* decl 
      = sb::buildVariableDeclaration(name,generateImplicitType(name),NULL,sb::topScopeStack());
    decl->set_parent(sb::topScopeStack());
    decl->set_definingDeclaration(decl);
    if(ini==0) ABORT();
    //ini->set_declptr(decl);
    //ini->set_definition(decl);
    ini->set_parent(decl);
    // buildImplicitVariableDeclaration(ret->get_symbol()->get_name());
  }
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
    ABORT();

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
    ABORT();
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
  SgFunctionDefinition* ret 
    = new SgFunctionDefinition(info);
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

  ret = sb::buildInitializedName(name.c_str(),typ,ini);
  ret->set_parent(astParent);
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
  else ABORT();
  
  return ret;
}

#define VISIT_VAL(valType,baseType,buildVal)				\
  SgNode* XevXmlVisitor::						\
  visit##valType(xe::DOMNode* node, SgNode* astParent)			\
  {									\
    valType* ret = 0;							\
    baseType ival;							\
    std::string vstr;							\
    if(XmlGetAttributeValue(node,"value",&ival)==false) ABORT();	\
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
    if(XmlGetAttributeValue(node,"value",&ival)==false) ABORT();	\
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
  else ABORT();
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
  else ABORT();

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
  
  if( XmlGetAttributeValue(node,"symbol",&name) )
    ret = sb::buildFunctionRefExp( SgName(name) );
  else
    ABORT();

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
  }
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

  ASSERT(astParent==0);
  if(XmlGetAttributeValue(node,"name",&name) && XmlGetAttributeValue(node,"only",&only)) 
    ret = new SgUseStatement(astParent->get_file_info(),name,only);
  else 
    ABORT();

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      //SgNode* astchild = this->visit(child);
      this->visit(child,ret);
    }
    child=child->getNextSibling();
  } 
  // TODO: set whether defining or non-defining
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
  else ABORT();
  
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgAttributeSpecificationStatement(xe::DOMNode* node, SgNode* astParent)
{
    SgAttributeSpecificationStatement*  ret = 0;
    SgExpression*                       exp = 0;

    int                                 kind;

    if(XmlGetAttributeValue(node,"kind",&kind)==false)
      ABORT();

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
	    dataGroup->set_parent(ret);
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
      ABORT();
      break;
    }

    return ret;
}

SgNode* 
XevXmlVisitor::visitSgImplicitStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgImplicitStatement*      ret   = new SgImplicitStatement( astParent->get_file_info() , true );
  SgInitializedName*        inam  = 0;
  SgInitializedNamePtrList  lst;

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

  SUBTREE_VISIT_BEGIN(node,astchild,0) 
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
 
  ret = new SgFortranDo( astParent->get_file_info(), ini,bnd,inc,body);
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
  SgClassDeclaration*   ret;
  SgClassDefinition*    exp=0;

  SgScopeStatement* scope = sb::topScopeStack();    //?
  //SgScopeStatement* scope = sb::ScopeStack.front(); // true

  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt=0;
  string                name,val;
  int                   typ=0;

  XmlGetAttributeValue(node,"tag_name",&name);
  XmlGetAttributeValue(node,"type",&typ);
  
  ret = sb::buildClassDeclaration( SgName(name.c_str()), scope );
  ret->set_class_type( (SgClassDeclaration::class_types)typ  );

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( exp==0 )
        exp = isSgClassDefinition( astchild );
    }
  SUBTREE_VISIT_END();

  ret->set_definition( exp );
  if(exp)
    exp->set_declaration(ret);
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
  ret = sb::buildClassDefinition( dec );
  ret->set_parent(astParent);
  sb::pushScopeStack(ret);
  if(si::is_Fortran_language())
    ret->setCaseInsensitive(true);

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
  if(seq)
    ret->set_isSequence(seq);
  if(pri)
    ret->set_isPrivate(pri);
  if(abs)
    ret->set_isAbstract(abs);

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgClassType(xe::DOMNode* node, SgNode* astParent)
{
  SgClassType*          ret = 0;
  SgScopeStatement*     scope = sb::topScopeStack();    //?
  //SgScopeStatement* scope = sb::ScopeStack.front();
  SgClassDeclaration*   dec;

  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt=0;
  string                name,val;
  int                   typ=0;

  XmlGetAttributeValue(node,"tag_name",&name);
  XmlGetAttributeValue(node,"type",&typ);


  dec = sb::buildClassDeclaration( SgName(name.c_str()), scope );
  dec->set_class_type( (SgClassDeclaration::class_types)typ  );

  SgDeclarationStatement* ds = isSgDeclarationStatement( dec );

  ds->setForward();
  ret = new SgClassType( ds );
//ds->set_parent(ret);
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
    cls->set_parent( type );
    namedType->get_declaration()->set_definingDeclaration(cls);
    namedType->set_declaration (cls);
    ret = sb::buildTypedefDeclaration( name.c_str(), 
                                       namedType,
                                       sb::topScopeStack());

    ret->set_declaration( isSgDeclarationStatement(cls) );
    cls->set_parent(ret);
    ret->set_requiresGlobalNameQualificationOnType(true);
    ret->set_typedefBaseTypeContainsDefiningDeclaration(true);

  }
    else {                                            // add (0819)
    ret = sb::buildTypedefDeclaration( name.c_str(), 
                                       typ,
                                       sb::topScopeStack());
  }
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
  else ABORT();

  return ret;
}

SgNode* 
XevXmlVisitor::visitSgDotExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDotExp*     ret = 0;
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

  ret = new SgDotExp( lhs->get_file_info(), lhs, rhs, lhs->get_type() );
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


  //xe::DOMNamedNodeMap* amap = node->getAttributes();
  //xe::DOMNode* nameatt = 0;
  string              name;
  //int end_name;

  if(XmlGetAttributeValue(node,"name",&name)==false)
    ABORT();

  SgFunctionType* ftyp = new SgFunctionType(SgTypeVoid::createType(), false);
  SgProgramHeaderStatement* ret 
    = new SgProgramHeaderStatement(astParent->get_file_info(),SgName(name.c_str()), ftyp, NULL);

  ret->set_definingDeclaration(ret);
  ret->set_scope(sb::topScopeStack());
  //ret->set_parent(sb::topScopeStack());
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

  Sg_File_Info* info = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
  info->setOutputInCodeGeneration();
  fdf->set_file_info(info);

  ret->set_name(SgName(name.c_str()));
  ret->set_definition(fdf);
  if(fdf->get_file_info()==0) ABORT();
  if(def && def->get_file_info()==0) ABORT();

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
  SgAsteriskShapeExp* ret = new SgAsteriskShapeExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode());   // 0822
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
  s->set_fortran_statement( new SgStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode()) );
  s->get_fortran_statement()->set_parent(s);
  s->set_label_type( (SgLabelSymbol::label_type_enum)type );
  s->set_numeric_label_value( ino );
  //SgLabelRefExp*  ret = new SgLabelRefExp( s );
  SgLabelRefExp*  ret = sb::buildLabelRefExp( s );
  s->set_parent(ret);
  //ret->set_startOfConstruct(astParent->get_file_info());
  ret->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
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
  SgContainsStatement*     ret = new SgContainsStatement(astParent->get_file_info());
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgModuleStatement(xe::DOMNode* node, SgNode* astParent)
{
  string                name;
  int                   typ=0;

  XmlGetAttributeValue(node,"tag_name",&name);
  XmlGetAttributeValue(node,"type",&typ);
  
  SgModuleStatement* ret = buildModuleStatementAndDefinition(
                SgName( name.c_str() ), sb::topScopeStack());
  Sg_File_Info* info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  info->setOutputInCodeGeneration();
  ret->set_file_info(info);
  SgClassDefinition*    exp=0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)    
    {
      if( exp==0 )
        exp = isSgClassDefinition( astchild );
    }
  SUBTREE_VISIT_END();

  ret->set_class_type( (SgClassDeclaration::class_types)typ  );
  ret->set_name( SgName( name.c_str() ) );
  exp->set_parent(ret);
  ret->set_definition( exp );
  ret->set_parent(astParent);
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

  if(para==0) ABORT();
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
      }
    }
  SUBTREE_VISIT_END();

  for (size_t i = 0;i<lst.size(); ++i){
    ret->get_block_list().push_back(lst[i]);
    lst[i]->set_parent(ret);
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
    ABORT();

  //if(str.size())                                // del (0821)
  ret = sb::buildStringVal(str);
  //else ABORT();                                 // del (0821)
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
  
  SUBTREE_VISIT_BEGIN(node,astchild,ret)    
    {
      bdy = isSgInterfaceBody(astchild);
      if( bdy ) {
	bdy->set_parent(ret);
	ret->get_interface_body_list().push_back(bdy);
      }
    }
  SUBTREE_VISIT_END();
  
  ret->set_generic_spec( (SgInterfaceStatement::generic_spec_enum) typ );
  ret->set_parent(astParent);
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgInterfaceBody(xercesc::DOMNode* node, SgNode* astParent)
{
  SgInterfaceBody*                ret=0;
  SgFunctionDeclaration*          bdy=0;
  SgProcedureHeaderStatement*     def=0;
  
  
  string                          name,val;
  
  XmlGetAttributeValue(node,"fnc_name",&name);
  //ret = new SgInterfaceBody(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  ret = new SgInterfaceBody( astParent->get_file_info() );
  ret->set_parent(astParent);

  /*--- 2013.08.05 delete --*/
  if( name.size() ) {
    ret->set_function_name( SgName( name.c_str() ) );
    ret->set_use_function_name( true );
  }
  /*---*/

  SUBTREE_VISIT_BEGIN(node,astchild,ret)            
    {
      if( bdy==0 )
	bdy = isSgFunctionDeclaration(astchild);
      if(def==0)
	def = isSgProcedureHeaderStatement(astchild);
      
    }
  SUBTREE_VISIT_END();
  if(bdy){
    ret->set_functionDeclaration( bdy );
    bdy->set_parent(ret);
  }

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
  SgFunctionType*         typ = 0;
  SgFunctionDefinition*   def = 0;
  
  string                  name;
  
  XmlGetAttributeValue(node,"name",&name);

  SUBTREE_VISIT_BEGIN(node,astchild,0)            
    {
      if( typ==0 )
	typ = isSgFunctionType(astchild);
      
      if( def==0 )
	def = isSgFunctionDefinition(astchild);
    }
  SUBTREE_VISIT_END();

  ret = new SgEntryStatement( astParent->get_file_info(),
			      SgName( name.c_str() ),
			      typ,
			      def  );
  ret->set_scope(def);
  ret->set_parent(astParent);  
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
  //SgScopeStatement* scope = sb::topScopeStack();    //?
  SgScopeStatement* scope = sb::ScopeStack.front();
  
  string                      name;
  int                         typ=0;
  
  XmlGetAttributeValue(node,"tag_name",&name);
  XmlGetAttributeValue(node,"type",&typ);
  
  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if( exp==0 )
	exp = isSgClassDefinition( astchild );
    }
  SUBTREE_VISIT_END();
  exp->setCaseInsensitive(true);
  exp->set_parent(ret);
  ret = new SgDerivedTypeStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
				   name.c_str(),SgClassDeclaration::e_struct,NULL,exp );
  ret->set_definingDeclaration(ret);
  
  SgDerivedTypeStatement* nondefiningClassDeclaration = 
    new SgDerivedTypeStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
			       name.c_str(),SgClassDeclaration::e_struct,NULL,NULL);
  nondefiningClassDeclaration->set_parent(scope);
  
  if (nondefiningClassDeclaration->get_type () == NULL) 
    nondefiningClassDeclaration->set_type (SgClassType::createType(nondefiningClassDeclaration));
  ret->set_type(nondefiningClassDeclaration->get_type());
  
  ret->set_firstNondefiningDeclaration(nondefiningClassDeclaration);
  
  nondefiningClassDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);
  nondefiningClassDeclaration->set_definingDeclaration(ret);
  
  nondefiningClassDeclaration->setForward();
  
  exp->set_declaration(ret);
  
  ret->set_scope(scope);
  nondefiningClassDeclaration->set_scope(scope);
  
  ret->set_parent(scope);
  
  SgClassSymbol* classSymbol = new SgClassSymbol(nondefiningClassDeclaration);
  
  scope->insert_symbol(name,classSymbol);
  

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
  SgEquivalenceStatement* ret = new SgEquivalenceStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExprListExp*          lst = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)        
    {
      //assuming these stmts appear in this order
      if( lst==0 )
	lst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  if( lst ) {
    ret->set_equivalence_set_list(lst);
    lst->set_parent(ret);
    lst->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  }
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
    ABORT();

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

  SUBTREE_VISIT_BEGIN(node,astchild,0)          
    {
      if( typ==0 )
	typ = isSgType(astchild);
      if( lst==0 )
	lst = isSgFunctionParameterTypeList(astchild);
    }
  SUBTREE_VISIT_END();

  if( typ==0 ) {
    typ = isSgType( sb::buildIntType() );
  }
  ret = sb::buildFunctionType( typ,lst );
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
  ret->set_parent(astParent);
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
    ABORT();
  ret->set_parent(astParent);
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
    ABORT();

  ret = new SgRenamePair(lname,uname);
  if(ret) {
    ret->set_parent(astParent);
    // add this SgRenamePair object to * its parent node *
    SgUseStatement* useStmt = isSgUseStatement(astParent);
    if(useStmt){ // this should be true
      useStmt->get_rename_list().push_back(ret);
    }
  }
  else ABORT();
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

