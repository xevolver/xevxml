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
#include "xevxml.hpp"
#include "xml2ast.hpp"

#include "Cxx_Grammar.h"

#include <iostream>
#include "StorageClasses.h"

#include "fortran_support.h"

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;


//#define VISIT(x) if(nname==#x) { return visit##x (node,astParent);}
//#define VISIT(x) if(nname==#x) { cerr << #x << endl; SgNode* ret = visit##x (node,astParent);   checkPreprocInfo(node,ret);  return ret;  }
#define VISIT(x) if(nname==#x) {SgNode* ret = visit##x (node,astParent);   checkPreprocInfo(node,ret);  return ret;  }

static SgFile* Xml2AstDOMVisit(stringstream& tr, SgProject* prj, string ofn)
{
  SgFile* ret = 0;
  try {
    xe::XercesDOMParser parser;
    string buf = tr.str();
    xe::MemBufInputSource 
      membuf((const XMLByte*)buf.c_str(), buf.length(), "memory_buffer");
    parser.parse(membuf);
    xe::DOMDocument* doc = parser.getDocument();
    
    //class Xml2AstVisitor visit(prj->get_file(0).getFileName(),ofn.c_str(),prj);
    //unlink(ofn.c_str());
    class xevxml::Xml2AstVisitor visit(ofn.c_str(),ofn.c_str(),prj);
    //class xevxml::Xml2AstVisitor visit("dummy.c",ofn.c_str(),prj);
    visit.visit(doc,0);
    ret = visit.getSgFile();
  }
  catch (...){
    return 0;
  }
  return ret;
}

namespace xevxml {
SgFile* Xml2Ast(stringstream& str, SgProject* prj, string ofn)
{
  SgFile* ret = 0;
  if((ret=Xml2AstDOMVisit(str,prj,ofn))==0){
    cerr << "Error: XML parsing failed" << endl;
    ABORT();
  }
  // Other terminations and cleanup.
  return ret;
}

}



using namespace xevxml;

Xml2AstVisitor::
Xml2AstVisitor(const string& ifn, const string& ofn, SgProject* prj)
{
  _file = isSgSourceFile(&prj->get_file(0));
  //_file = isSgSourceFile(sb::buildFile(ifn,ofn,prj));
  //_file = new SgSourceFile();
  if(_file==0){ ABORT(); }

#if 0
  if(ofn[ofn.size()-1] !='c'){
    //_file->get_project()->set_Fortran_only(true);
    _file->set_outputLanguage(SgFile::e_Fortran_output_language);
    //file->set_outputFormat(SgFile::e_fixed_form_output_format);
    _file->set_outputFormat(SgFile::e_free_form_output_format);
    _file->set_sourceFileUsesFortran90FileExtension(true);
    SageBuilder::symbol_table_case_insensitive_semantics = true;
    cerr << "Output Language: Fortran" << endl;
  }
  else {
    _file->set_outputLanguage(SgFile::e_C_output_language);
    cerr << "Output Language: C" << endl;
  }
#endif
}

Xml2AstVisitor::
~Xml2AstVisitor() {}

SgNode* 
Xml2AstVisitor::visit(xe::DOMNode* node, SgNode* astParent)
{
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
      string nname = xe::XMLString::transcode(node->getNodeName());
      
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

      VISIT(SgPlusPlusOp);
      VISIT(SgMinusOp);
      VISIT(SgAssignOp);
      VISIT(SgPlusAssignOp);
      VISIT(SgMultAssignOp);
      VISIT(SgAddOp);
      VISIT(SgLessThanOp);

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


      if( nname != "PreprocessingInfo" ) {
        cerr << "unknown AST node found " << nname << endl;
        ABORT();
      }
    }
  }
  return 0;
}

SgNode* 
Xml2AstVisitor::visitSgSourceFile(xe::DOMNode* node, SgNode* astParent)
{
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      this->visit(child,_file);
    }
    child=child->getNextSibling();
  } 
  return _file;
}

SgNode* 
Xml2AstVisitor::visitSgGlobal(xe::DOMNode* node, SgNode* astParent)
{
  SgGlobal* ret = _file->get_globalScope();
  //  SgGlobal* ret = si::getFirstGlobalScope(_file->get_project());
  sb::pushScopeStack(ret);

  //cerr << ret->get_numberOfTraversalSuccessors() << endl;
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgStatement* astchild = isSgStatement(this->visit(child,astParent));
      if(astchild) {
	si::appendStatement (astchild,isSgScopeStatement(ret));

      }
    }
    child=child->getNextSibling();
  } 
  sb::popScopeStack();
  //cerr << ret->unparseToString() ;

  //cerr << ret->get_numberOfTraversalSuccessors() << endl;
#if 0
  for(int i(0);i<ret->get_numberOfTraversalSuccessors();i++){
    cerr << i << ":" << ret->get_traversalSuccessorByIndex(i)->class_name() << endl;
  }
  cerr << _file->get_numberOfTraversalSuccessors() << endl;
#endif
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgPragmaDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgPragmaDeclaration* ret = 0;
  SgPragma* pr = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if(pr==0)
	pr = isSgPragma(astchild);
    }
    child=child->getNextSibling();
  } 
  if(pr) {
    ret = sb::buildPragmaDeclaration(pr->get_pragma()); 
  }
  else ABORT();
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgPragma(xe::DOMNode* node, SgNode* astParent)
{
  SgPragma* ret = 0;
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* att = 0;
  string line;

  if(amap) {
    att=amap->getNamedItem(xe::XMLString::transcode("pragma"));
    if(att)
      line = xe::XMLString::transcode(att->getNodeValue());
  }

  if(line.size()) {
    ret = sb::buildPragma(line);
  }
  else ABORT();
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgVariableDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgVariableDeclaration*                    ret  = 0;
  SgInitializedName*                        name = 0;
  SgInitializedName*                        tmp  = 0;
  //SgClassDeclaration*                       cls  = 0;
  SgDeclarationStatement*                   cls  = 0;
  Rose_STL_Container<SgInitializedName*>    varList;

  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* satt = 0;
  string            storage;
  string            bitstr;
  unsigned long     bit = 0;

  if(amap) {
    satt=amap->getNamedItem(xe::XMLString::transcode("modifier"));
    if(satt)
      storage = xe::XMLString::transcode(satt->getNodeValue());
    
    satt=amap->getNamedItem(xe::XMLString::transcode("bitfield"));
    if(satt)
      bitstr = xe::XMLString::transcode(satt->getNodeValue());
  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);

      tmp = isSgInitializedName(astchild);

      if( tmp ) {
        name = tmp;
        varList.push_back( name );
      }

      if( cls==0 ) {
        cls = isSgDeclarationStatement(astchild);
      }
    }
    child=child->getNextSibling();
  } 
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

    if( varList.size() > 1 )  
      ret->get_variables() = varList;

  }
  else ABORT();


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
Xml2AstVisitor::visitSgFunctionDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgFunctionDeclaration*   ret = 0;
  SgFunctionParameterList* lst = 0;
  //SgFunctionDefinition*    def = 0;
  SgBasicBlock*            def = 0;
  SgType*                  typ = 0;

  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* satt = 0;
  string storage,name;
  
  if(amap) {
    satt=amap->getNamedItem(xe::XMLString::transcode("modifier"));
    if(satt)
      storage = xe::XMLString::transcode(satt->getNodeValue());
    satt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(satt)
      name = xe::XMLString::transcode(satt->getNodeValue());
  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if(lst==0)
	lst = isSgFunctionParameterList(astchild);
      if(def==0)
	def = isSgBasicBlock(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
    child=child->getNextSibling();
  } 
  if(lst) {
    if(def)
      ret 
	= sb::buildDefiningFunctionDeclaration(SgName(name.c_str()), typ, lst);
    else 
      ret 
	= sb::buildNondefiningFunctionDeclaration(SgName(name.c_str()), typ, lst);
  }
  else ABORT();

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
  else 
    ((ret->get_declarationModifier()).get_storageModifier()).setDefault();
    
  if( def == 0 ) {
    SgFunctionDefinition* def 
      = new SgFunctionDefinition(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
    ret->set_definition( def );
    def->set_parent(ret);
    ret->get_declarationModifier().get_storageModifier().setExtern();
  }
  else
    si::replaceStatement(ret->get_definition()->get_body(),def,true);
 return ret;
}

SgNode* 
Xml2AstVisitor::visitSgProcedureHeaderStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgProcedureHeaderStatement*     ret = 0;
  SgType*                         typ = 0;
  SgFunctionParameterList*        lst = 0;
  SgScopeStatement*               scope = isSgScopeStatement(_file->get_globalScope());
  SgBasicBlock*                   def = 0;
  SgFunctionDefinition*           fdf = 0;
  
  xe::DOMNamedNodeMap*            amap = node->getAttributes();
  xe::DOMNode*                    satt = 0;
  string                          name;
  stringstream                    val;
  int                             kind;
  
  if(amap) {
    satt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(satt)
      name = xe::XMLString::transcode(satt->getNodeValue());
    
    satt=amap->getNamedItem(xe::XMLString::transcode("subprogram_kind"));
    if(satt) {
      val << xe::XMLString::transcode(satt->getNodeValue());
      val >> kind;
    }
  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if(lst==0)
	lst = isSgFunctionParameterList(astchild);
      if(typ==0)
	typ = isSgType(astchild);
      if(def==0)
	def = isSgBasicBlock(astchild);
      if(fdf==0)
	fdf = isSgFunctionDefinition(astchild);
    }
    child=child->getNextSibling();
  }
  
  if(lst){
    if( kind != SgProcedureHeaderStatement::e_block_data_subprogram_kind ){
      ret = sb::buildProcedureHeaderStatement( (const char*)(name.c_str()), typ, lst,
		       (SgProcedureHeaderStatement::subprogram_kind_enum)kind , scope);
    }
    else
      {
	// add (block data) 0828
	SgFunctionType* functionType = new SgFunctionType(SgTypeVoid::createType(), false);
	fdf = new SgFunctionDefinition( Sg_File_Info::generateDefaultFileInfoForTransformationNode(),def );
	ret = new SgProcedureHeaderStatement( Sg_File_Info::generateDefaultFileInfoForTransformationNode(),SgName(name), functionType,  fdf );
	fdf->set_parent(ret);
	ret->set_subprogram_kind( (SgProcedureHeaderStatement::subprogram_kind_enum)kind );
      }
  }
  else ABORT();
  
  if(def)
    si::replaceStatement( ret->get_definition()->get_body(),def,true );

  return ret;

}

