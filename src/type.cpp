/**
 * @file     type.cpp
 * @brief    Visitor Functions of SgType classes.
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

#if 0
SgType*
XevXmlVisitor::buildType(xe::DOMNode* node, SgExpression* ex, SgNode* astParent)
{
  SgType* itype = 0;
  SgType* ret = 0;
  string  kind;
  string  tagname;
  SgExpression* kexp = 0;

  if(node==0) return ret;
  XmlGetAttributeValue(node,"type_kind",&kind);
  XmlGetAttributeValue(node,"tag_name",&tagname);

  // reverse order
  xe::DOMNode* child=node->getLastChild();
  //xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if(itype==0)
        //itype = buildType(child,ex);
        itype = isSgType(astchild);
    }
    child=child->getPreviousSibling();
    //child=child->getNextSibling();
  }
  char* buf = xe::XMLString::transcode(node->getNodeName());
  std::string name = std::string(buf);
  xercesc::XMLString::release(&buf);

  kexp = 0;
  if(itype) itype->set_parent(ret);
  if( kind.size()>0 ) {
    int val = atoi(kind.c_str());
    kexp = new SgIntVal(val,kind);
    kexp->set_startOfConstruct(DEFAULT_FILE_INFO );
    //ret->set_type_kind( kexp );
    kexp->set_parent(ret);
  }

  if(name=="SgTypeBool")
    ret = SgTypeBool::createType(kexp);
  //ret = sb::buildBoolType();
  else if(name=="SgTypeChar")
    ret = SgTypeChar::createType(kexp);
  //ret = sb::buildCharType();
  else if(name=="SgTypeDouble")
    ret = SgTypeDouble::createType(kexp);
  //ret = sb::buildDoubleType();
  else if(name=="SgTypeFloat")
    ret = SgTypeFloat::createType(kexp);
  //ret = sb::buildFloatType();
  else if(name=="SgTypeInt")
    ret = SgTypeInt::createType(0,kexp);
  //ret = sb::buildIntType();
  else if(name=="SgTypeLong")
    ret = SgTypeLong::createType(kexp);
  //ret = sb::buildLongType();
  else if(name=="SgTypeLongLong")
    ret = SgTypeLongLong::createType(kexp);
  //ret = sb::buildLongLongType();
  else if(name=="SgTypeShort")
    ret = SgTypeShort::createType(kexp);
  //ret = sb::buildShortType();
//  else if(name=="SgTypeString"){
//    if(ex)
//      ret = sb::buildStringType(ex);
//    else
//      ret = sb::buildStringType();
//  }
  else if(name=="SgTypeVoid")
    ret = SgTypeVoid::createType(kexp);
  //ret = sb::buildVoidType();
  else if(name=="SgTypeWchar")
    ret = SgTypeWchar::createType(kexp);
  //ret = sb::buildWcharType();
  else if(name=="SgTypeSignedChar")
    ret = SgTypeSignedChar::createType(kexp);
  //ret = sb::buildSignedCharType();
  else if(name=="SgTypeSignedInt")
    ret = SgTypeSignedInt::createType(kexp);
  //ret = sb::buildSignedIntType();
  else if(name=="SgTypeSignedLong")
    ret = SgTypeSignedLong::createType(kexp);
  //ret = sb::buildSignedLongType();
  else if(name=="SgTypeSignedLongLong")
    ret = SgTypeSignedLongLong::createType(kexp);
  //ret = sb::buildSignedLongLongType();
  else if(name=="SgTypeSignedShort")
    ret = SgTypeSignedShort::createType(kexp);
  //ret = sb::buildSignedShortType();
  else if(name=="SgTypeUnsignedChar")
    ret = SgTypeUnsignedChar::createType(kexp);
  //ret = sb::buildUnsignedCharType();
  else if(name=="SgTypeUnsignedInt")
    ret = SgTypeUnsignedInt::createType(kexp);
  //ret = sb::buildUnsignedIntType();
  else if(name=="SgTypeUnsignedLong")
    ret = SgTypeUnsignedLong::createType(kexp);
  //ret = sb::buildUnsignedLongType();
  else if(name=="SgTypeUnsignedLongLong")
    ret = SgTypeUnsignedLongLong::createType(kexp);
  //ret = sb::buildUnsignedLongLongType();
  else if(name=="SgTypeUnsignedShort")
    ret = SgTypeUnsignedShort::createType(kexp);
  //ret = sb::buildUnsignedShortType();
  else if(name=="SgTypeLongDouble")
    ret = SgTypeLongDouble::createType(kexp);
  //ret = sb::buildLongDoubleType();
  else if(name=="SgTypeUnknown")
    ret = SgTypeUnknown::createType(kexp);
  //ret = sb::buildUnknownType();
//   else if(name=="SgPointerType") {
//    if(itype)
//      ret = sb::buildPointerType(itype);
//    else XEV_ABORT();
// }
  else if(name=="SgReferenceType") {
    if(itype)
      ret = sb::buildReferenceType(itype);
    else {
      XEV_DEBUG_INFO(node);
      XEV_ABORT();
    }
  }
  /*
  else if (name=="SgModifierType") {
    if(itype) {
      ret = buildModifierType(node,itype,astParent);
    }
    else {
      itype = sb::buildIntType();
      ret = buildModifierType( node,itype, astParent );
    }
    //else XEV_ABORT();
  }
  */
  else if (name=="SgClassType") {
    SgClassSymbol* sym = sb::topScopeStack()->lookup_class_symbol(tagname);
    if(sym==0){
      XEV_DEBUG_INFO(node);
      XEV_ABORT();
    }
    SgClassDeclaration* decl = sym->get_declaration();
    if(decl==0){
      XEV_DEBUG_INFO(node);
      XEV_ABORT();
    }
    ret = new SgClassType(decl);
    if(ret==0){
      XEV_DEBUG_INFO(node);
      XEV_ABORT();
    }
    //ret = isSgType( this->visit(node,ret) );
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
  ret->set_parent(astParent);
  return ret;
}
#endif

