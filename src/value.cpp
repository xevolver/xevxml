/**
 * @file     value.cpp
 * @brief    interconversion of values between Rose and XML
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
#include "common.hpp"
#include "xml2rose.hpp"
#include "rose2xml.hpp"

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;
using namespace XevXml;

/* unparseLanguageIndependentConstructs.h */
/* this is what unparser does for values */
template <typename T>
std::string tostring(T t) {
  std::ostringstream oss;
  oss << std::showpoint << t << std::flush;
  return oss.str();
}

#define VISIT_VAL(valType,baseType)                                     \
  /** Visitor of a Sg##valType element in an XML document */            \
  SgNode* XevXmlVisitor::                                               \
  visitSg##valType(xercesc::DOMNode* node, SgNode* astParent)           \
  {                                                                     \
    Sg##valType* ret = 0;                                               \
    SgExpression* oexp=0;                                               \
    baseType ival=0;                                                    \
    std::string vstr;                                                   \
    if(XmlGetAttributeValue(node,"value",&ival)==false) {               \
      XEV_MISSING_ATTR(Sg##valType,value,true);                         \
    }                                                                   \
    ret = sb::build##valType(ival);                                     \
    XEV_ASSERT(ret!=NULL);                                              \
    ret->set_parent(astParent);                                         \
    if(XmlGetAttributeValue(node,"value",&vstr)) {                      \
      vstr = XmlEntity2Str(vstr);                                       \
      if(vstr != tostring(ival))                                        \
        ret->set_valueString(vstr);                                     \
    }                                                                   \
    SUBTREE_VISIT_BEGIN(node,astchild,ret)                              \
      {                                                                 \
        if(oexp==0) oexp=isSgExpression(astchild);                      \
      }                                                                 \
    SUBTREE_VISIT_END();                                                \
    if(oexp) ret->set_originalExpressionTree(oexp);                     \
    return ret;                                                         \
  }                                                                     \
  /** XML attribute writer of Sg##vatType */                            \
  void XevSageVisitor::attribSg##valType(SgNode* node)                  \
  {                                                                     \
    Sg##valType* n = isSg##valType(node);                               \
      if(n) {                                                           \
        std::string unparsedString = tostring(n->get_value());          \
        if(n->get_valueString().size() > 0                              \
           && n->get_valueString() != unparsedString ){                 \
          sstr() << " value=\""                                         \
                 << XmlStr2Entity(n->get_valueString()) << "\" ";       \
        }                                                               \
        else                                                            \
          sstr() << " value=\"" << unparsedString << "\" ";             \
      }                                                                 \
  }                                                                     \
  /** XML internal node writer of Sg##vatType */                        \
  void XevSageVisitor::inodeSg##valType(SgNode* node) {                 \
    SgValueExp* n = isSgValueExp(node);                                 \
    if(n && n->get_originalExpressionTree())                            \
      this->visit(n->get_originalExpressionTree());                     \
    return;                                                             \
  }

#define VISIT_VAL_NO_STRING(valType,baseType)                           \
  /** Visitor of a Sg##valType element in an XML document */            \
  SgNode* XevXmlVisitor::                                               \
  visitSg##valType(xercesc::DOMNode* node, SgNode* astParent)           \
  {                                                                     \
    Sg##valType* ret = 0;                                               \
    SgExpression* oexp=0;                                               \
    baseType ival=0;                                                    \
    std::string vstr;                                                   \
    if(XmlGetAttributeValue(node,"value",&ival)==false) {               \
      XEV_MISSING_ATTR(Sg##valType,value,true);                         \
    }                                                                   \
    ret = sb::build##valType(ival);                                     \
      XEV_ASSERT(ret!=NULL);                                            \
    ret->set_parent(astParent);                                         \
    SUBTREE_VISIT_BEGIN(node,astchild,ret)                              \
      {                                                                 \
        if(oexp==0) oexp=isSgExpression(astchild);                      \
      }                                                                 \
    SUBTREE_VISIT_END();                                                \
    if(oexp) ret->set_originalExpressionTree(oexp);                     \
    return ret;                                                         \
  }                                                                     \
  /** XML attribute writer of Sg##valType */                            \
  void XevSageVisitor::attribSg##valType(SgNode* node)                  \
  {                                                                     \
    Sg##valType* n = isSg##valType(node);                               \
      if(n) {                                                           \
        sstr()<< " value=\"" << n->get_value() << "\" ";                \
      }                                                                 \
  }                                                                     \
  /** XML internal node writer of Sg##vatType */                        \
  void XevSageVisitor::inodeSg##valType(SgNode* node) {                 \
    SgValueExp* n = isSgValueExp(node);                                 \
    if(n && n->get_originalExpressionTree())                            \
      this->visit(n->get_originalExpressionTree());                     \
    return;                                                             \
  }