SgNode* 
Xml2AstVisitor::visitSgFunctionParameterList(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFunctionParameterList* ret = sb::buildFunctionParameterList();
  SgInitializedName* ini=0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if((ini = isSgInitializedName(astchild)) != 0 ){
	si::appendArg(ret,ini);
      }
    }
    child=child->getNextSibling();
  } 
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgBasicBlock(xe::DOMNode* node, SgNode* astParent)
{
  SgBasicBlock* ret = sb::buildBasicBlock();
  sb::pushScopeStack(ret);
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if(isSgStatement(astchild)){
	//SageInterface::appendStatement(isSgStatement(astchild));
	ret->append_statement(isSgStatement(astchild));
      }
    }
    child=child->getNextSibling();
  }
  sb::popScopeStack();

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgExprStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgExprStatement* ret = 0;
  
  SgExpression* exp = 0;
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if(exp==0)
                exp = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }
  ret = sb::buildExprStatement(exp);
  //checkPreprocInfo(node,ret);                     // delete (0717)
  
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino   = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgForStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForStatement* ret = 0;
  SgStatement*    ini = 0;
  SgStatement*    tst = 0;
  SgExpression*   inc = 0;
  SgStatement*    bdy = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
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
    child=child->getNextSibling();
  } 
  ret = sb::buildForStatement(ini,tst,inc,bdy);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgForInitStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForInitStatement* ret = 0;
  SgStatement* stmt  = 0;
  SgStatementPtrList lst;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if((stmt = isSgStatement(astchild))!=0)
	lst.push_back(stmt);
    }
    child=child->getNextSibling();
  } 
  //lst could be empty
  ret = sb::buildForInitStatement(lst);

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgIfStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgIfStmt* ret = 0;
  SgStatement* tstmt  = 0;
  SgStatement* fstmt  = 0;
  SgExprStatement* cond  = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      /* assuming these stmts appear in this order */
      if(cond==0)
	cond = isSgExprStatement(astchild);
      else if (tstmt==0)
	tstmt = isSgStatement(astchild);
      else if (fstmt==0)
	fstmt = isSgStatement(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildIfStmt(cond,tstmt,fstmt);

  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino   = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	//s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }
    
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgWhileStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgWhileStmt*          ret   = 0;
  SgExprStatement*      cond  = 0;
  SgStatement*          tstmt = 0;
  SgStatement*          fstmt = 0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      /* assuming these stmts appear in this order */
      if(cond==0)
        cond = isSgExprStatement(astchild);
      else if (tstmt==0)
        tstmt = isSgStatement(astchild);
      else if (fstmt==0)
        fstmt = isSgStatement(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildWhileStmt(cond,tstmt,fstmt);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgDoWhileStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDoWhileStmt*        ret  = 0;
  SgStatement*          body = 0;
  SgExprStatement*      cond = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      /* assuming these stmts appear in this order */
      if(body==0)
        body = isSgStatement(astchild);
      else if (cond==0)
        cond = isSgExprStatement(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildDoWhileStmt(body,cond);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgConditionalExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgConditionalExp*     ret   = 0;
  SgExpression*         cond  = 0;
  SgExpression*         tstmt = 0;
  SgExpression*         fstmt = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child){
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      /* assuming these stmts appear in this order */
      if(cond==0)
        cond = isSgExpression(astchild);
      else if (tstmt==0)
        tstmt = isSgExpression(astchild);
      else if (fstmt==0)
        fstmt = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildConditionalExp(cond,tstmt,fstmt);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgSizeOfOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgSizeOfOp*           ret = 0;
  SgExpression*         exp = 0;
  SgType*               typ = 0;

  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt = 0;
  string                class_name;

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("type"));
    if(nameatt)
      class_name = xe::XMLString::transcode(nameatt->getNodeValue());
  }

  xe::DOMNode* child=node->getFirstChild();
  while(child){
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if(exp==0)
	exp = isSgExpression(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
    child=child->getNextSibling();
  }
  
  if( typ )
   ret = sb::buildSizeOfOp( typ );
  else
   ret = sb::buildSizeOfOp( exp );

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgSwitchStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgSwitchStatement*    ret  = 0;
  SgStatement*          item = 0;
  SgStatement*          body = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child)
  {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE)
    {
      SgNode* astchild = this->visit(child,ret);
      if(item==0)
        item = isSgStatement(astchild);
      else if (body==0)
        body = isSgStatement(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildSwitchStatement(item,body);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgCaseOptionStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgCaseOptionStmt*     ret  = 0;
  SgExpression*         key  = 0;
  SgStatement*          body = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child)
  {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE)
    {
      SgNode* astchild = this->visit(child,ret);
      if(key==0)
        key = isSgExpression(astchild);
      else if (body==0)
        body = isSgStatement(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildCaseOptionStmt(key,body);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgBreakStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgBreakStmt* ret = 0;
  ret = sb::buildBreakStmt();

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgContinueStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgContinueStmt* ret = sb::buildContinueStmt();
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgDefaultOptionStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDefaultOptionStmt*      ret  = 0;
  SgStatement*              body = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child){
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if (body==0)
        body = isSgStatement(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildDefaultOptionStmt(body);
  
  return ret;
}




#define VISIT_BINARY_OP(op)						\
  SgNode*								\
  Xml2AstVisitor::visitSg##op(xercesc::DOMNode* node, SgNode* astParent) \
  {									\
    SgExpression* lhs = 0;						\
    SgExpression* rhs = 0;						\
    xe::DOMNode* child=node->getFirstChild();				\
    while(child) {							\
      if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){	\
	SgNode* astchild = this->visit(child);				\
	if(lhs==0)							\
	  lhs = isSgExpression(astchild);				\
	else if(rhs==0)							\
	  rhs = isSgExpression(astchild);				\
      }									\
      child=child->getNextSibling();					\
    }									\
    if( lhs && rhs )							\
      return sb::build##op(lhs,rhs);					\
    else								\
      ABORT();								\
  }									



#define VISIT_UNARY_OP(op)						\
  SgNode*								\
  Xml2AstVisitor::visitSg##op(xercesc::DOMNode* node, SgNode* astParent) \
  {									\
    SgExpression* exp = 0;						\
    xe::DOMNamedNodeMap* amap = node->getAttributes();			\
    xe::DOMNode* modeatt = 0;						\
    stringstream mode;							\
    int imode=0;							\
    if(amap) {								\
      modeatt=amap->getNamedItem(xe::XMLString::transcode("mode"));	\
      if(modeatt)							\
	mode << xe::XMLString::transcode(modeatt->getNodeValue());	\
    }									\
    if(mode.str().size()){						\
      mode >> imode;							\
    }									\
    xe::DOMNode* child=node->getFirstChild();				\
    while(child) {							\
      if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){	\
	SgNode* astchild = this->visit(child);				\
	if(exp==0)							\
	  exp = isSgExpression(astchild);				\
      }									\
      child=child->getNextSibling();					\
    }									\
    if( exp ) 								\
      return sb::build##op(exp,						\
			   imode?SgUnaryOp::postfix:SgUnaryOp::prefix); \
    else								\
      ABORT();								\
  }									
  
#define VISIT_UNARY_P1_OP(op)						\
  SgNode*								\
  Xml2AstVisitor::visitSg##op(xercesc::DOMNode* node, SgNode* astParent) \
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

SgNode* 
Xml2AstVisitor::visitSgNullExpression(xe::DOMNode* node, SgNode* astParent)
{
  return sb::buildNullExpression();
}

SgNode* 
Xml2AstVisitor::visitSgNullStatement(xe::DOMNode* node, SgNode* astParent)
{
  return sb::buildNullStatement();
}

SgNode* 
Xml2AstVisitor::visitSgVarRefExp(xe::DOMNode* node, SgNode* astParent)
{
  xe::DOMNamedNodeMap*   amap    = node->getAttributes();
  xe::DOMNode*           nameatt = 0;
  string                 name;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }
  if(name.size())
    return sb::buildVarRefExp(name);
  else 
    ABORT();
}