static void attribSgType(std::ostream& str, SgNode* node)
{
  // type kind is now written as an inode
#if 0
  SgType* t = isSgType(node);
  if( t && t->get_type_kind() ){
    SgIntVal* v = isSgIntVal( t->get_type_kind() );
    if( v )
      str << " kind=\"" << v->get_valueString() << "\" ";
  }
#endif
}

static void inodeSgType(XevXml::XevSageVisitor* v, SgNode* node)
{
  SgType* t = isSgType(node);
  if( t && t->get_type_kind() ){
    v->visit(t->get_type_kind());
  }
  if( t && t->get_modifiers() ){
    v->visit(t->get_modifiers());
  }
}

#define VISIT_TYPE(Type) \
  /** Visitor of a Sg##Type element in an XML document */               \
  SgNode* XevXmlVisitor::                                               \
  visitSg##Type(xercesc::DOMNode* node, SgNode* astParent)              \
  {                                                                     \
    SgType* ret=NULL;                                                   \
    SgExpression* kexp = NULL;                                          \
    SUBTREE_VISIT_BEGIN(node,astchild,ret)                              \
      {                                                                 \
        kexp = isSgExpression(astchild);                                \
      }                                                                 \
    SUBTREE_VISIT_END();                                                \
    ret= Sg##Type::createType(kexp);                                    \
    if( kexp )                                                          \
      kexp->set_parent(ret);                                            \
    ret->set_parent(astParent);                                         \
    return ret;                                                         \
  }                                                                     \
  /** XML attribute writer of Sg##Type */                               \
  void XevSageVisitor::attribSg##Type(SgNode* node){                    \
    attribSgType(sstr(),node);                                          \
  }                                                                     \
  /** XML interanal node writer of Sg##Type */                          \
  void XevSageVisitor::inodeSg##Type(SgNode* node){                     \
    inodeSgType(this,node);                                             \
  }

