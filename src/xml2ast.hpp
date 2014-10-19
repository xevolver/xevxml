/**
 * @file     xml2ast.hpp
 * @brief    Classes for converting an XML document to ROSE AST.
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
#ifndef ___XML2AST_H___
#define ___XML2AST_H___
#include "common.hpp"
#include <xevxml.hpp>
#include <xmlutils.hpp>

//#include <string>

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
//#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
//#include <xercesc/dom/DOMLSInput.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

namespace XevXML {

class XevXmlVisitor
{
  XevConversionHelper* _help;
  SgSourceFile*        _file;
  SgProject*           _prj;

public:
  XevXmlVisitor(SgProject* =0, XevConversionHelper* =0);
  ~XevXmlVisitor();

#if 0
  void visit2(xercesc::DOMNode* node,int depth=0)
  {
    if(node) {
      xercesc::DOMNode* child=node->getFirstChild();
      while(child){
	std::cerr<< depth 
		 << xercesc::XMLString::transcode(child->getNodeName()) 
		 << std::endl;
	this->visit2(child,depth+1);
	xercesc::DOMNode* next=child->getNextSibling();
	child=next;
      }
    }
  }
#endif

  SgNode* visit(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgSourceFile(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgGlobal(xercesc::DOMNode* node, SgNode* astParent=0);


  SgNode* visitSgPragmaDeclaration(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgPragma(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgVariableDeclaration(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFunctionDeclaration(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgProcedureHeaderStatement(xercesc::DOMNode* node, SgNode* astParent=0);


  SgNode* visitSgFunctionParameterList(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgBasicBlock(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgExprStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgForStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgForInitStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgIfStmt(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgWhileStmt(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDoWhileStmt(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgSwitchStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgCaseOptionStmt(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgBreakStmt(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDefaultOptionStmt(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgContinueStmt(xercesc::DOMNode* node, SgNode* astParent=0);



  SgNode* visitSgPlusPlusOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgMinusOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgPlusAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgMultAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgAddOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLessThanOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgGreaterThanOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLessOrEqualOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgGreaterOrEqualOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgNotEqualOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgMinusMinusOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgNotOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgOrOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgEqualityOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgAndOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgModOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDivideOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgMultiplyOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgMinusAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgIorAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgAndAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgXorAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgBitComplementOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLshiftOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgRshiftOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgConcatenationOp(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgConditionalExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgSubtractOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDivAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgModAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgBitAndOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgBitOrOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgBitXorOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgCommaOpExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgPntrArrRefExp(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgSizeOfOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgArrayType(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgAddressOfOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgUnaryAddOp(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgNullExpression(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgNullStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgVarRefExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgCastExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgColonShapeExp(xercesc::DOMNode* node, SgNode* astParent=0);


  SgNode* visitSgReturnStmt(xercesc::DOMNode* node, SgNode* astParent=0);

  /* NOTE: this function returns SgBasicBlock */
  SgNode* visitSgFunctionDefinition(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgInitializedName(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgAssignInitializer(xercesc::DOMNode* node, SgNode* astParent=0);

  /* --- values --- */
  SgNode* visitSgBoolValExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgCharVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgWcharVal(xercesc::DOMNode* node, SgNode* astParent=0);
  //SgNode* visitSgComplexVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDoubleVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFloatVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgIntVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLongIntVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLongLongIntVal(xercesc::DOMNode* node, SgNode* astParent=0);
  //SgNode* visitSgEnumVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLongDoubleVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgShortVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgStringVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgUnsignedCharVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgUnsignedShortVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgUnsignedIntVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgUnsignedLongVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgUnsignedLongLongIntVal(xercesc::DOMNode* node, SgNode* astParent=0);


  SgNode* visitSgTypedefType(xercesc::DOMNode* node, SgNode* astParent=0);

  SgType* buildModifierType(xercesc::DOMNode* node, SgType* itype, SgNode* astParent=0);

  SgType* buildType(xercesc::DOMNode* node, SgExpression* ex=0, SgNode* astParent=0);
  
  SgNode* visitSgTypeInt(xercesc::DOMNode* node, SgNode* astParent=0)
  {
    return buildType(node,0,astParent);
  }

  SgNode* visitSgTypeFloat(xercesc::DOMNode* node, SgNode* astParent=0)
  {
    return buildType(node,0,astParent);
  }

  SgNode* visitSgTypeDouble(xercesc::DOMNode* node, SgNode* astParent=0)
  {
    return buildType(node,0,astParent);
  }

  SgNode* visitSgModifierType(xercesc::DOMNode* node, SgNode* astParent=0)
  {
    return buildType(node,0,astParent);
  }

  SgNode* visitSgPointerType(xercesc::DOMNode* node, SgNode* astParent=0);

/*
  SgNode* visitSgPointerType(xercesc::DOMNode* node, SgNode* astParent=0)
  {
    return buildType(node,0,astParent);
  }
*/

  SgNode* visitSgTypeChar(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeUnsignedChar(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeSignedChar(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }

  SgNode* visitSgTypeShort(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeUnsignedShort(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeSignedShort(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }

  SgNode* visitSgTypeUnsignedInt(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeSignedInt(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }

  SgNode* visitSgTypeLong(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeUnsignedLong(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeSignedLong(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  } 

  SgNode* visitSgTypeLongLong(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeUnsignedLongLong(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeSignedLongLong(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeVoid(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeBool(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }
  SgNode* visitSgTypeLongDouble(xercesc::DOMNode* node, SgNode* astParent=0)  {  return buildType(node,0,astParent);  }


  SgNode* visitSgLabelStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgGotoStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgExprListExp(xercesc::DOMNode* node, SgNode* astParent=0);



  SgNode* visitPreprocessingInfo(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgUseStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFortranIncludeLine(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgAttributeSpecificationStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgImplicitStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFortranDo(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgFunctionCallExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFunctionRefExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgTypeString(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgTypeComplex(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgComplexVal(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgClassDeclaration(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgClassDefinition(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgClassType(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgEnumDeclaration(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgEnumType(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgEnumVal(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgTypedefDeclaration(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDotExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgArrowExp(xercesc::DOMNode* node, SgNode* astParent=0);

  SgNode* visitSgProgramHeaderStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgPrintStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgAsteriskShapeExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLabelRefExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFormatStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFormatItem(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgWriteStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgOpenStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgCloseStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgContainsStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgModuleStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgExponentiationOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgCommonBlock(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgCommonBlockObject(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgForAllStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgSubscriptExpression(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgInterfaceStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgInterfaceBody(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgReadStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgEntryStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgAllocateStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDeallocateStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgArithmeticIfStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgStopOrPauseStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgWhereStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgElseWhereStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgNullifyStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgBackspaceStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgEndfileStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgRewindStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgInquireStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFlushStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgNamelistStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDerivedTypeStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgComputedGotoStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgTypeEllipse(xercesc::DOMNode* node, SgNode* astParent=0) {  return buildType(node,0,astParent);  } ;
  SgNode* visitSgTypeDefault(xercesc::DOMNode* node, SgNode* astParent=0) {  return buildType(node,0,astParent);  } ;
  SgNode* visitSgPointerDerefExp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgVarArgStartOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgVarArgOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgVarArgEndOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgEquivalenceStatement(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgAsmStmt(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgAggregateInitializer(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFunctionType(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgFunctionParameterTypeList(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgPointerAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgTypeImaginary(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgCompoundInitializer(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgLshiftAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgRshiftAssignOp(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgImpliedDo(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDataStatementGroup(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDataStatementObject(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgDataStatementValue (xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgRenamePair(xercesc::DOMNode* node, SgNode* astParent=0);
  SgNode* visitSgConstructorInitializer(xercesc::DOMNode* node, SgNode* astParent=0);

  void checkPreprocInfo(xercesc::DOMNode* node, SgNode* astNode);
  void checkExpression (xercesc::DOMNode* node, SgNode* astNode);
  void checkStatement  (xercesc::DOMNode* node, SgNode* astNode);
  void checkFunctDecl  (xercesc::DOMNode* node, SgNode* astNode);

  //SgFile* getSgFile() {return _file;}
  SgProject* getSgProject() {return _prj;}
};


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
	std::cerr << n->unparseToString()	<< " \n";
	ABORT();
      }
      //cerr << n->unparseToString();
    }
  }
};

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
      if(tbl==0) ABORT();
      printf("\n Symbol table of %s (parent=%s)\n", 
	     n->class_name().c_str(),n->get_parent()->class_name().c_str());
      printf("=== BEGIN ====\n");
      tbl->print();
      printf("===  END  ====\n");
    }
    for(size_t i(0);i<n->get_numberOfTraversalSuccessors();++i){
      if(n->get_traversalSuccessorByIndex(i))
	this->visit(n->get_traversalSuccessorByIndex(i));
    }
    return NULL;
  }
};

}

#endif