SgNode* 
Xml2AstVisitor::visitSgCastExp(xe::DOMNode* node, SgNode* astParent)
{
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
}

SgNode* 
Xml2AstVisitor::visitSgReturnStmt(xe::DOMNode* node, SgNode* astParent)
{
  SgReturnStmt*        ret   = 0;
  SgExpression*        exp   = 0;
  xe::DOMNode*         child = node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if(exp==0)
	exp = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildReturnStmt(exp);

  return ret;
}

/* NOTE: this function returns SgBasicBlock */
SgNode* 
Xml2AstVisitor::visitSgFunctionDefinition(xe::DOMNode* node, SgNode* astParent)
{
  SgBasicBlock*   ret   = 0;
  xe::DOMNode*    child = node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      //SgNode* astchild = this->visit(child);
      SgNode* astchild = this->visit(child,astParent);
      if(ret == 0 ){
	ret = isSgBasicBlock(astchild);
      }
    }
    child=child->getNextSibling();
  } 
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgInitializedName(xe::DOMNode* node, SgNode* astParent)
{
  SgInitializedName* ret = 0;
  SgInitializer*     ini = 0;
  SgType*            typ = 0;
  
  xe::DOMNamedNodeMap* amap    = node->getAttributes();
  xe::DOMNode*         nameatt = 0;
  string               name;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if(ini==0)
	ini = isSgInitializer(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
    child=child->getNextSibling();
  } 

  ret = sb::buildInitializedName(name.c_str(),typ,ini);

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgAssignInitializer(xe::DOMNode* node, SgNode* astParent)
{
  SgAssignInitializer* ret = 0;
  SgExpression*        exp = 0;
  SgType*              typ = 0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if(exp==0)
	exp = isSgExpression(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
    child=child->getNextSibling();
  } 

  if(exp && typ){
    ret = sb::buildAssignInitializer(exp,typ);
  }
  else ABORT();
  
  return ret;
}

#define VISIT_VAL(valType,baseType,buildVal)				\
  SgNode* Xml2AstVisitor::						\
  visit##valType(xe::DOMNode* node, SgNode* astParent)			\
  {									\
    valType* ret = 0;							\
    xe::DOMNamedNodeMap* amap = node->getAttributes();			\
    xe::DOMNode* valatt = 0;						\
    stringstream val;							\
    if(amap) {								\
      valatt=amap->getNamedItem(xe::XMLString::transcode("value"));	\
      if(valatt)							\
	val << xe::XMLString::transcode(valatt->getNodeValue());	\
    }									\
    if(val.str().size()){						\
      baseType ival;							\
      val >> ival;							\
      ret = sb::build##buildVal(ival);					\
    }									\
    else ABORT();							\
    return ret;								\
  }


VISIT_VAL(SgBoolValExp,bool,BoolValExp);
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
Xml2AstVisitor::visitSgLabelStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgLabelStatement*         ret   = 0;
  SgStatement*              body  = 0;
  SgScopeStatement*         scope = 0;
  
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* nameatt = 0;
  string                    slabel,nlabel;
  stringstream              val;
  int                       ino   = 0;

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("slabel"));
    if(nameatt) {
      slabel = xe::XMLString::transcode(nameatt->getNodeValue());
    }

    nameatt=amap->getNamedItem(xe::XMLString::transcode("nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
    }

  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if (body==0)
        body = isSgStatement(astchild);
      if(scope==0)
        scope = isSgScopeStatement(astchild);
    }
    child=child->getNextSibling();
  }

  if( nlabel.size() ){
    ret = sb::buildLabelStatement( nlabel.c_str(), body, scope );
    SgLabelSymbol*  s = new SgLabelSymbol();
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    //s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    s->set_label_type(SgLabelSymbol::e_start_label_type);
    val << nlabel;
    val >> ino;
    s->set_numeric_label_value( ino );
    SgLabelRefExp*  l = new SgLabelRefExp( s );
    ret->set_numeric_label( l );
  }
  else if( slabel.size() ){
    ret = sb::buildLabelStatement( slabel.c_str(), body, scope );
  }
  else ABORT();

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgGotoStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgGotoStatement*      ret   = 0;
  SgLabelStatement*     label = 0;
  SgStatement*          body  = 0;
  SgScopeStatement*     scope = 0;
  SgLabelSymbol*        s     = 0;
  SgLabelRefExp*        l     = 0;

  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt = 0;
  string                s_name,n_name;
  int                   ino = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_label"));
    if(nameatt) {
      s_name = xe::XMLString::transcode(nameatt->getNodeValue());
      if( s_name.size() )
        label = sb::buildLabelStatement( s_name.c_str(), body, scope );
    }

    nameatt=amap->getNamedItem(xe::XMLString::transcode("n_label"));
    if(nameatt) {
      n_name = xe::XMLString::transcode(nameatt->getNodeValue());
      if( n_name.size() ) {
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
    }

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
Xml2AstVisitor::visitSgTypedefType(xe::DOMNode* node, SgNode* astParent)
{
  SgType* ret = 0;
  SgScopeStatement* scope = sb::topScopeStack();    //?


  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* nameatt = 0;
  string name;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("type_name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }

  if( name.size() ){
    ret = sb::buildOpaqueType( name.c_str(), scope );
  }
  else ABORT();

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgExprListExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgExprListExp*                ret = 0;
  SgExpression*                 exp  = 0;
  std::vector< SgExpression * > exprs;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
        if((exp = isSgExpression(astchild))!=0) {
          exprs.push_back(exp);
        }
    }
    child=child->getNextSibling();
  } 
  //exprs could be empty
  ret = sb::buildExprListExp( exprs );

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgFunctionRefExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFunctionRefExp* ret = 0;

  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* nameatt = 0;
  string name;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("symbol"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }
  if( name.size() )
    ret = sb::buildFunctionRefExp( SgName(name) );
  else
    ABORT();
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgFunctionCallExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFunctionCallExp*        ret  = 0;
  SgExpression*             exp  = 0;
  SgExprListExp*            para = 0;


  std::vector< SgExpression * > exprs;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if( exp==0 )
        exp = isSgExpression(astchild);
      if( para==0 )
        para = isSgExprListExp(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildFunctionCallExp( exp, para );
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgUseStatement(xe::DOMNode* node, SgNode* astParent)
{
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* nameatt = 0;
  string name;

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      //SgNode* astchild = this->visit(child);
      this->visit(child);
    }
    child=child->getNextSibling();
  } 

  if(name.size()) {
    SgUseStatement* useStmt = new SgUseStatement(astParent->get_file_info(),name,false);
    return useStmt;
  }
  else 
    ABORT();

}