#define VISIT_TYPE2(Type)                                               \
  /** Visitor of a Sg##Type element in an XML document */               \
  SgNode* XevXmlVisitor::                                               \
  visitSg##Type(xercesc::DOMNode* node, SgNode* astParent)              \
  {                                                                     \
    SgType* ret=NULL;                                                   \
    ret= new Sg##Type();                                                \
    ret->set_parent(astParent);                                         \
    return ret;                                                         \
  }                                                                     \
  /** XML attribute writer of Sg##Type */                               \
  void XevSageVisitor::attribSg##Type(SgNode* node) {                   \
    attribSgType(sstr(),node);                                          \
  }                                                                     \
  /** XML interanal node writer of Sg##Type */                          \
  void XevSageVisitor::inodeSg##Type(SgNode* node){}

#define VISIT_TYPE3(Type) \
  /** Visitor of a Sg##Type element in an XML document */               \
  SgNode* XevXmlVisitor::                                               \
  visitSg##Type(xercesc::DOMNode* node, SgNode* astParent)              \
  {                                                                     \
    SgType* ret=NULL;                                                   \
    SgType* typ=NULL;                                                   \
    SgExpression* kexp = NULL;                                          \
    SUBTREE_VISIT_BEGIN(node,astchild,ret)                              \
      {                                                                 \
        if(typ==0) typ = isSgType(astchild);                            \
        if(kexp==0) kexp = isSgExpression(astchild);                    \
      }                                                                 \
    SUBTREE_VISIT_END();                                                \
    if(typ)                                                             \
      ret = Sg##Type::createType(typ,kexp);                             \
    ret->set_parent(astParent);                                         \
    return ret;                                                         \
  }                                                                     \
  /** XML attribute writer of Sg##Type */                               \
  void XevSageVisitor::attribSg##Type(SgNode* node)                     \
  {                                                                     \
    attribSgType(sstr(),node);                                          \
  }                                                                     \
  /** XML interanal node writer of Sg##Type */                          \
  void XevSageVisitor::inodeSg##Type(SgNode* node){                     \
    inodeSgType(this,node);                                             \
  }

//VISIT_TYPE(ArrayType);
//VISIT_TYPE(ClassType);
//VISIT_TYPE(EnumType);
//VISIT_TYPE(FunctionType);
//VISIT_TYPE(ModifierType);
//VISIT_TYPE3(PointerType);
VISIT_TYPE(TypeBool);
VISIT_TYPE(TypeChar);
//VISIT_TYPE3(TypeComplex);
VISIT_TYPE(TypeCrayPointer);
VISIT_TYPE2(TypeDefault);
VISIT_TYPE(TypeDouble);
VISIT_TYPE2(TypeEllipse);
VISIT_TYPE(TypeFloat);
//VISIT_TYPE3(TypeImaginary);
//VISIT_TYPE(TypeInt);
//VISIT_TYPE(TypeLabel);
VISIT_TYPE(TypeLong);
VISIT_TYPE(TypeLongDouble);
VISIT_TYPE(TypeLongLong);
VISIT_TYPE(TypeShort);
VISIT_TYPE(TypeSignedChar);
VISIT_TYPE(TypeSignedInt);
VISIT_TYPE(TypeSignedLong);
VISIT_TYPE(TypeSignedLongLong);
VISIT_TYPE(TypeSignedShort);
//VISIT_TYPE(TypeString);
VISIT_TYPE(TypeUnsignedChar);
VISIT_TYPE(TypeUnsignedInt);
VISIT_TYPE(TypeUnsignedLong);
VISIT_TYPE(TypeUnsignedLongLong);
VISIT_TYPE(TypeUnsignedShort);
VISIT_TYPE(TypeVoid);
//VISIT_TYPE(TypedefType);