#define VISIT_VAL_CHAR(valType,baseType)                                \
  /** Visitor of a Sg##valType element in an XML document */            \
  SgNode* XevXmlVisitor::                                               \
  visitSg##valType(xercesc::DOMNode* node, SgNode* astParent)           \
  {                                                                     \
    Sg##valType* ret = 0;                                               \
    baseType ival=0;                                                    \
    unsigned long temp=0;                                               \
    SgExpression* oexp=0;                                               \
    std::string vstr;                                                   \
    if(XmlGetAttributeValue(node,"value",&temp)==false) {               \
      XEV_MISSING_ATTR(Sg##valType,value,true);                         \
    }                                                                   \
    ival = (baseType)temp;                                              \
    ret = sb::build##valType(ival);                                     \
      XEV_ASSERT(ret!=NULL);                                            \
    ret->set_parent(astParent);                                         \
    SUBTREE_VISIT_BEGIN(node,astchild,ret)                              \
      {                                                                 \
        if(oexp==0) oexp=isSgExpression(astchild);                      \
      }                                                                 \
    SUBTREE_VISIT_END();                                                \
    if(oexp) ret->set_originalExpressionTree(oexp);                     \
    return ret;                                                         \
  }                                                                     \
  /** XML attribute writer of Sg##valType */                            \
  void XevSageVisitor::attribSg##valType(SgNode* node)                  \
  {                                                                     \
    Sg##valType* n = isSg##valType(node);                               \
      if(n) {                                                           \
        unsigned long temp = (unsigned long)n->get_value();             \
        sstr() << " value=\"" << temp << "\" ";                         \
      }                                                                 \
  }                                                                     \
  /** XML internal node writer of Sg##vatType */                        \
  void XevSageVisitor::inodeSg##valType(SgNode* node) {                 \
    SgValueExp* n = isSgValueExp(node);                                 \
    if(n && n->get_originalExpressionTree())                            \
      this->visit(n->get_originalExpressionTree());                     \
    return;                                                             \
  }

VISIT_VAL_NO_STRING(BoolValExp,bool);
VISIT_VAL_CHAR(CharVal,char);
VISIT_VAL(DoubleVal,double);
VISIT_VAL(FloatVal,float);
VISIT_VAL(IntVal,int);
VISIT_VAL(LongDoubleVal,long double);
VISIT_VAL(LongIntVal,long int);
VISIT_VAL(LongLongIntVal,long long int);
//VISIT_VAL(SgComplexVal);
//VISIT_VAL(SgEnumVal);
VISIT_VAL(ShortVal,short);
//VISIT_VAL(StringVal,string);
VISIT_VAL_CHAR(UnsignedCharVal,unsigned char);
VISIT_VAL(UnsignedIntVal,unsigned int);
VISIT_VAL(UnsignedLongLongIntVal,unsigned long long);
VISIT_VAL(UnsignedLongVal,unsigned long);
VISIT_VAL(UnsignedShortVal,unsigned short);
VISIT_VAL(WcharVal,unsigned short);

// =====================================================================
/// Visitor of a SgEnumVal element in an XML document
SgNode*
XevXmlVisitor::visitSgEnumVal(xe::DOMNode* node, SgNode* astParent)
{
  SgEnumSymbol* esym = 0;
  SgEnumVal*    ret  = 0;
  SgExpression* oexp = 0;
  int           ival = 0;
  string        name,ename;

  if(XmlGetAttributeValue(node,"name",&name)==false)
    XEV_MISSING_ATTR(SgEnumVal,name,true);
  if(XmlGetAttributeValue(node,"value",&ival)==false)
    XEV_MISSING_ATTR(SgEnumVal,value,true);

  if(XmlGetAttributeValue(node,"enum",&ename)){
    esym = si::lookupEnumSymbolInParentScopes(ename);
    if(esym==0){
      XEV_INFO("enum symbol \"" << name << "\" not found");
    }
  }
  if(esym)
    ret = new SgEnumVal(ival,esym->get_declaration(),name);
  else
    ret = new SgEnumVal(ival,NULL,name);
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  ret->set_startOfConstruct(DEFAULT_FILE_INFO);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(oexp==0) oexp=isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();
  if(oexp) ret->set_originalExpressionTree(oexp);
  return ret;
}
/** XML attribute writer of SgEnumVal */
void XevSageVisitor::attribSgEnumVal(SgNode* node) {
  SgEnumVal* n = isSgEnumVal(node);
    if(n) {
      sstr() << " value=\"" << n->get_value() << "\" ";
      sstr() << " name=" << n->get_name() << " ";
      sstr() << " enum=" << n->get_declaration()->get_name() << " ";
    }
}
/** XML internal node writer of SgEnumVal */
void XevSageVisitor::inodeSgEnumVal(SgNode* node) {
  SgValueExp* n = isSgValueExp(node);
  if(n && n->get_originalExpressionTree())
    this->visit(n->get_originalExpressionTree());
  return;
}

