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
#include "common.hpp"
#include "xml2ast.hpp"

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;
using namespace XevXML;

SgType* 
XevXmlVisitor::buildModifierType(xe::DOMNode* node, SgType* itype, SgNode* astParent)
{
  SgType* ret;
  string modtype;
  
  if(XmlGetAttributeValue(node,"modifier",&modtype)==false)
      ABORT();

  if( modtype == "const" ) {
    //return sb::buildConstType(itype);
    SgModifierType *result = new SgModifierType(itype);
    result->get_typeModifier().get_constVolatileModifier().setConst();
    ret=result;
  }
  else if ( modtype == "volatile" )
    ret = sb::buildVolatileType(itype);
  else if ( modtype == "restrict" )
    ret = sb::buildRestrictType(itype);
  else return 0;
  ret->set_parent(astParent);
  return ret;
}

SgType* 
XevXmlVisitor::buildType(xe::DOMNode* node, SgExpression* ex, SgNode* astParent)
{
  SgType* itype = 0;
  SgType* ret = 0;
  SgExpression* kexp = 0;
  //SgExpression* lexp = 0;
  if(node==0) return ret;
  int kind=0, len=0;

  XmlGetAttributeValue(node,"type_kind",&kind);
  //XmlGetAttributeValue(node,"lengthExpression",&len);

  // reverse order
  xe::DOMNode* child=node->getLastChild();
  //xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      if(itype==0)
        itype = buildType(child,ex);

      if(kind && kexp==0){
	SgNode* astchild = this->visit(child); 
	kexp = isSgExpression(astchild);
      }
    }
    child=child->getPreviousSibling();
    //child=child->getNextSibling();
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
      ret = buildModifierType(node,itype,astParent);
    }
    else {
      itype = sb::buildIntType();
      ret = buildModifierType( node,itype, astParent );
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
  else {
    return NULL;
  }
  if(itype) itype->set_parent(ret);
  if(kind && kexp) {
    ret->set_type_kind(kexp);
    kexp->set_parent(ret);
  }
  ret->set_parent(astParent);
  return ret;
}


SgNode* 
XevXmlVisitor::visitSgTypeString(xe::DOMNode* node, SgNode* astParent)
{
  SgTypeString* ret = 0;
  int len=0,kind=0;
  SgExpression *kexp=0,*lexp=0;
  XmlGetAttributeValue(node,"type_kind",&kind);
  XmlGetAttributeValue(node,"lengthExpression",&len);

  xercesc::DOMNode* cld_ = (node)->getFirstChild();
  while(cld_) {								
    if(cld_->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){		
      SgNode* astchild = this->visit(cld_);
      if(kind && kexp==0){
	kexp = isSgExpression(astchild);
      }
      else if(len && lexp==0){
	lexp = isSgExpression(astchild);
      }
      
    }
    cld_=cld_->getNextSibling();
  }

  if(lexp){
    ret = sb::buildStringType( lexp );
  }
  else{
    lexp
      = new SgAsteriskShapeExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
    ret = sb::buildStringType( lexp );
  }
  lexp->set_parent(ret);
  /*
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("len"));
    if(nameatt) {
      length = xe::XMLString::transcode(nameatt->getNodeValue());
    }
  }
  if( length.size() ){
    int ival = atoi(length.c_str());
    Sg_File_Info* info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
    //v->set_file_info(info);
    SgIntVal* v = new SgIntVal( info,ival,length );
    ret = sb::buildStringType( (SgExpression*)v );
  }
  else {
    SgAsteriskShapeExp* ast 
      = new SgAsteriskShapeExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
    ret = sb::buildStringType( (SgExpression*)ast );
  }
  */
  if(ret==0) ABORT();
  if(kind&&kexp){
    ret->set_type_kind(kexp);
    kexp->set_parent(ret);
  }
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgArrayType(xe::DOMNode* node, SgNode* astParent)
{
  SgArrayType*            ret = new SgArrayType();
  SgArrayType*            ary = 0; 
  SgArrayType*            sav = 0; 
  SgType*                 typ = 0;
  SgExpression*           exp = 0;
  SgExpression*           v;
  SgPointerType*          ptr = 0;
  SgPointerType*          ptrtyp = 0;
 
  //xe::DOMNamedNodeMap*    amap = node->getAttributes();
  //xe::DOMNode*            nameatt = 0;
  string                  str1,str2;
  int                     rnk=0;
  unsigned long           idx=0;

  XmlGetAttributeValue(node,"rank",&rnk);
  if(XmlGetAttributeValue(node,"index",&str2))
    idx = strtoul( str2.c_str(),0,0 );

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
	if(typ==0) 
	  typ = isSgType(astchild);
	
	if((exp = isSgExpression(astchild))!=0){
	  exp->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
	  exprs.push_back(exp);
	}

      }
      child=child->getNextSibling();
    }
    if(ret==0) ABORT();
    if(typ){
      ret->set_base_type( typ );
      typ->set_parent(ret);
    }
    else ABORT();
    if(rnk){
      if(exprs.size() != (size_t)rnk) ABORT();
      ret->set_rank( rnk );
      lst=sb::buildExprListExp( exprs );
      lst->set_parent(ret);
      //std::cerr << lst->unparseToCompleteString() << std::endl;
      for(size_t i=0;i<exprs.size();i++)
	exprs[i]->set_parent(lst);
      ret->set_dim_info( lst );
    }
  }
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgPointerType(xe::DOMNode* node, SgNode* astParent)
{
  SgPointerType*      ret = new SgPointerType();
  SgType*             typ = 0;
  SgModifierType*     mdf = 0;
  SgTypedefType*      tdf = 0;
  SgPointerType*      ptr = 0;
  SgArrayType*        ary = 0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      typ = isSgType(astchild);
      
      if( (mdf==0) && (tdf==0) && (ptr==0) && (ary==0) ){
	ret->set_base_type( typ );
	mdf = isSgModifierType(astchild);
	tdf = isSgTypedefType(astchild);
	ary = isSgArrayType(astchild);
	ptr = isSgPointerType(astchild);
	if( tdf )
	  break;
      }
      else {
	if( mdf )
	  mdf->set_base_type( typ );
	if( ptr )
	  ptr->set_base_type( typ );
	if( ary )
	  ary->set_base_type( typ );
      }
    }
    child=child->getNextSibling();
  }

  if(ret==0) ABORT();
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgTypeComplex(xe::DOMNode* node, SgNode* astParent)
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

  if(ret==0) ABORT();
  ret->set_parent(astParent);
  return ret;
}

SgNode* 
XevXmlVisitor::visitSgTypeImaginary(xe::DOMNode* node, SgNode* astParent)
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

  if(ret==0) ABORT();
  ret->set_parent(astParent);
  return ret;
}