SgNode* 
Xml2AstVisitor::visitSgFortranIncludeLine(xe::DOMNode* node, SgNode* astParent)
{
  SgFortranIncludeLine* ret = 0;
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* nameatt = 0;
  std::string name;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("filename"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }

  if( name.size() ){
    ret = sb::buildFortranIncludeLine( name );
  }
  else ABORT();
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgAttributeSpecificationStatement(xe::DOMNode* node, SgNode* astParent)
{
    SgAttributeSpecificationStatement*  ret = 0;
    SgExpression*                       exp = 0;

    xe::DOMNamedNodeMap*                amap = node->getAttributes();
    xe::DOMNode*                        nameatt = 0;
    stringstream                        val;
    int                                 kind;

    if(amap) {
        nameatt=amap->getNamedItem(xe::XMLString::transcode("kind"));
        if(nameatt)
            val << xe::XMLString::transcode(nameatt->getNodeValue());
        val >> kind;
    }
    else ABORT();

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


    xe::DOMNode* child=node->getFirstChild();

    switch (kind){
    case SgAttributeSpecificationStatement::e_parameterStatement:
    case SgAttributeSpecificationStatement::e_externalStatement:
    case SgAttributeSpecificationStatement::e_dimensionStatement:
    case SgAttributeSpecificationStatement::e_allocatableStatement:
      
      while(child) {
	if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
	  SgNode* astchild = this->visit(child,ret);
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
	child=child->getNextSibling();
      }
      ret = sb::buildAttributeSpecificationStatement( (SgAttributeSpecificationStatement::attribute_spec_enum)  kind  );
      elst = sb::buildExprListExp( lst );
      elst->set_parent(ret);
      ret->set_parameter_list( elst );
      break;
      
    case SgAttributeSpecificationStatement::e_dataStatement :

      ret = new SgAttributeSpecificationStatement( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
      ret->set_attribute_kind((SgAttributeSpecificationStatement::attribute_spec_enum)  kind);
      
      while(child) {
	if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
	  SgNode* astchild = this->visit(child,ret);
	  dataGroup = isSgDataStatementGroup(astchild);
	  if( dataGroup ) {
	    
	    localList.push_back(dataGroup);
	    dataGroup->set_parent(ret);
	  }
	}
	child=child->getNextSibling();
      }
      for (size_t i = 0; i<localList.size(); i++)
	ret->get_data_statement_group_list().push_back(localList[i]);
      break;
      
    case SgAttributeSpecificationStatement::e_saveStatement :
      ret = new SgAttributeSpecificationStatement( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
      ret->set_attribute_kind((SgAttributeSpecificationStatement::attribute_spec_enum)  kind);
      
      while(child) {
	if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
	  SgNode* astchild = this->visit(child,ret);
	  str = isSgStringVal(astchild);
	  if( str ) {
	    slst.push_back(  str->get_value() );
	  }
	}
	child=child->getNextSibling();
      }
      ret->get_name_list() = slst;
      break;

    default:
      ABORT();
      break;
    }

    return ret;
}

SgNode* 
Xml2AstVisitor::visitSgImplicitStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgImplicitStatement*      ret   = new SgImplicitStatement( astParent->get_file_info() , true );
  SgInitializedName*        inam  = 0;
  SgInitializedNamePtrList  lst;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if((inam = isSgInitializedName(astchild))!=0)
        lst.push_back(inam);
    }
    child=child->getNextSibling();
  }

  if(!lst.empty()) {
    ret->set_implicit_none( false );
    ret->get_variables() = lst;
  }

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgFortranDo(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFortranDo*              ret;
  SgExpression*             ini  = 0;
  SgExpression*             bnd  = 0;
  SgExpression*             inc  = 0;
  SgBasicBlock*             body = 0;

  xe::DOMNamedNodeMap*      amap = node->getAttributes();
  xe::DOMNode*              nameatt = 0;
  stringstream              val;
  int                       style = 0;
  int                       enddo = 0;
  int                       ino   = 0;
  string                    slabel,nlabel;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("style"));
    if(nameatt) {
      val << xe::XMLString::transcode(nameatt->getNodeValue());
      val >> style;
    }

    nameatt=amap->getNamedItem(xe::XMLString::transcode("end"));
    if(nameatt) {
      val.str("");
      val.clear(stringstream::goodbit);
      val << xe::XMLString::transcode(nameatt->getNodeValue());
      val >> enddo;
    }

    nameatt=amap->getNamedItem(xe::XMLString::transcode("slabel"));
    if(nameatt) {
      slabel = xe::XMLString::transcode(nameatt->getNodeValue());
    }

    nameatt=amap->getNamedItem(xe::XMLString::transcode("nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
    }

  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
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
    child=child->getNextSibling();
  }
  ret = new SgFortranDo( astParent->get_file_info(), ini,bnd,inc,body);
  ret->set_old_style( style );
  ret->set_has_end_statement( enddo );

  if( slabel.size() )
    ret->set_string_label( slabel );
  else if( nlabel.size() ) {
    SgLabelSymbol*  s = new SgLabelSymbol();
    //s->set_label_type( SgLabelSymbol::label_type_enum.e_non_numeric_label_type );
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    val.str("");
    val.clear(stringstream::goodbit);
    val << nlabel;
    val >> ino;
    s->set_numeric_label_value( ino );
    SgLabelRefExp*  l = new SgLabelRefExp( s );
    ret->set_end_numeric_label( l );
  }

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgComplexVal(xercesc::DOMNode* node, SgNode* astParent)
{
  SgComplexVal* ret = 0;
    
  SgValueExp*   real  = 0;
  SgValueExp*   imag  = 0;


  xe::DOMNode* child=node->getFirstChild();
  while(child)
  {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE)
    {
      SgNode* astchild = this->visit(child,ret);
      /* assuming these exprs appear in this order */
      if(real==0)
        real = isSgValueExp(astchild);
      else if (imag==0)
        imag = isSgValueExp(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildComplexVal( real, imag );
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgClassDeclaration(xercesc::DOMNode* node, SgNode* astParent)
{
  SgClassDeclaration*   ret;
  SgClassDefinition*    exp=0;

  SgScopeStatement* scope = sb::topScopeStack();    //?
  //SgScopeStatement* scope = sb::ScopeStack.front(); // true

  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt=0;
  string                name,val;
  int                   typ=0;

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("tag_name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());

    nameatt=amap->getNamedItem(xe::XMLString::transcode("type"));
    if(nameatt){
      val = xe::XMLString::transcode(nameatt->getNodeValue());
        typ = atoi( val.c_str() );
    }
  }
  ret = sb::buildClassDeclaration( SgName(name.c_str()), scope );
  ret->set_class_type( (SgClassDeclaration::class_types)typ  );


  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if( exp==0 )
        exp = isSgClassDefinition( astchild );
    }
    child=child->getNextSibling();
  }
  ret->set_definition( exp );

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgClassDefinition(xercesc::DOMNode* node, SgNode* astParent)
{
  SgClassDefinition*        ret = 0;
  SgClassDeclaration*       dec = isSgClassDeclaration( astParent );
  SgDeclarationStatement*   fld = 0;
  
  
  ret = sb::buildClassDefinition( dec );
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      fld = isSgDeclarationStatement(astchild);
      if( fld ) {
        ret->append_member(fld);
        fld->set_parent(ret);
      }
    }
    child=child->getNextSibling();
  }

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgClassType(xe::DOMNode* node, SgNode* astParent)
{
  SgClassType*          ret = 0;
  SgScopeStatement*     scope = sb::topScopeStack();    //?
  //SgScopeStatement* scope = sb::ScopeStack.front();
  SgClassDeclaration*   dec;

  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt=0;
  string                name,val;
  int                   typ=0;

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("tag_name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());

    nameatt=amap->getNamedItem(xe::XMLString::transcode("type"));
    if(nameatt){
      val = xe::XMLString::transcode(nameatt->getNodeValue());
        typ = atoi( val.c_str() );
    }
  }


  dec = sb::buildClassDeclaration( SgName(name.c_str()), scope );
  dec->set_class_type( (SgClassDeclaration::class_types)typ  );

  SgDeclarationStatement* ds = isSgDeclarationStatement( dec );

  ds->setForward();
  ret = new SgClassType( ds );
//ds->set_parent(ret);

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgTypedefDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgTypedefDeclaration*     ret = 0;
  SgClassDeclaration*       cls = 0;
  SgType*                   typ = 0;

  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* satt = 0;
  string name;
  
  if(amap) {
    satt=amap->getNamedItem(xe::XMLString::transcode("tag_name"));
    if(satt)
      name = xe::XMLString::transcode(satt->getNodeValue());
  }


  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if(typ==0)
        typ = isSgType(astchild);
      if(cls==0)
        cls = isSgClassDeclaration(astchild);
    }
    child=child->getNextSibling();
  }

    
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
Xml2AstVisitor::visitSgEnumDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgEnumDeclaration*    ret = 0;
  SgScopeStatement*     scope = sb::topScopeStack();    //?

  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt=0;
  string                name,val;

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("tag_name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }


  SgInitializedName*        inam  = 0;
  SgInitializedNamePtrList  lst;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if((inam = isSgInitializedName(astchild))!=0)
        lst.push_back(inam);
    }
    child=child->getNextSibling();
  }


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
Xml2AstVisitor::visitSgEnumType(xe::DOMNode* node, SgNode* astParent)
{
  SgEnumType* ret =  new SgEnumType(0);
  return ret;
}