// =====================================================================
/// Visitor of a SgComplexVal element in an XML document
SgNode*
XevXmlVisitor::visitSgComplexVal(xercesc::DOMNode* node, SgNode* astParent)
{
  SgComplexVal* ret = 0;
  SgExpression* oexp= 0;
  SgValueExp*   real  = 0;
  SgValueExp*   imag  = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,astParent)
    {
      /* assuming these exprs appear in this order */
      if(real==0)
        real = isSgValueExp(astchild);
      else if (imag==0)
        imag = isSgValueExp(astchild);
      else if (oexp==0)
        oexp = isSgExpression(oexp);
    }
  SUBTREE_VISIT_END();
  if( real==0 && imag ==0 ){
    real = sb::buildLongDoubleVal(0);
    imag = sb::buildLongDoubleVal(0);
  }
  else if (imag == 0){
    // this works for complex_01.c. but is this correct?
    imag = real;
    real = 0;
  }

  ret = sb::buildComplexVal( real, imag );
  XEV_ASSERT(ret!=NULL);
  return ret;
}
/** XML attribute writer of SgComplexVal */
void
XevSageVisitor::attribSgComplexVal(SgNode* node) {return;}
/** XML internal node writer of SgComplexVal */
void XevSageVisitor::inodeSgComplexVal(SgNode* node){
  SgValueExp* n = isSgValueExp(node);
  if(n && n->get_originalExpressionTree())
    this->visit(n->get_originalExpressionTree());
  return;
}

// =====================================================================
/// Visitor of a SgStringVal element in an XML document
SgNode*
XevXmlVisitor::visitSgStringVal(xercesc::DOMNode* node, SgNode* astParent)
{
  SgStringVal*            ret = 0;
  string                  str;
  int                     flag=0;
  SgExpression*           oexp=0;

  if(XmlGetAttributeValue(node,"value",&str) == false){
    XEV_MISSING_ATTR(SgStringVal,value,true);
  }
  //if(str.size())                                // del (0821)
  ret = sb::buildStringVal(XmlEntity2Str(str));
  XEV_ASSERT(ret!=NULL);
  //else XEV_ABORT();                                 // del (0821)
  ret->set_parent(astParent);

  if(XmlGetAttributeValue(node,"single",&flag))
    ret->set_usesSingleQuotes(flag);
  if(XmlGetAttributeValue(node,"wchar",&flag))
    ret->set_wcharString(flag);
  if(XmlGetAttributeValue(node,"string16",&flag))
    ret->set_is16bitString(flag);
  if(XmlGetAttributeValue(node,"string32",&flag))
    ret->set_is32bitString(flag);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(oexp==0) oexp=isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();
  if(oexp) ret->set_originalExpressionTree(oexp);
  return ret;
}
/** XML attribute writer of SgStringVal */
void XevSageVisitor::attribSgStringVal(SgNode* node)
{
  SgStringVal* n = isSgStringVal(node);
  if(n) {
    sstr() << " value=\"" << XmlStr2Entity(n->get_value()) << "\" ";
    if(n->get_usesSingleQuotes()==true)
      sstr() << " single=\"1\" ";
    if(n->get_wcharString()==true)
      sstr() << " wchar=\"1\" ";
    if(n->get_is16bitString()==true)
      sstr() << " string16=\"1\" ";
    if(n->get_is32bitString()==true)
      sstr() << " string32=\"1\" ";
  }
}
/** XML internal node writer of SgStringVal */
void XevSageVisitor::inodeSgStringVal(SgNode* node) {
  SgValueExp* n = isSgValueExp(node);
  if(n && n->get_originalExpressionTree())
    this->visit(n->get_originalExpressionTree());
  return;
}