// ===============================================================================
/// Visitor of a SgArrayType element in an XML document
SgNode*
XevXmlVisitor::visitSgArrayType(xe::DOMNode* node, SgNode* astParent)
{
  SgArrayType*            ret = 0;
  SgArrayType*            ary = 0;
  SgArrayType*            sav = 0;
  SgType*                 typ = 0;
  SgExpression*           v;
  SgPointerType*          ptr = 0;
  SgPointerType*          ptrtyp = 0;
  string                  str1,str2;
  int                     rnk = 0;
  unsigned long           idx = 0;
  SgExpression*           iexp= 0;
  SgExprListExp*          lst = 0;
  XmlGetAttributeValue(node,"rank",&rnk);
  if(XmlGetAttributeValue(node,"index",&str2))
    idx = strtoul( str2.c_str(),0,0 );

  ret = new SgArrayType();
  ret->set_rank(rnk);
  //sav = ret;
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(typ==NULL)
        typ = isSgType(astchild);
      else {
        if(lst==NULL)
          lst =isSgExprListExp(iexp);
        if(iexp==NULL || iexp==lst )
          iexp = isSgExpression(astchild);
      }
    }
  SUBTREE_VISIT_END();

  if(typ){
    ret->set_base_type(typ);
    typ->set_parent(ret);
  }
  else{
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }

  if(lst !=NULL){
    if((size_t)rnk == lst->get_expressions().size()){
      ret->set_rank(rnk);
      ret->set_dim_info(lst);
    }
    else {
      XEV_DEBUG_INFO(node);
      XEV_ABORT();
    }
  }
  if(iexp !=NULL){
    ret->set_index(iexp);
    iexp->set_parent(ret);
  }

  ret->set_parent(astParent);

  return ret;
}
/** XML attribute writer of SgArrayType */
void XevSageVisitor::attribSgArrayType(SgNode* node) {
  SgArrayType* n = isSgArrayType(node);
  if(n){
    sstr() << " rank=\"" << n->get_rank() << "\" ";
    //SgUnsignedLongVal* ul = isSgUnsignedLongVal( n->get_index() );
    //if( ul ){
    //if(ul->get_valueString().size())
    //sstr() << " index=\"" << ul->get_valueString() << "\" ";
    //else
    //sstr() << " index=\"" << ul->get_value() << "\" ";
    //}
  }
  attribSgType(sstr(),node);
}
/** XML internal node writer of SgArrayType */
void XevSageVisitor::inodeSgArrayType(SgNode* node) {
  SgArrayType* n = isSgArrayType(node);
  if(n){
    this->visit(n->get_base_type());
    if(n->get_dim_info())
      this->visit(n->get_dim_info());
    //if(n->get_rank()==0)
    if(n->get_index()){
      SgExpression* idx = n->get_index();
      if(idx->get_file_info() && idx->get_file_info()->isSameFile(file_)==false){
        idx = isSgExpression(si::deepCopy(n->get_index()));
        si::setSourcePositionForTransformation(idx);
      }
      this->visit(idx);
    }

  }
  //SgExprListExp* lste = isSgArrayType(node)->get_dim_info();
  //this->visit(lste);
}

// ===============================================================================
/// Visitor of a SgClassType element in an XML document
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

  XmlGetAttributeValue(node,"name",&name);
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
    dec->set_firstNondefiningDeclaration(dec);
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
/** XML attribute writer of SgClassType */
void XevSageVisitor::attribSgClassType(SgNode* node)
{
  SgClassType* n = isSgClassType(node);
  SgDeclarationStatement* ds = n->get_declaration();
  SgClassDeclaration*     cd = isSgClassDeclaration(ds);
  //if( cd->get_isUnNamed() )
  //sstr() << " name=\"\" ";
  //else
  sstr() << " name=" << n->get_name() << " ";
  sstr() << " type=\"" << cd->get_class_type() << "\" ";
}
/** XML internal node writer of SgClassType */
void XevSageVisitor::inodeSgClassType(SgNode* node) {}