SgNode*
Xml2AstVisitor::visitSgEnumVal(xe::DOMNode* node, SgNode* astParent)
{
  SgIntVal* ret = 0;
    
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* valatt = 0;
  stringstream val;
  if(amap) {
    valatt=amap->getNamedItem(xe::XMLString::transcode("value"));
    if(valatt)
      val << xe::XMLString::transcode(valatt->getNodeValue());
  }
  if(val.str().size()){
    int ival;
    val >> ival;
    ret = sb::buildIntVal(ival);
  }
  else ABORT();


  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgDotExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDotExp*     ret = 0;
  SgExpression* lhs = 0;
  SgExpression* rhs = 0;


  std::vector< SgExpression * > exprs;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if( lhs==0 )
        lhs = isSgExpression(astchild);
      else if( rhs==0 )
        rhs = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = new SgDotExp( lhs->get_file_info(), lhs, rhs, lhs->get_type() );
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgArrowExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgArrowExp*     ret = 0;
  SgExpression* lhs = 0;
  SgExpression* rhs = 0;


  std::vector< SgExpression * > exprs;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if( lhs==0 )
        lhs = isSgExpression(astchild);
      else if( rhs==0 )
        rhs = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = new SgArrowExp( lhs->get_file_info(), lhs, rhs, lhs->get_type() );
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgProgramHeaderStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgType*               typ = 0;
  SgFunctionDefinition* blk = 0;
  SgBasicBlock*         def = 0;
  SgFunctionParameterList*      lst = 0;


  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* nameatt = 0;
  string              name;

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }
  else ABORT();

  SgFunctionType* ftyp = new SgFunctionType(SgTypeVoid::createType(), false);
  SgProgramHeaderStatement* ret = new SgProgramHeaderStatement(astParent->get_file_info(),SgName(name.c_str()), ftyp, NULL);

  ret->set_definingDeclaration(ret);
  ret->set_scope(sb::topScopeStack());
  ret->set_parent(sb::topScopeStack());

  //SgGlobal* globalScope = isSgGlobal(sb::topScopeStack());
  //globalScope->append_statement(ret);
  //SgFunctionSymbol* symbol = new SgFunctionSymbol(ret);
  //globalScope->insert_symbol(SgName(name.c_str()), symbol);


  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if( typ==0 )
        typ = isSgType(astchild);
      if( blk==0 )
        blk = isSgFunctionDefinition(astchild);
      if(def==0) {
        def = isSgBasicBlock(astchild);
      }
      if(lst==0)
        lst = isSgFunctionParameterList(astchild);
    }
    child=child->getNextSibling();
  }

  SgFunctionDefinition* programDefinition = new SgFunctionDefinition(ret, def);

  def->setCaseInsensitive(true);
  programDefinition->setCaseInsensitive(true);

  def->set_parent(programDefinition);
  programDefinition->set_parent(ret);

  ret->set_name(SgName(name.c_str()));

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgPrintStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgPrintStatement*     ret = new SgPrintStatement(astParent->get_file_info());
  SgExprListExp*        exp = 0;
  SgExpression*         fmt = 0;
  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt = 0;
  string                nlabel;
  int                   ino   = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if( exp==0 )
	exp = isSgExprListExp(astchild);
      else if( fmt==0 )
	fmt = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }
  
  ret->set_io_stmt_list(exp);
  ret->set_format( fmt );
  exp->set_parent(ret);
  fmt->set_parent(ret);
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgAsteriskShapeExp(xe::DOMNode* node, SgNode* astParent)
{
  //SgAsteriskShapeExp* ret = new SgAsteriskShapeExp(astParent->get_file_info());
  SgAsteriskShapeExp* ret = new SgAsteriskShapeExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode());   // 0822
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgLabelRefExp(xe::DOMNode* node, SgNode* astParent)
{
  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt = 0;
  int                   ino   = 0;
  string                nlabel;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() )
        ino = atoi( nlabel.c_str() );
    }
  }

  SgLabelSymbol*  s = new SgLabelSymbol();
  //s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
  s->set_fortran_statement( new SgStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode()) );
  s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
  s->set_numeric_label_value( ino );
  SgLabelRefExp*  ret = new SgLabelRefExp( s );
  s->set_parent(ret);
  //ret->set_startOfConstruct(astParent->get_file_info());
  ret->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgFormatStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgFormatItem*           itm = 0;
  SgFormatItemList*       lst = new SgFormatItemList();
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino   = 0;
  
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if((itm = isSgFormatItem(astchild))!=0){
	lst->get_format_item_list().insert(
              //lst->get_format_item_list().begin(), itm );
              lst->get_format_item_list().end(), itm );
      }
    }
    child=child->getNextSibling();
  }

  SgFormatStatement*    ret = new SgFormatStatement(astParent->get_file_info(),lst);
  lst->set_parent( ret );
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	//s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgFormatItem(xe::DOMNode* node, SgNode* astParent)
{
  SgStringVal*          val;
  //SgExpression*         val;
  string                fmt;
  string                sgl;
  string                dbl;
  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("fmt"));
    if(nameatt) {
      fmt = xe::XMLString::transcode(nameatt->getNodeValue());
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("SingleQuote"));
    if(nameatt) {
      sgl = xe::XMLString::transcode(nameatt->getNodeValue());
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("DoubleQuote"));
    if(nameatt) {
      dbl = xe::XMLString::transcode(nameatt->getNodeValue());
    }
  }
  
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
Xml2AstVisitor::visitSgWriteStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgWriteStatement*     ret = new SgWriteStatement(astParent->get_file_info());
  SgExprListExp*        exp = 0;
  SgExpression*         fmt = 0;
  SgExpression*         unt = 0;
  
  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt = 0;
  string                nlabel;
  int                   ino   = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	//s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      //assuming these stmts appear in this order
      if( exp==0 )
	exp = isSgExprListExp(astchild);
      else if( unt==0 )
	unt = isSgExpression(astchild);
      else if( fmt==0 )
	fmt = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }
  ret->set_io_stmt_list(exp);
  ret->set_format( fmt );
  ret->set_io_statement(SgIOStatement::e_write);
  ret->set_unit(unt);
  exp->set_parent(ret);
  fmt->set_parent(ret);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgOpenStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgOpenStatement*        ret = new SgOpenStatement(astParent->get_file_info());
  SgExpression*           exp = 0;
  SgLabelRefExp*          lbl = 0;
  
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino = 0;
  int                     flg[17];
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }

    ino = 0;
    memset( flg,0,sizeof(flg) );
    nameatt=amap->getNamedItem(xe::XMLString::transcode("unit"));
    if(nameatt) {
      flg[ ino ] = 1;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("iostat"));
    if(nameatt) {
      flg[ ino ] = 2;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("err"));
    if(nameatt) {
      flg[ ino ] = 3;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("file"));
    if(nameatt) {
      flg[ ino ] = 4;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("status"));
    if(nameatt) {
      flg[ ino ] = 5;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("access"));
    if(nameatt) {
      flg[ ino ] = 6;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("form"));
    if(nameatt) {
      flg[ ino ] = 7;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("recl"));
    if(nameatt) {
      flg[ ino ] = 8;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("blank"));
    if(nameatt) {
      flg[ ino ] = 9;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("position"));
    if(nameatt) {
      flg[ ino ] = 10;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("action"));
    if(nameatt) {
      flg[ ino ] = 11;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("delim"));
    if(nameatt) {
      flg[ ino ] = 12;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("pad"));
    if(nameatt) {
      flg[ ino ] = 13;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("iomsg"));
    if(nameatt) {
      flg[ ino ] = 14;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("round"));
    if(nameatt) {
      flg[ ino ] = 15;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("sign"));
    if(nameatt) {
      flg[ ino ] = 16;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("asynchronous"));
    if(nameatt) {
      flg[ ino ] = 17;
      ino++;
    }
  }

  ino = 0;
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      
      exp = isSgExpression(astchild);
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
    child=child->getNextSibling();
  }
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgCloseStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgCloseStatement*     ret = new SgCloseStatement(astParent->get_file_info());
  SgExpression*         unt = 0;
  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt = 0;
  string                nlabel;
  int                   ino   = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	//s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if( unt==0 )
	unt = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }
  ret->set_unit(unt);
  
  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgContainsStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgContainsStatement*     ret = new SgContainsStatement(astParent->get_file_info());
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgModuleStatement(xe::DOMNode* node, SgNode* astParent)
{
  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt=0;
  string                name,val;
  int                   typ=0;


  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("tag_name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
    
    nameatt=amap->getNamedItem(xe::XMLString::transcode("type"));
    if(nameatt){
      val = xe::XMLString::transcode(nameatt->getNodeValue());
      typ = atoi( val.c_str() );
    }
  }
  
  SgModuleStatement* ret = buildModuleStatementAndDefinition(
                SgName( name.c_str() ), sb::topScopeStack());
  ret->set_file_info(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgClassDefinition*    exp=0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if( exp==0 )
        exp = isSgClassDefinition( astchild );
    }
    child=child->getNextSibling();
  }

  ret->set_class_type( (SgClassDeclaration::class_types)typ  );
  ret->set_name( SgName( name.c_str() ) );
  exp->set_parent(ret);
  ret->set_definition( exp );

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgCommonBlockObject(xercesc::DOMNode* node, SgNode* astParent)
{
  SgCommonBlockObject*  ret = 0;
  SgExprListExp*        para = 0;

  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt=0;
  string                name;


  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if( para==0 )
        para = isSgExprListExp(astchild);
    }
    child=child->getNextSibling();
  } 

  ret = sb::buildCommonBlockObject( name, para );
  para ->set_parent(ret);

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgCommonBlock(xercesc::DOMNode* node, SgNode* astParent)
{
  SgCommonBlock*        ret=0;
  SgCommonBlockObject*  obj=0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if( obj==0 )
        obj = isSgCommonBlockObject(astchild);
    }
    child=child->getNextSibling();
  } 

  ret = sb::buildCommonBlock( obj );
  obj->set_parent(ret);

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgStringVal(xercesc::DOMNode* node, SgNode* astParent)
{
  SgStringVal*            ret = 0;
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            valatt = 0;
  string                  str;
  if(amap) {
    valatt=amap->getNamedItem(xe::XMLString::transcode("value"));
    if(valatt)
      str = xe::XMLString::transcode(valatt->getNodeValue());
  }
  
  //if(str.size())                                // del (0821)
  ret = sb::buildStringVal(str);
  //else ABORT();                                 // del (0821)
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgSubscriptExpression(xercesc::DOMNode* node, SgNode* astParent)
{
  SgSubscriptExpression*      ret=0;
  SgExpression*               low=0;
  SgExpression*               upp=0;
  SgExpression*               str=0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      //assuming these stmts appear in this order
      if( low==0 )
	low = isSgExpression(astchild);
      else if( upp==0 )
	upp = isSgExpression(astchild);
      else if( str==0 )
	str = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }
  
  ret = new SgSubscriptExpression(low,upp,str);
  ret->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  low->set_parent(ret);
  upp->set_parent(ret);
  str->set_parent(ret);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgForAllStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForAllStatement*      ret 
    = new SgForAllStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExprListExp*          hed = 0;
  SgBasicBlock*           bdy = 0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if( hed==0 )
	hed = isSgExprListExp(astchild);
      else if( bdy==0 )
	bdy = isSgBasicBlock(astchild);
    }
    child=child->getNextSibling();
  }
  
  hed->set_parent(ret);
  bdy->set_parent(ret);
  bdy->setCaseInsensitive(true);

  //ret->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  hed->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  bdy->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  
  ret->set_forall_header(hed);
  ret->set_body(bdy);
  ret->set_has_end_statement(true);
  //ret->set_has_end_statement(false);

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgInterfaceStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgInterfaceStatement*      ret=0;
  SgInterfaceBody*           bdy=0;
  
  xe::DOMNamedNodeMap*  amap = node->getAttributes();
  xe::DOMNode*          nameatt=0;
  string                name,val;
  int                   typ=0;

  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
    
    nameatt=amap->getNamedItem(xe::XMLString::transcode("type"));
    if(nameatt){
      val = xe::XMLString::transcode(nameatt->getNodeValue());
      typ = atoi( val.c_str() );
    }
  }

  ret = new SgInterfaceStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
         	 SgName( name.c_str() ),(SgInterfaceStatement::generic_spec_enum)typ );
  
  xe::DOMNode* child = node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      bdy = isSgInterfaceBody(astchild);
      if( bdy ) {
	bdy->set_parent(ret);
	ret->get_interface_body_list().push_back(bdy);
      }
    }
    child=child->getNextSibling();
  }
  
  ret->set_generic_spec( (SgInterfaceStatement::generic_spec_enum) typ );
  
  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgInterfaceBody(xercesc::DOMNode* node, SgNode* astParent)
{
  SgInterfaceBody*                ret=0;
  SgFunctionDeclaration*          bdy=0;
  SgProcedureHeaderStatement*                   def = 0;
  
  
  xe::DOMNamedNodeMap*            amap = node->getAttributes();
  xe::DOMNode*                    nameatt=0;
  string                          name,val;
  
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("fnc_name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }
  
  //ret = new SgInterfaceBody(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  ret = new SgInterfaceBody( astParent->get_file_info() );

/*--- 2013.08.05 delete
    if( name.size() ) {
        ret->set_function_name( SgName( name.c_str() ) );
        ret->set_use_function_name( true );
    }
---*/
        
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if( bdy==0 )
	bdy = isSgFunctionDeclaration(astchild);
      if(def==0)
	def = isSgProcedureHeaderStatement(astchild);
      
    }
    child=child->getNextSibling();
  }
  
  ret->set_functionDeclaration( bdy );
  bdy->set_parent(ret);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgReadStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgReadStatement*        ret = new SgReadStatement(astParent->get_file_info());
  SgExprListExp*          exp = 0;
  SgExpression*           fmt = 0;
  SgExpression*           unt = 0;
  
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino   = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	//s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      //assuming these stmts appear in this order
      if( exp==0 )
	exp = isSgExprListExp(astchild);
      else if( unt==0 )
	unt = isSgExpression(astchild);
      else if( fmt==0 )
	fmt = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }
  
  ret->set_io_stmt_list(exp);
  ret->set_format( fmt );
  ret->set_unit(unt);
  exp->set_parent(ret);
  fmt->set_parent(ret);

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgEntryStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgEntryStatement*       ret = 0;
  SgFunctionType*         typ = 0;
  SgFunctionDefinition*   def = 0;
  
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  name;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if( typ==0 )
	typ = isSgFunctionType(astchild);
      
      if( def==0 )
	def = isSgFunctionDefinition(astchild);
    }
    child=child->getNextSibling();
  }
  
  ret = new SgEntryStatement( astParent->get_file_info(),
			      SgName( name.c_str() ),
			      typ,
			      def  );
  ret->set_scope(def);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgAllocateStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgAllocateStatement*    ret=0;
  SgExprListExp*          exp=0;
  
  ret = new SgAllocateStatement(astParent->get_file_info());
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if( exp==0 )
	exp = isSgExprListExp(astchild);
    }
    child=child->getNextSibling();
  }
  
  ret->set_expr_list(exp);
  exp->set_parent(ret);
  
  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgDeallocateStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDeallocateStatement*    ret=0;
  SgExprListExp*          exp=0;
  
  ret = new SgDeallocateStatement(astParent->get_file_info());
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if( exp==0 )
	exp = isSgExprListExp(astchild);
    }
    child=child->getNextSibling();
    }
  
  //printf( "ret=%p,exp=%p\n",ret,exp);
  ret->set_expr_list(exp);
  exp->set_parent(ret);
  
  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgArithmeticIfStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgArithmeticIfStatement*    ret = new SgArithmeticIfStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*               cnd = 0;
  SgLabelRefExp*              les = 0;
  SgLabelRefExp*              eql = 0;
  SgLabelRefExp*              grt = 0;
  
  xe::DOMNode* child = node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
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
    child=child->getNextSibling();
  }

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
Xml2AstVisitor::visitSgStopOrPauseStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgStopOrPauseStatement*     ret=0;
  SgExpression*               cod=0;
  
  xe::DOMNamedNodeMap*        amap = node->getAttributes();
  xe::DOMNode*                nameatt=0;
  string                      val;
  int                         typ=0;
    

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("type"));
    if(nameatt){
      val = xe::XMLString::transcode(nameatt->getNodeValue());
      typ = atoi( val.c_str() );
    }
  }
  
  ret = new SgStopOrPauseStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  ret->set_stop_or_pause( (SgStopOrPauseStatement::stop_or_pause_enum) typ );
  
  xe::DOMNode* child = node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if( cod )
	cod = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }

  if( cod==0 )
    cod = new SgNullExpression(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  
  ret->set_code( cod );
  cod->set_parent( ret );
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgElseWhereStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgElseWhereStatement* ret =  
    new SgElseWhereStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  SgExpression*         cond  = 0;
  SgBasicBlock*         body  = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE) {
      SgNode* astchild = this->visit(child,ret);
      if(cond==0)
        cond = isSgExpression(astchild);
      else if (body==0)
        body = isSgBasicBlock(astchild);
    }
    child=child->getNextSibling();
  } 
  ret->set_condition( cond );
  ret->set_body( body );
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgWhereStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgWhereStatement* ret =  
    new SgWhereStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
    
  SgExpression*         cond = 0;
  SgBasicBlock*         body = 0;
  SgElseWhereStatement* elsw = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      /* assuming these stmts appear in this order */
      if(cond==0)
        cond = isSgExpression(astchild);
      else if (body==0)
        body = isSgBasicBlock(astchild);
      else if (elsw==0)
        elsw = isSgElseWhereStatement(astchild);
    }
    child=child->getNextSibling();
  } 
  ret->set_condition( cond );
  ret->set_body( body );
  ret->set_elsewhere( elsw );
  ret->set_has_end_statement(true);

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgNullifyStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgNullifyStatement*   ret = 
    new SgNullifyStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  SgExprListExp*        plst = 0;


  std::vector< SgExpression * > exprs;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if( plst==0 )
        plst = isSgExprListExp(astchild);
    }
    child=child->getNextSibling();
  } 
  ret->set_pointer_list( plst );
  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgBackspaceStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgBackspaceStatement*   ret = 
    new SgBackspaceStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*           unt = 0;
  SgLabelRefExp*          err = 0;
  
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino   = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	//s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
	//assuming these stmts appear in this order
      if( unt==0 )
	unt = isSgExpression(astchild);
      else if( err==0 )
	err = isSgLabelRefExp(astchild);
    }
    child=child->getNextSibling();
    }
  
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
Xml2AstVisitor::visitSgEndfileStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgEndfileStatement*     ret = 
    new SgEndfileStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*           unt = 0;
  SgLabelRefExp*          err = 0;
  
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino   = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	//s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }

  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if( unt==0 )
	unt = isSgExpression(astchild);
      else if( err==0 )
	err = isSgLabelRefExp(astchild);
    }
    child=child->getNextSibling();
  }

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
Xml2AstVisitor::visitSgRewindStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgRewindStatement*      ret = 
    new SgRewindStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*           unt = 0;
  
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino   = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	//s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }

  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      //assuming these stmts appear in this order
      if( unt==0 )
	unt = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }
  
  if( unt ) {
    ret->set_unit( unt );
    unt->set_parent(ret);
    unt->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  }
  
  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgInquireStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgInquireStatement*     ret = 
    new SgInquireStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*           exp = 0;
  SgLabelRefExp*          lbl = 0;
  SgVarRefExp*            var = 0;
  SgExprListExp*          ele = 0;
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino = 0;
  int                     flg[33];
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
    
    ino = 0;
    memset( flg,0,sizeof(flg) );
    nameatt=amap->getNamedItem(xe::XMLString::transcode("iolength"));
    if(nameatt) {
      flg[ ino ] = 1;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("unit"));
    if(nameatt) {
      flg[ ino ] = 2;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("iostat"));
    if(nameatt) {
      flg[ ino ] = 3;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("err"));
    if(nameatt) {
      flg[ ino ] = 4;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("iomsg"));
    if(nameatt) {
      flg[ ino ] = 5;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("file"));
    if(nameatt) {
      flg[ ino ] = 6;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("access"));
    if(nameatt) {
      flg[ ino ] = 7;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("form"));
    if(nameatt) {
      flg[ ino ] = 8;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("recl"));
    if(nameatt) {
      flg[ ino ] = 9;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("blank"));
    if(nameatt) {
      flg[ ino ] = 10;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("exist"));
    if(nameatt) {
      flg[ ino ] = 11;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("opened"));
    if(nameatt) {
      flg[ ino ] = 12;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("number"));
    if(nameatt) {
      flg[ ino ] = 13;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("named"));
    if(nameatt) {
      flg[ ino ] = 14;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(nameatt) {
      flg[ ino ] = 15;
      ino++;
        }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("sequential"));
    if(nameatt) {
      flg[ ino ] = 16;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("direct"));
    if(nameatt) {
      flg[ ino ] = 17;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("formatted"));
    if(nameatt) {
      flg[ ino ] = 18;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("unformatted"));
    if(nameatt) {
      flg[ ino ] = 19;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("nextrec"));
    if(nameatt) {
      flg[ ino ] = 20;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("position"));
    if(nameatt) {
      flg[ ino ] = 21;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("action"));
    if(nameatt) {
      flg[ ino ] = 22;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("access"));
    if(nameatt) {
      flg[ ino ] = 23;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("read"));
    if(nameatt) {
      flg[ ino ] = 24;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("write"));
    if(nameatt) {
      flg[ ino ] = 25;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("readwrite"));
    if(nameatt) {
      flg[ ino ] = 26;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("delim"));
    if(nameatt) {
      flg[ ino ] = 27;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("pad"));
    if(nameatt) {
      flg[ ino ] = 28;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("asynchronous"));
    if(nameatt) {
      flg[ ino ] = 29;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("decimal"));
    if(nameatt) {
      flg[ ino ] = 30;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("stream"));
    if(nameatt) {
      flg[ ino ] = 31;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("size"));
    if(nameatt) {
      flg[ ino ] = 32;
      ino++;
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("pending"));
    if(nameatt) {
      flg[ ino ] = 33;
      ino++;
    }
  }
  
  ino = 0;
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      
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
    child=child->getNextSibling();
  }

  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgFlushStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgFlushStatement*      ret =
    new SgFlushStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExpression*           unt = 0;
  
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino   = 0;

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	//s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if( unt==0 )
	unt = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }
  
  if( unt ) {
    ret->set_unit( unt );
    unt->set_parent(ret);
    unt->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  }

  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgNamelistStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgNamelistStatement*    ret = 
    new SgNamelistStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgNameGroup*            grp = new SgNameGroup();
  SgNameGroupPtrList      glst;
  SgStringVal*            str = 0;
  
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  name;
  string                  nlabel;
  int                     ino   = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	//s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
    
    nameatt=amap->getNamedItem(xe::XMLString::transcode("group"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }
  
  Rose_STL_Container<std::string> slst;
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      str = isSgStringVal(astchild);
      if( str ) {
	slst.push_back(  str->get_value() );
      }
    }
    child=child->getNextSibling();
  }

  grp->set_group_name( name );
  grp->get_name_list() = slst;
  grp->set_parent(ret);
  ret->get_group_list().push_back( grp );
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgDerivedTypeStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgDerivedTypeStatement*     ret = 0;
  SgClassDefinition*          exp = 0;
  //SgScopeStatement* scope = sb::topScopeStack();    //?
  SgScopeStatement* scope = sb::ScopeStack.front();
  
  xe::DOMNamedNodeMap*        amap = node->getAttributes();
  xe::DOMNode*                nameatt=0;
  string                      name,val;
  int                         typ=0;
  
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("tag_name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
    
    nameatt=amap->getNamedItem(xe::XMLString::transcode("type"));
    if(nameatt){
      val = xe::XMLString::transcode(nameatt->getNodeValue());
      typ = atoi( val.c_str() );
    }
  }
  
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if( exp==0 )
	exp = isSgClassDefinition( astchild );
    }
    child=child->getNextSibling();
  }

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
Xml2AstVisitor::visitSgComputedGotoStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgExprListExp*          exp = 0;
  SgExpression*           var = 0;
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino   = 0;
  
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if( exp==0 )
	exp = isSgExprListExp(astchild);
      else if( var==0 )
	var = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }
    
  SgComputedGotoStatement*    ret = 
    new SgComputedGotoStatement(astParent->get_file_info(), exp,var );
  
  exp->set_parent(ret);
  var->set_parent(ret);
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }
  
  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgPointerDerefExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgPointerDerefExp*    ret = 0;
  SgExpression*         exp = 0;
  SgType*               typ = 0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child){
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE) {
      SgNode* astchild = this->visit(child,astParent);
      if(typ==0)
	typ = isSgType(astchild);
      if(exp==0)
	exp = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }
  
  ret = new SgPointerDerefExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
			      exp, typ );

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgVarArgStartOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgVarArgStartOp*  ret = 0;
  SgExpression*     lhs = 0;
  SgExpression*     rhs = 0;
  
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if( lhs==0 )
        lhs = isSgExpression(astchild);
      else if( rhs==0 )
        rhs = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = new SgVarArgStartOp( Sg_File_Info::generateDefaultFileInfoForTransformationNode(), lhs, rhs, lhs->get_type() );
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgVarArgOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgVarArgOp*       ret = 0;
  SgExpression*     lhs = 0;
  SgType*           typ = 0;
  
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if( typ==0 )
        typ = isSgType(astchild);
      else if( lhs==0 )
        lhs = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = new SgVarArgOp( Sg_File_Info::generateDefaultFileInfoForTransformationNode(), lhs, typ );
  //ret = sb::buildVarArgOp_nfi( lhs,lhs->get_type() );
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgVarArgEndOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgVarArgEndOp*  ret = 0;
  SgExpression*     lhs = 0;
  
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if( lhs==0 )
        lhs = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = new SgVarArgEndOp( Sg_File_Info::generateDefaultFileInfoForTransformationNode(), lhs, lhs->get_type() );
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgEquivalenceStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgEquivalenceStatement* ret = new SgEquivalenceStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  SgExprListExp*          lst = 0;
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  nlabel;
  int                     ino   = 0;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("s_nlabel"));
    if(nameatt) {
      nlabel = xe::XMLString::transcode(nameatt->getNodeValue());
      if( nlabel.size() ){
	ino = atoi( nlabel.c_str() );
	SgLabelSymbol*  s = new SgLabelSymbol();
	s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
	s->set_label_type(SgLabelSymbol::e_start_label_type);
	s->set_numeric_label_value( ino );
	SgLabelRefExp*  l = new SgLabelRefExp( s );
	SgStatement*    stmt = isSgStatement(ret);
	stmt->set_numeric_label(l);
	l->set_parent(ret);
      }
    }
  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      //assuming these stmts appear in this order
      if( lst==0 )
	lst = isSgExprListExp(astchild);
    }
    child=child->getNextSibling();
  }
  
  if( lst ) {
    ret->set_equivalence_set_list(lst);
    lst->set_parent(ret);
    lst->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  }
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgAsmStmt(xe::DOMNode* node, SgNode* astParent)
{
  SgAsmStmt*     ret = 0;
  
  xe::DOMNamedNodeMap*        amap = node->getAttributes();
  xe::DOMNode*                nameatt=0;
  string                      asm_code;
  string                      vol;
  int                         typ=0;
  
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("asm_code"));
    if(nameatt)
      asm_code = xe::XMLString::transcode(nameatt->getNodeValue());
    
    nameatt=amap->getNamedItem(xe::XMLString::transcode("volatile"));
    if(nameatt){
      vol = xe::XMLString::transcode(nameatt->getNodeValue());
      typ = atoi( vol.c_str() );
    }
  }
  else ABORT();
  
  ret = sb::buildAsmStatement( asm_code );
  ret->set_isVolatile( typ );

  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgAggregateInitializer(xe::DOMNode* node, SgNode* astParent)
{
  SgAggregateInitializer* ret = 0;
  SgExprListExp*          lst = 0;
  SgType*                 typ = 0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if( typ==0 )
	typ = isSgType(astchild);
      if( lst==0 )
	lst = isSgExprListExp(astchild);
    }
    child=child->getNextSibling();
  } 
  
  ret = sb::buildAggregateInitializer( lst,typ );
  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgFunctionType(xe::DOMNode* node, SgNode* astParent)
{
  SgFunctionType*                 ret = 0;
  SgType*                         typ = 0;
  SgFunctionParameterTypeList*    lst = 0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if( typ==0 )
	typ = isSgType(astchild);
      if( lst==0 )
	lst = isSgFunctionParameterTypeList(astchild);
    }
    child=child->getNextSibling();
  } 
  
  if( typ==0 ) {
    typ = isSgType( sb::buildIntType() );
  }
  ret = sb::buildFunctionType( typ,lst );

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgFunctionParameterTypeList(xe::DOMNode* node, SgNode* astParent)
{
  //SgFunctionParameterTypeList*    ret = 0;
  SgExprListExp*                  exp = 0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if( exp==0 )
	exp = isSgExprListExp(astchild);
    }
    child=child->getNextSibling();
  } 
  
  if( exp )
    exp = new SgExprListExp( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  
  return  sb::buildFunctionParameterTypeList( exp );
}

