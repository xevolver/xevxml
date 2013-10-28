/**
 * @file     types.cpp
 * @brief    Functions to build SgType classes.
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

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;
using namespace xevxml;

SgType* 
Xml2AstVisitor::buildModifierType(xe::DOMNode* node, SgType* itype)
{
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* typatt = 0;
  string modtype;
  
  if(amap) {
    typatt=amap->getNamedItem(xe::XMLString::transcode("modifier"));
    if(typatt)
      modtype = xe::XMLString::transcode(typatt->getNodeValue());
    else 
      ABORT();
  }
  if( modtype == "const" ) {
    //return sb::buildConstType(itype);
    SgModifierType *result = new SgModifierType(itype);
    result->get_typeModifier().get_constVolatileModifier().setConst();
    return result;
  }
  else if ( modtype == "volatile" )
    return sb::buildVolatileType(itype);
  else if ( modtype == "restrict" )
    return sb::buildRestrictType(itype);
  else return 0;
}

SgType* 
Xml2AstVisitor::buildType(xe::DOMNode* node, SgExpression* ex)
{
  SgType* itype = 0;
  SgType* ret = 0;

  if(node==0) return ret;
  
  // reverse order
  xe::DOMNode* child=node->getLastChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      if(itype==0)
        itype = buildType(child,ex);
    }
    child=child->getPreviousSibling();
  } 
  
  string name = xe::XMLString::transcode(node->getNodeName());
  
  
  if(name=="SgTypeBool")
    ret = sb::buildBoolType();
  else if(name=="SgTypeChar")
    ret = sb::buildCharType();
  else if(name=="SgTypeDouble")
    ret = sb::buildDoubleType();
  else if(name=="SgTypeFloat")
    ret = sb::buildFloatType();
  else if(name=="SgTypeInt")
    ret = sb::buildIntType();
  else if(name=="SgTypeLong")
    ret = sb::buildLongType();
  else if(name=="SgTypeLongLong")
    ret = sb::buildLongLongType();
  else if(name=="SgTypeShort")
    ret = sb::buildShortType();
//  else if(name=="SgTypeString"){
//    if(ex) 
//      ret = sb::buildStringType(ex);
//    else
//      ret = sb::buildStringType();
//  }
  else if(name=="SgTypeVoid")
    ret = sb::buildVoidType();
  else if(name=="SgTypeWchar")
    ret = sb::buildWcharType();
  else if(name=="SgTypeSignedChar")
    ret = sb::buildSignedCharType();
  else if(name=="SgTypeSignedInt")
    ret = sb::buildSignedIntType();
  else if(name=="SgTypeSignedLong")
    ret = sb::buildSignedLongType();
  else if(name=="SgTypeSignedLongLong")
    ret = sb::buildSignedLongLongType();
  else if(name=="SgTypeSignedShort")
    ret = sb::buildSignedShortType();
  else if(name=="SgTypeUnsignedChar")
    ret = sb::buildUnsignedCharType();
  else if(name=="SgTypeUnsignedInt")
    ret = sb::buildUnsignedIntType();
  else if(name=="SgTypeUnsignedLong")
    ret = sb::buildUnsignedLongType();
  else if(name=="SgTypeUnsignedLongLong")
    ret = sb::buildUnsignedLongLongType();
  else if(name=="SgTypeUnsignedShort")
    ret = sb::buildUnsignedShortType();
  else if(name=="SgTypeLongDouble")
    ret = sb::buildLongDoubleType();
  else if(name=="SgTypeUnknown")
    ret = sb::buildUnknownType();
//   else if(name=="SgPointerType") {
//    if(itype)
//      ret = sb::buildPointerType(itype);
//    else ABORT();
// }
  else if(name=="SgReferenceType") {
    if(itype)
      ret = sb::buildReferenceType(itype);
    else ABORT();
  }
  else if (name=="SgModifierType") {
    if(itype) {
      ret = buildModifierType(node,itype);
    }
    else {
      itype = sb::buildIntType();
      ret = buildModifierType( node,itype );
    }
    //else ABORT();
  }

  else if (name=="SgClassType") {
    ret = isSgType( this->visit(node,ret) );
  }
  else if (name=="SgTypeEllipse") {
    ret = new SgTypeEllipse();
  }
  else if (name=="SgTypeDefault") {
    ret = new SgTypeDefault();
  }

  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgTypeString(xe::DOMNode* node, SgNode* astParent)
{
  SgTypeString* ret = 0;
  
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* nameatt = 0;
  string length;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("len"));
    if(nameatt) {
      length = xe::XMLString::transcode(nameatt->getNodeValue());
    }
  }

  if( length.size() ){
    int ival = atoi(length.c_str());
    SgIntVal* v = new SgIntVal( ival,length );
    ret = sb::buildStringType( (SgExpression*)v );
    
  }
  else {
    SgAsteriskShapeExp* ast 
      = new SgAsteriskShapeExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
    ret = sb::buildStringType( (SgExpression*)ast );
  }
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgArrayType(xe::DOMNode* node, SgNode* astParent)
{
  SgArrayType*            ret = new SgArrayType();
  SgArrayType*            ary = 0; 
  SgArrayType*            sav = 0; 
  SgType*                 typ = 0;
  SgExpression*           exp = 0;
  SgExpression*           v;
  SgPointerType*          ptr = 0;
  SgPointerType*          ptrtyp = 0;
 
  xe::DOMNamedNodeMap*    amap = node->getAttributes();
  xe::DOMNode*            nameatt = 0;
  string                  str1,str2;
  int                     rnk=0;
  unsigned long           idx=0;

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("rank"));
    if(nameatt) {
      str1 = xe::XMLString::transcode(nameatt->getNodeValue());
      rnk = atoi( str1.c_str() );
    }
    nameatt=amap->getNamedItem(xe::XMLString::transcode("index"));
    if(nameatt) {
      str2 = xe::XMLString::transcode(nameatt->getNodeValue());
      if( str2.size() )
	idx = strtoul( str2.c_str(),0,0 );
    }
  }
  else ABORT();


  if( rnk == 0 ) {
    xe::DOMNode* child=node->getFirstChild();
    
    sav = ret;
    while(child){
      if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE) {
	SgNode* astchild = this->visit(child,ret);
	ary = isSgArrayType(astchild);
	typ = isSgType(astchild);
	ptr = isSgPointerType(astchild);
	
	if( ptrtyp==0 )
	  sav->set_base_type( typ );
	else
	  ptrtyp->set_base_type( typ );
	
	if( (ptrtyp==0) && ptr )
	  ptrtyp = ptr;
	
	if( ary )
	  sav = ary;
      }
      child=child->getNextSibling();
    }
    
    if( idx ){
      v= new SgUnsignedLongVal( idx,str2 );
      v->set_startOfConstruct( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
      ret->set_index(v);
    }
  }
  else{
    xe::DOMNode*                    child=node->getFirstChild();
    SgExprListExp*                  lst;
    std::vector< SgExpression * >   exprs;
    
    while(child){
      if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
	SgNode* astchild = this->visit(child,ret);
	
	if( typ==0 )
	  typ = isSgType(astchild);

	if((exp = isSgExpression(astchild))!=0){
	  exp->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
	  exprs.push_back(exp);
	}

      }
      child=child->getNextSibling();
    }
    ret->set_base_type( typ );
    typ->set_parent(ret);
    ret->set_rank( rnk );
    lst=sb::buildExprListExp( exprs );
    lst->set_parent(ret);
    ret->set_dim_info( lst );
  }

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgPointerType(xe::DOMNode* node, SgNode* astParent)
{
  SgPointerType*      ret = new SgPointerType();
  SgType*             typ = 0;
  SgModifierType*     mdf = 0;
  SgTypedefType*      tdf = 0;
  SgPointerType*      ptr = 0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      typ = isSgType(astchild);
      
      if( (mdf==0) && (tdf==0) && (ptr==0) ){
	ret->set_base_type( typ );
	mdf = isSgModifierType(astchild);
	tdf = isSgTypedefType(astchild);
	ptr = isSgPointerType(astchild);
	if( tdf )
	  break;
      }
      else {
	if( mdf )
	  mdf->set_base_type( typ );
	if( ptr )
	  ptr->set_base_type( typ );
      }
    }
    child=child->getNextSibling();
  }
    
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgTypeComplex(xe::DOMNode* node, SgNode* astParent)
{
  SgTypeComplex*    ret = 0;
  SgType*           typ = 0;

  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* nameatt = 0;
  string              type;
  string              base;

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("type_kind"));
    if(nameatt)
      type = xe::XMLString::transcode(nameatt->getNodeValue());

    nameatt=amap->getNamedItem(xe::XMLString::transcode("base_type"));
    if(nameatt)
      base = xe::XMLString::transcode(nameatt->getNodeValue());

    if( base == "SgTypeFloat" )
        typ = new SgTypeFloat();
    else if( base == "SgTypeDouble" )
        typ = new SgTypeDouble();
    else
        typ = new SgTypeLongDouble();

    ret = sb::buildComplexType( typ );

    if( type.size() ) {
      int ival = atoi(type.c_str());
      SgExpression* v = new SgIntVal(ival,type );
      v->set_startOfConstruct( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
      ret->set_type_kind( v );
    }
  }
  else ABORT();

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgTypeImaginary(xe::DOMNode* node, SgNode* astParent)
{
  SgTypeImaginary*  ret = 0;
  SgType*           typ = 0;
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* nameatt = 0;
  string              type;
  string              base;

  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("type_kind"));
    if(nameatt)
      type = xe::XMLString::transcode(nameatt->getNodeValue());
    
    nameatt=amap->getNamedItem(xe::XMLString::transcode("base_type"));
    if(nameatt)
      base = xe::XMLString::transcode(nameatt->getNodeValue());

    if( base == "SgTypeFloat" )
        typ = new SgTypeFloat();
    else if( base == "SgTypeDouble" )
        typ = new SgTypeDouble();
    else
        typ = new SgTypeLongDouble();

    ret = sb::buildImaginaryType( typ );

    if( type.size() ) {
      int ival = atoi(type.c_str());
      SgExpression* v = new SgIntVal(ival,type );
      v->set_startOfConstruct( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
      ret->set_type_kind( v );
    }

  }
  else ABORT();

  return ret;
}