// ===============================================================================
// SgEnumType
// ===============================================================================
/** Visitor of a SgEnumType element in an XML document */
SgNode*
XevXmlVisitor::visitSgEnumType(xe::DOMNode* node, SgNode* astParent)
{
  string name;
  SgEnumType* ret =  0;
  SgEnumSymbol* esym =0;

  if(XmlGetAttributeValue(node,"name",&name)==false){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  esym = si::lookupEnumSymbolInParentScopes(name);
  if(esym){
    SgEnumDeclaration* decl = esym->get_declaration();
    ret = new SgEnumType(decl);
  }
  else{
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  ret->set_parent(astParent);
  return ret;
}
/** XML attribute writer of SgEnumType */
void XevSageVisitor::attribSgEnumType(SgNode* node) {
  SgEnumType* n = isSgEnumType(node);
  if(n){
    sstr() << " name=" << n->get_name() << " ";
  }
}
/** XML internal node writer of SgEnumType */
void XevSageVisitor::inodeSgEnumType (SgNode*) {}

// ===============================================================================
/// Visitor of a SgFunctionType element in an XML document
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

  if(lst==0){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
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
/** XML attribute writer of SgFunctionType */
void XevSageVisitor::attribSgFunctionType(SgNode* node) {
  attribSgType(sstr(),node);
}
/** XML internal node writer of SgFunctionType */
void XevSageVisitor::inodeSgFunctionType (SgNode* node)
{
  SgFunctionType* t = isSgFunctionType(node);
  if(t){
    this->visit(t->get_return_type());
    SgFunctionParameterTypeList* lst = t->get_argument_list();
    if(lst)
      this->visit(lst);
  }
}

// ===============================================================================
/// Visitor of a SgModifierType element in an XML document
SgNode*
XevXmlVisitor::visitSgModifierType(xe::DOMNode* node, SgNode* astParent)
{
  SgModifierType* ret=NULL;
  SgType* typ =NULL;
  string modtype;

  if(XmlGetAttributeValue(node,"modifier",&modtype)==false){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(typ==NULL)
        typ = isSgType(astchild);
    }
  SUBTREE_VISIT_END();

  if( modtype == "const" ) {
    //return sb::buildConstType(itype);
    //ret = new SgModifierType(typ);
    ret = sb::buildConstType(typ);
  }
  else if ( modtype == "volatile" )
    ret = sb::buildVolatileType(typ);
  else if ( modtype == "restrict" )
    ret = sb::buildRestrictType(typ);
  else return NULL;
  typ->set_parent(ret);
  ret->set_parent(astParent);
  return ret;
}
/** XML attribute writer of SgModifierType */
void XevSageVisitor::attribSgModifierType(SgNode* node){
  SgModifierType* n = isSgModifierType(node);
  if(n) {
    SgTypeModifier m = n->get_typeModifier();
    if( m.isRestrict() )
      sstr() << " modifier=\"restrict\" ";

    SgConstVolatileModifier cv = m.get_constVolatileModifier();
    if( cv.isConst() )
      sstr() << " modifier=\"const\" ";
    else if(cv.isVolatile() )
      sstr() << " modifier=\"volatile\" ";
  }
}
/** XML interanal node writer of SgModifierType */
void XevSageVisitor::inodeSgModifierType (SgNode* node) {
  SgModifierType* n = isSgModifierType(node);
  if(n){
    this->visit(n->get_base_type());
  }
}


// ===============================================================================
/// Visitor of a SgTypeString element in an XML document
SgNode*
XevXmlVisitor::visitSgTypeString(xe::DOMNode* node, SgNode* astParent)
{
  SgTypeString* ret = 0;
  int len=0;
  SgExpression *kexp=0,*lexp=0;

  XmlGetAttributeValue(node,"len",&len);

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(len && lexp==0){
        lexp = isSgExpression(astchild);
      }
      else if(kexp==0){
        kexp = isSgExpression(astchild);
      }
    }
  SUBTREE_VISIT_END();

  if(lexp==0)
    lexp = new SgAsteriskShapeExp(DEFAULT_FILE_INFO);
  lexp->set_parent(ret);
  // don't use SageBuilder::buildStringType()
  // it does not work with Fortran type kind
  ret = SgTypeString::createType( lexp,kexp );

  if(ret==0) {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }

  ret->set_parent(astParent);
  return ret;
}
/** XML attribute writer of SgTypeString */
void XevSageVisitor::attribSgTypeString(SgNode* node){
  SgExpression*   exp = isSgTypeString(node)->get_lengthExpression();
  if( exp ) {
    sstr() << " len=\"1\" ";
  }
  attribSgType(sstr(),node);
}
/** XML internal node writer of SgTypeString */
void XevSageVisitor::inodeSgTypeString(SgNode* node){
  if(isSgTypeString(node)->get_lengthExpression())
    this->visit(isSgTypeString(node)->get_lengthExpression());
  inodeSgType(this,node);
}

// ===============================================================================
/// Visitor of a SgTypedefType element in an XML document
SgNode*
XevXmlVisitor::visitSgTypedefType(xe::DOMNode* node, SgNode* astParent)
{
  SgTypedefType*          ret = 0;
  string                  name;
  SgDeclarationStatement* ds = 0;

  XmlGetAttributeValue(node,"name",&name);
  SgTypedefSymbol* tsym = si::lookupTypedefSymbolInParentScopes(name);
  if(tsym)
    ds = isSgDeclarationStatement( tsym->get_declaration() );
  else {
    SgScopeStatement* scope = sb::topScopeStack();
    SgTypedefDeclaration* dec
      //don't use high-level build function
      = new SgTypedefDeclaration(DEFAULT_FILE_INFO,name,
                                 SgTypeUnknown::createType());

    dec->set_name(name);
    dec->set_parent(scope);
    dec->set_scope(scope);
    dec->set_firstNondefiningDeclaration(dec);
    //dec->set_definition(NULL);
    dec->set_definingDeclaration(NULL);
    ds = isSgDeclarationStatement( dec );
    ds->setForward();
    // don't insert symbol!
  }
  XEV_ASSERT(ds!=NULL);
  ret = new SgTypedefType( ds );
  ret->set_parent(astParent);
  return ret;
}
/** XML attribute writer of SgTypedefType */
void XevSageVisitor::attribSgTypedefType(SgNode* node){
  SgTypedefType* n = isSgTypedefType(node);
  sstr() << " name=" << n->get_name() << " ";
  attribSgType(sstr(),node);
}
/** XML internal node writer of SgTypedefType */
void XevSageVisitor::inodeSgTypedefType(SgNode* node){}

// ===============================================================================
/// Visitor of a SgTypeComplex element in an XML document
SgNode*
XevXmlVisitor::visitSgTypeComplex(xe::DOMNode* node, SgNode* astParent)
{
  SgTypeComplex*    ret = 0;
  SgType*           typ = 0;
  SgExpression*    kexp = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(typ==NULL)
        typ = isSgType(astchild);
      if(kexp==NULL)
        kexp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();
  if(typ==NULL){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  ret = sb::buildComplexType( typ );
  if(ret==NULL) {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }

  if( kexp ) {
    ret->set_type_kind( kexp );
  }
  ret->set_parent(astParent);
  return ret;
}
/** XML attribute writer of SgTypeComplex */
void XevSageVisitor::attribSgTypeComplex(SgNode* node){
  attribSgType(sstr(),node);
}
/** XML internal node writer of SgTypeComplex */
void XevSageVisitor::inodeSgTypeComplex(SgNode* node){
  inodeSgType(this,node);
  this->visit(isSgTypeComplex(node)->get_base_type());
}


// ===============================================================================
/// Visitor of a SgTypeImaginary element in an XML document
SgNode*
XevXmlVisitor::visitSgTypeImaginary(xe::DOMNode* node, SgNode* astParent)
{
  SgTypeImaginary*  ret = 0;
  SgType*           typ = 0;
  SgExpression*     kexp = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(typ==NULL)
        typ = isSgType(astchild);
      if(kexp==0)
        kexp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();
  if(typ==NULL){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  ret = sb::buildImaginaryType( typ );
  if(ret==NULL) {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }

  if( kexp ) {
    ret->set_type_kind( kexp );
  }
  ret->set_parent(astParent);
  return ret;
}
/** XML attribute writer of SgTypeImaginary */
void XevSageVisitor::attribSgTypeImaginary(SgNode* node){
  attribSgType(sstr(),node);
}
/** XML internal node writer of SgTypeImaginary */
void XevSageVisitor::inodeSgTypeImaginary(SgNode* node){
  inodeSgType(this,node);
  this->visit(isSgTypeImaginary(node)->get_base_type());
}


// ===============================================================================
/// Visitor of a SgTypeInt element in an XML document
SgNode*
XevXmlVisitor::visitSgTypeInt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgType* ret=NULL;
  SgExpression* kexp = NULL;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(kexp==0)
        kexp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret= SgTypeInt::createType(0,kexp);
  ret->set_parent(astParent);
  return ret;
}
/** XML attribute writer of SgTypeInt */
void XevSageVisitor::attribSgTypeInt(SgNode* node){
  attribSgType(sstr(),node);
}
/** XML internal node writer of SgTypeInt */
void XevSageVisitor::inodeSgTypeInt(SgNode* node){
  inodeSgType(this,node);
}

// ===============================================================================
/// Visitor of a SgTypeLabel element in an XML document
SgNode*
XevXmlVisitor::visitSgTypeLabel(xercesc::DOMNode* node, SgNode* astParent)
{
  SgTypeLabel* ret=NULL;
  string name;
  SgExpression* kexp = NULL;


  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(kexp==0)
        kexp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret= SgTypeLabel::createType(kexp);
  if(XmlGetAttributeValue(node,"name",&name))
    ret->set_name(name);
  ret->set_parent(astParent);
  return ret;
}
/** XML attribute writer of SgTypeLabel */
void XevSageVisitor::attribSgTypeLabel(SgNode* node){
  SgTypeLabel* n = isSgTypeLabel(node);

  if(n){
    sstr() << " name=" << n->get_name() << " ";
  }
  attribSgType(sstr(),node);
}
/** XML internal node writer of SgTypeLabel */
void XevSageVisitor::inodeSgTypeLabel(SgNode* node){
  inodeSgType(this,node);
}


// ===============================================================================
/// Visitor of a SgPointerType element in an XML document
SgNode*
XevXmlVisitor::visitSgPointerType(xe::DOMNode* node, SgNode* astParent)
{
  SgPointerType*      ret = new SgPointerType();
  SgType*             typ = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(typ==NULL){
        typ = isSgType(astchild);
        if(typ)
          ret->set_base_type(typ);
      }
    }
  SUBTREE_VISIT_END();

  if(ret==NULL||typ==NULL) {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  ret->set_parent(astParent);
  return ret;
}
/** XML attribute writer of SgPointerType */
void XevSageVisitor::attribSgPointerType(SgNode* node){
  attribSgType(sstr(),node);
}
/** XML interanal node writer of SgPointerType */
void XevSageVisitor::inodeSgPointerType(SgNode* node){
  SgPointerType* n = isSgPointerType(node);
  if(n){
    this->visit(n->get_base_type());
  }
}