SgNode* 
Xml2AstVisitor::visitSgPointerAssignOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgPointerAssignOp*    ret = 0;
  SgExpression*         lhs = 0;
  SgExpression*         rhs = 0;
  
  
  std::vector< SgExpression * > exprs;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if( lhs==0 )
        lhs = isSgExpression(astchild);
      else if( rhs==0 )
        rhs = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = new SgPointerAssignOp( Sg_File_Info::generateDefaultFileInfoForTransformationNode(), lhs, rhs, rhs->get_type() );
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgCompoundInitializer(xe::DOMNode* node, SgNode* astParent)
{
  SgCompoundInitializer*  ret = 0;
  SgExprListExp*          lst = 0;
  SgType*                 typ = 0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if( typ==0 )
	typ = isSgType(astchild);
      if( lst==0 )
	lst = isSgExprListExp(astchild);
    }
        child=child->getNextSibling();
  } 
  
  ret = sb::buildCompoundInitializer( lst,typ );
  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgImpliedDo(xercesc::DOMNode* node, SgNode* astParent)
{
  SgImpliedDo*        ret = 0;
  SgExpression*       ini = 0;
  SgExpression*       las = 0;
  SgExpression*       inc = 0;
  SgExprListExp*      lst = 0;
  SgScopeStatement*   scp = 0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
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
    child=child->getNextSibling();
  }
  
  ret = new SgImpliedDo(  Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
			  ini,las,inc,lst,scp);
  ini->set_parent(ret);
  las->set_parent(ret);
  inc->set_parent(ret);
  lst->set_parent(ret);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgDataStatementGroup(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDataStatementGroup*   ret = new SgDataStatementGroup();
  SgDataStatementObject*  obj = new SgDataStatementObject();
  SgDataStatementValue*   val = new SgDataStatementValue();
  SgExprListExp*          nam = 0;
  SgExpression*           exp = 0;
  SgExprListExp*          exprList = new SgExprListExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
  
  val->set_initializer_list( exprList );
  exprList->set_parent(val);
  val->set_data_initialization_format( SgDataStatementValue::e_explict_list );
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
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
    }
    child=child->getNextSibling();
  }
  
  ret->get_object_list().push_back(obj);
  ret->get_value_list().push_back(val);
  
  return ret;
}


