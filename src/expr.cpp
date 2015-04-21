/**
 * @file     expr.cpp
 * @brief    Visitor Functions of SgExpression classes.
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
#include "common.hpp"
#include "xml2rose.hpp"
#include "rose2xml.hpp"

//* ROSE headers */
#include "fortran_support.h"
namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;
using namespace XevXml;


static void attribSgExpression(ostream& istr,SgNode* node)
{
  SgExpression*  n = isSgExpression(node);
  if(n==0)return;

  if(n->get_need_paren())
    istr << " paren=\"1\" ";

  if(n->get_lvalue())
    istr << " lvalue=\"1\" ";

  SgInitializer* ini = isSgInitializer(node);
  if(ini){
    SgCastExp* c = isSgCastExp(ini->get_originalExpressionTree());
    if(c && c->get_file_info() &&c->get_file_info()->isCompilerGenerated()==false)
      istr << " cast=\"" << ini->get_is_explicit_cast() <<"\" ";
    //istr << " cast=\"1\" ";
  }
}

#define ATTRIB_EXPR_DEFAULT(x)                      \
  /** XML attriute writer of Sg##x */               \
  void XevSageVisitor::attribSg##x(SgNode* node)    \
  {attribSgExpression(sstr(),node);}

#define INODE_EXPR_DEFAULT(x)                       \
  /** XML internal node writer of Sg##x */          \
  void XevSageVisitor::inodeSg##x(SgNode* node)     \
  {return;}

#define INODE_EXPR_TYPE(x)                          \
  /** XML internal node writer of Sg##x */          \
  void XevSageVisitor::inodeSg##x(SgNode* node){    \
    Sg##x* n = isSg##x(node);                       \
      if(n)                                         \
        this->visit(n->get_type());                 \
  }

#define EXPR_DEFAULT(x)                         \
  ATTRIB_EXPR_DEFAULT(x)                        \
  INODE_EXPR_DEFAULT(x)

// ===============================================================================
/// Visitor of a SgActualArgumentExpression element in an XML document
SgNode*
XevXmlVisitor::visitSgActualArgumentExpression(xe::DOMNode* node, SgNode* astParent)
{
  SgActualArgumentExpression* ret = 0;
  SgExpression*               exp = 0;
  std::string                 name;

  XmlGetAttributeValue(node,"name",&name);
  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if( exp==0 )
        exp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  if(exp!=NULL)
    ret = sb::buildActualArgumentExpression( name,exp );
  else {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }

  ret->set_parent(astParent);
  exp->set_parent(ret);
  return ret;
}
//ATTRIB_EXPR_DEFAULT(CastExp);
void XevSageVisitor::attribSgActualArgumentExpression(SgNode* node)
{
  SgActualArgumentExpression* actual = isSgActualArgumentExpression(node);
  if(actual){
    sstr() << " name=" << actual->get_argument_name() << " ";
  }

  attribSgExpression(sstr(),node);
}
INODE_EXPR_DEFAULT(ActualArgumentExpression);

// ===============================================================================
/// Visitor of a SgAggregateInitializer element in an XML document
SgNode*
XevXmlVisitor::visitSgAggregateInitializer(xe::DOMNode* node, SgNode* astParent)
{
  SgAggregateInitializer* ret = 0;
  SgExprListExp*          lst = 0;
  //SgType*                 typ = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if( lst==0 )
        lst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  if(lst!=NULL)
    ret = sb::buildAggregateInitializer( lst,lst->get_type() );
  else {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }

  ret->set_parent(astParent);
  lst->set_parent(ret);

  int brace=0;
  XmlGetAttributeValue(node,"need_brace",&brace);
  ret->set_need_explicit_braces(brace);


  return ret;
}
void XevSageVisitor::attribSgAggregateInitializer(SgNode* node)
{
  SgAggregateInitializer* n = isSgAggregateInitializer(node);
  if(n){
    if(n->get_need_explicit_braces())
      sstr() << " need_brace=\"1\" ";
  }
  attribSgExpression(sstr(),node);
}
INODE_EXPR_DEFAULT(AggregateInitializer);

// ===============================================================================
/// Visitor of a SgAssignInitializer element in an XML document
SgNode*
XevXmlVisitor::visitSgAssignInitializer(xe::DOMNode* node, SgNode* astParent)
{
  SgAssignInitializer* ret = 0;
  SgExpression*        exp = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(exp==0)
        exp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  if(exp){
    ret = sb::buildAssignInitializer(exp,exp->get_type());
    exp->set_parent(ret);
  }
  else {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  return ret;
}
EXPR_DEFAULT(AssignInitializer);


// ===============================================================================
/// Visitor of a SgAsteriskShapeExp element in an XML document
SgNode*
XevXmlVisitor::visitSgAsteriskShapeExp(xe::DOMNode* node, SgNode* astParent)
{
  SgAsteriskShapeExp* ret = new SgAsteriskShapeExp(DEFAULT_FILE_INFO);   // 0822
  ret->set_parent(astParent);
  return ret;
}
EXPR_DEFAULT(AsteriskShapeExp);



// ===============================================================================
/// Visitor of a SgCastExp element in an XML document
SgNode*
XevXmlVisitor::visitSgCastExp(xe::DOMNode* node, SgNode* astParent)
{
  SgCastExp*     ret   = 0;
  SgType*        typ   = 0;
  SgExpression*  exp   = 0;
  int            cty   = 0;
  int            imp   = 0;

  XmlGetAttributeValue(node,"ctype",&cty);
  XmlGetAttributeValue(node,"implicit",&imp);

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(exp==0)
        exp = isSgExpression(astchild);
      if(typ==0)
        typ = isSgType(astchild);
    }
  SUBTREE_VISIT_END();

  if(imp){
    // for nested implicit casting
    if(isSgCastExp(exp)){
      exp->set_parent(astParent);
      return exp;
    }
  }

  if(typ && exp){
    ret = sb::buildCastExp(exp,typ,(SgCastExp::cast_type_enum)cty);
    ret->set_parent(astParent);
  }
  else if(exp){
    // ignore implicit type conversions
    XEV_WARN("An SgCastExp node ignored");
    XEV_DEBUG_INFO(node);
    exp->set_parent(astParent);
    return exp;
  }
  else {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  if(imp){
    // ignore implicit type conversions
    ret->get_startOfConstruct()->setCompilerGenerated();
    ret->get_operatorPosition()->setCompilerGenerated();
    ret->get_file_info()->setCompilerGenerated();
  }
  return ret;
}
//ATTRIB_EXPR_DEFAULT(CastExp);
void XevSageVisitor::attribSgCastExp(SgNode* node)
{
  SgCastExp* cast = isSgCastExp(node);
  if(cast){
    sstr() << " ctype=\"" << cast->get_cast_type() << "\" ";
    //Sg_File_Info* info = cast->get_file_info();
    Sg_File_Info* info = cast->get_startOfConstruct();
    if(info && info->isCompilerGenerated()){
      sstr() << " implicit=\"1\" ";
    }
  }

  attribSgExpression(sstr(),node);
}

INODE_EXPR_TYPE(CastExp);



// ===============================================================================
/// Visitor of a SgColonShapeExp element in an XML document
SgNode*
XevXmlVisitor::visitSgColonShapeExp(xe::DOMNode* node, SgNode* astParent)
{
  SgExpression * ret = new SgColonShapeExp();
  ret->set_parent(astParent);
  return ret;
}
EXPR_DEFAULT(ColonShapeExp);

#if 0
// ===============================================================================
/// Visitor of a SgCompoundInitializer element in an XML document
// SgCompoundInitializer seems an AST node for OpenCL vector initialization.
// e.g. float2 x = float2(1.0f,1.0f).
// XevXML does not support OpenCL. So the following methods are commented out.
SgNode*
XevXmlVisitor::visitSgCompoundInitializer(xe::DOMNode* node, SgNode* astParent)
{
  SgCompoundInitializer*  ret = 0;
  SgExprListExp*          lst = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if( lst==0 )
        lst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildCompoundInitializer( lst,lst->get_type() );
  ret->set_parent(astParent);
  lst->set_parent(ret);
  //typ->set_parent(ret);
  return ret;
}
EXPR_DEFAULT(CompoundInitializer);
#endif

// ===============================================================================
/// Visitor of a SgConditionalExp element in an XML document
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
EXPR_DEFAULT(ConditionalExp);


// ===============================================================================
/// Visitor of a SgConstructorInitializer element in an XML document
SgNode*
XevXmlVisitor::visitSgConstructorInitializer(xercesc::DOMNode* node, SgNode* astParent)
{
  SgConstructorInitializer* ret      = 0;
  SgMemberFunctionDeclaration* mdecl = 0;
  SgExprListExp* elst                = 0;
  SgType*        typ                 = 0;
  int name     =0;
  int qual     =0;
  int paren    =0;
  int unkc     =0;

  XmlGetAttributeValue(node,"name",             &name);
  XmlGetAttributeValue(node,"qual",             &qual);
  XmlGetAttributeValue(node,"paren_after_name", &paren);
  XmlGetAttributeValue(node,"unknown",          &unkc);

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
/** XML attribute writer of SgConstructorInitializer */
void XevSageVisitor::attribSgConstructorInitializer(SgNode* node)
{
  SgConstructorInitializer* n = isSgConstructorInitializer(node);
  if(n){
    sstr() << " name=\"" << n->get_need_name() << "\" ";
    sstr() << " qual=\"" << n->get_need_qualifier() << "\" ";
    sstr() << " paren_after_name=\"" << n->get_need_parenthesis_after_name() << "\" ";
    sstr() << " unknown=\"" << n->get_associated_class_unknown() << "\" ";
  }
  attribSgExpression(sstr(),node);
}
INODE_EXPR_TYPE(ConstructorInitializer);


// ===============================================================================
/// Visitor of a SgDotExp element in an XML document
SgNode*
XevXmlVisitor::visitSgDotExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDotExp*     ret = new SgDotExp(DEFAULT_FILE_INFO);
  SgExpression* lhs = 0;
  SgExpression* rhs = 0;
  SgClassDefinition*   defn=0;
  bool pushed = false;

  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( lhs==0 ){
        lhs = isSgExpression(astchild);
        if(lhs) {
          SgClassType* ctype = isSgClassType(lhs->get_type());
          if(ctype==0)
            //ctype = isSgClassType(si::getElementType(lhs->get_type()));
            ctype = isSgClassType(lhs->get_type()->findBaseType());
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
            }
          }
          else {
            //XEV_WARN( lhs->get_type()->class_name() );
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

  return ret;
}
EXPR_DEFAULT(DotExp);

// ===============================================================================
/// Visitor of a SgArrowExp element in an XML document
SgNode*
XevXmlVisitor::visitSgArrowExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgArrowExp*     ret = 0;
  SgExpression* lhs = 0;
  SgExpression* rhs = 0;
  SgClassDefinition*   defn=0;
  ret = sb::buildArrowExp( lhs,rhs);
  bool pushed = false;

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( lhs==0 ){
        lhs = isSgExpression(astchild);
        if(lhs) {
          SgClassType    *ctype = 0;
          SgPointerType  *ptype = isSgPointerType(lhs->get_type());

          if(ptype==0 && isSgTypedefType(lhs->get_type())){
            SgTypedefType* ttype = isSgTypedefType(lhs->get_type());
            ptype = isSgPointerType(ttype->get_base_type());
          }
          else if(ptype==0 && isSgModifierType(lhs->get_type())){
            SgModifierType* mtype = isSgModifierType(lhs->get_type());
            ptype = isSgPointerType(mtype->get_base_type());
          }

          if(ptype)
            ctype = isSgClassType(si::getElementType(ptype));
          else{
            XEV_WARN(lhs->get_type()->class_name() <<" is given as lhs of SgArrowExp");
            XEV_DEBUG_INFO(node);
            XEV_ABORT();
          }

          if(ctype==0){
            ctype = isSgClassType(ptype->findBaseType());
#if 0
            SgTypedefType* ttype = isSgTypedefType(ptype->get_base_type());
            SgModifierType* mtype = isSgModifierType(ptype->get_base_type());
            if(ttype){
              ctype = isSgClassType(ttype->get_base_type());
            }
            else if(mtype){
              ctype = isSgClassType(mtype->get_base_type());
            }
#endif
          }

          if(ctype){
            SgClassDeclaration* decl = isSgClassDeclaration(ctype->get_declaration());
            if(decl && decl->get_definition()==0)
              decl = isSgClassDeclaration(decl->get_definingDeclaration());

            // defining declaration is found
            if(decl && decl->get_definition()){
              defn = decl->get_definition();
              sb::pushScopeStack(defn);
              pushed = true;
            }
            // defining declaration is not found
            else {
              XEV_WARN( "Defning declaration of " << ctype->get_name().getString() << " is not found");
            }
          }
          else {
            //XEV_WARN( ptype->get_base_type()->class_name() <<" is found");
            XEV_DEBUG_INFO(node);
            XEV_ABORT();
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
  if(lhs == 0 || rhs == 0) XEV_ABORT();
  ret->set_lhs_operand(lhs);
  ret->set_rhs_operand(rhs);
  lhs->set_parent(ret);
  rhs->set_parent(ret);
  ret->set_parent(astParent);
  return ret;
}
EXPR_DEFAULT(ArrowExp);

// ===============================================================================
/// Visitor of a SgExprListExp element in an XML document
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
        // commented out because this should be set by the node itself
        //if(exp->get_startOfConstruct()==NULL)
        //exp->set_startOfConstruct(DEFAULT_FILE_INFO);
        ret->append_expression(exp);
      }
    }
  SUBTREE_VISIT_END();
  return ret;
}
EXPR_DEFAULT(ExprListExp);


// ===============================================================================
/// Visitor of a SgFunctionCallExp element in an XML document
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
EXPR_DEFAULT(FunctionCallExp);

static SgFunctionSymbol* generateImplicitFunctionSymbol(xe::DOMNode* node, string& name, SgSourceFile* file)
{
    // see generateFunctionRefExp() in fortran_support.C
  SgFunctionType* ftype = new SgFunctionType(generateImplicitType(name),false);
  SgProcedureHeaderStatement* decl =  new SgProcedureHeaderStatement(name,ftype,NULL);
  SgScopeStatement* scope = sb::topScopeStack();
  decl->set_firstNondefiningDeclaration(decl);
  decl->set_definingDeclaration(NULL);
  if( isSgClassDefinition(scope) == NULL ){
    scope = TransformationSupport::getClassDefinition(sb::topScopeStack());
    if( scope == NULL ){
      scope = file->get_globalScope();
      //scope = TransformationSupport::getGlobalScope(sb::topScopeStack());
    }
  }
  if(scope==NULL) {
    XEV_DEBUG_INFO(node);XEV_ABORT();
  }
  decl->set_scope(scope);
  decl->set_parent(scope);
  decl->set_subprogram_kind(SgProcedureHeaderStatement::e_function_subprogram_kind);
  decl->set_file_info(DEFAULT_FILE_INFO);
  SgFunctionSymbol * fsym = new SgFunctionSymbol(decl);
  scope->insert_symbol(name,fsym);

  return fsym;
}

// ===============================================================================
/// Visitor of a SgFunctionRefExp element in an XML document
SgNode*
XevXmlVisitor::visitSgFunctionRefExp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFunctionRefExp* ret = 0;

  //xe::DOMNamedNodeMap* amap = node->getAttributes();
  //xe::DOMNode*         satt = 0;
  string               name;
  //stringstream         val;
  int                  kind;

  //sb::pushScopeStack(_file->get_globalScope());
  if( XmlGetAttributeValue(node,"name",&name) == false){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  SgFunctionSymbol* functionSymbol = si::lookupFunctionSymbolInParentScopes(SgName(name));
  if(functionSymbol==0 && si::is_Fortran_language()==true){
    // function symbol is not found. this means the function is not declared yet.
    // see generateFunctionRefExp() in fortran_support.C

    functionSymbol = generateImplicitFunctionSymbol(node,name,_file);

#if 0
    // for debugging
    if( matchAgainstIntrinsicFunctionList(name) == false ){
      XEV_WARN("\"" << name << "\" is not declared yet");
    }
    else {
      XEV_WARN("\"" << name << "\" is an intrinsic function");
    }
#endif
  }
  if(functionSymbol==NULL){
    //build a function based on the name (C language)
    ret = sb::buildFunctionRefExp(name);
  }
  else
    ret = sb::buildFunctionRefExp( functionSymbol );

  if(XmlGetAttributeValue(node,"kind",&kind )){
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
    else {
      // this is not an error.
      //XEV_DEBUG_INFO(node);
      //XEV_ABORT();
    }
  }

  return ret;
}
/** XML attribute writer of SgFunctionRefExp */
void XevSageVisitor::attribSgFunctionRefExp(SgNode* node){
  SgFunctionRefExp* n = isSgFunctionRefExp(node);
  if(n){
    sstr() << " name=" << n->get_symbol()->get_name() << " ";

    SgProcedureHeaderStatement* h =
      isSgProcedureHeaderStatement( n->get_symbol()->get_declaration() );
    if(h && h->get_subprogram_kind () != SgProcedureHeaderStatement::e_function_subprogram_kind){
      sstr() << " kind=\"" << h->get_subprogram_kind () << "\" ";
    }
  }
  attribSgExpression(sstr(),node);
}
INODE_EXPR_DEFAULT(FunctionRefExp);


// ===============================================================================
/// Visitor of a SgImpliedDo element in an XML document
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

  ret = new SgImpliedDo(DEFAULT_FILE_INFO,ini,las,inc,lst,scp);
  ini->set_parent(ret);
  las->set_parent(ret);
  inc->set_parent(ret);
  lst->set_parent(ret);
  ret->set_parent(astParent);
  return ret;
}
EXPR_DEFAULT(ImpliedDo);


// ===============================================================================
/// Visitor of a SgLabelRefExp element in an XML document
SgNode*
XevXmlVisitor::visitSgLabelRefExp(xe::DOMNode* node, SgNode* astParent)
{
  int                   ino   = 0;
  int                   type  = 0;

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
/** XML attribute writer of SgLabelRefExp */
void XevSageVisitor::attribSgLabelRefExp(SgNode* node){
  SgLabelRefExp*      n = isSgLabelRefExp(node);

  if(n) {
    sstr() << " type=\"" << n->get_symbol()->get_label_type() << "\" ";
    if(n->get_symbol()->get_label_type() != SgLabelSymbol::e_alternative_return_type){
      sstr() << " nlabel=\"" << n->get_symbol()->get_numeric_label_value() << "\" ";
    }
    else {
      SgFunctionDefinition* fdef = si::getEnclosingFunctionDefinition(node);
      SgFunctionDeclaration* decl = NULL;
      if( fdef == 0 || (decl=fdef->get_declaration()) == 0){
        XEV_WARN("def="<<fdef<<", decl="<<decl<<", parent="<<node->get_parent()->class_name());
        XEV_ABORT();
      }
      SgInitializedNamePtrList& args =decl->get_args();
      SgInitializedName* ini = n->get_symbol()->get_fortran_alternate_return_parameter();
      int counter=0;
      for(size_t i(0);i<args.size();i++){
        if(isSgTypeLabel(args[i]->get_type())){
          counter++;
          if(ini == args[i]){
            sstr() << " nlabel=\"" << counter << "\" ";
            break;
          }
        }
      }
    }
  }
  attribSgExpression(sstr(),node);
}
INODE_EXPR_DEFAULT(LabelRefExp);


// ===============================================================================
/// Visitor of a SgNullExpression element in an XML document
SgNode*
XevXmlVisitor::visitSgNullExpression(xe::DOMNode* node, SgNode* astParent)
{
  SgNullExpression* ret = sb::buildNullExpression();
  ret->set_parent(astParent);
  return ret;
}
EXPR_DEFAULT(NullExpression);


// ===============================================================================
/// Visitor of a SgPointerDerefExp element in an XML document
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
  if(exp==0||typ==0){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  ret = new SgPointerDerefExp(DEFAULT_FILE_INFO,exp,typ);
  ret->set_parent(astParent);
  exp->set_parent(ret);
  typ->set_parent(ret);
  return ret;
}
ATTRIB_EXPR_DEFAULT(PointerDerefExp);
INODE_EXPR_TYPE(PointerDerefExp);


// ===============================================================================
/// Visitor of a SgSizeOfOp element in an XML document
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
ATTRIB_EXPR_DEFAULT(SizeOfOp);
/** XML internal node writer of SgSizeOfOp */
void XevSageVisitor::inodeSgSizeOfOp(SgNode* node){
  SgType* typ = isSgSizeOfOp(node)->get_operand_type();
  if( typ )
    this->visit(typ);
}


// ===============================================================================
/// Visitor of a SgSubscriptExpression element in an XML document
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
  ret->set_startOfConstruct(DEFAULT_FILE_INFO);
  low->set_parent(ret);
  upp->set_parent(ret);
  str->set_parent(ret);
  ret->set_parent(astParent);

  return ret;
}
EXPR_DEFAULT(SubscriptExpression);


// ===============================================================================
/// Visitor of a SgVarArgEndOp element in an XML document
SgNode*
XevXmlVisitor::visitSgVarArgEndOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgVarArgEndOp*  ret = 0;
  SgExpression*   lhs = 0;


  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if( lhs==0 )
        lhs = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = new SgVarArgEndOp( DEFAULT_FILE_INFO, lhs, lhs->get_type() );
  ret->set_parent(astParent);
  lhs->set_parent(ret);
  return ret;
}
EXPR_DEFAULT(VarArgEndOp);


// ===============================================================================
/// Visitor of a SgVarArgOp element in an XML document
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
      if( lhs==0 )
        lhs = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = new SgVarArgOp( DEFAULT_FILE_INFO, lhs, typ );
  //ret = sb::buildVarArgOp_nfi( lhs,lhs->get_type() );
  ret->set_parent(astParent);
  lhs->set_parent(ret);
  typ->set_parent(ret);
  return ret;
}
ATTRIB_EXPR_DEFAULT(VarArgOp);
/** XML interanal node writer of SgVarArgOp */
void XevSageVisitor::inodeSgVarArgOp(SgNode* node)
{
  this->visit( isSgVarArgOp(node)->get_expression_type());
}


// ===============================================================================
/// Visitor of a SgVarArgStartOp element in an XML document
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

  ret = new SgVarArgStartOp(DEFAULT_FILE_INFO, lhs, rhs, lhs->get_type() );
  ret->set_parent(astParent);
  lhs->set_parent(ret);
  rhs->set_parent(ret);
  return ret;
}
EXPR_DEFAULT(VarArgStartOp);



// ===============================================================================
/// Visitor of a SgVarRefExp element in an XML document
SgNode*
XevXmlVisitor::visitSgVarRefExp(xe::DOMNode* node, SgNode* astParent)
{
  string       name;
  SgVarRefExp* ret=0;
  SgScopeStatement* scope = sb::topScopeStack();

  if(XmlGetAttributeValue(node,"name",&name)==false){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
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
      SgSymbol* sym  = si::lookupSymbolInParentScopes(name);
      if(isSgAliasSymbol(sym)){
        SgAliasSymbol* asym = isSgAliasSymbol(sym);
        vsym = isSgVariableSymbol(asym->get_alias());
        if(vsym==0) {
          XEV_DEBUG_INFO(node);
          XEV_ABORT();
        }
        name1 = vsym->get_declaration();
      }
      else if(si::is_Fortran_language()){
        // implicit variables
        scope = si::getEnclosingProcedure (sb::topScopeStack());
        if(scope==NULL) scope = _file->get_globalScope();
        name1  = sb::buildInitializedName(name,generateImplicitType(name));

        name1->set_scope(scope);
        vsym= new SgVariableSymbol(name1);
        vsym->set_parent(scope);
      }
      else {
        XEV_WARN("variable " << name << " is not found in any symbol tables.");
        //XEV_DEBUG_INFO(node);
        //XEV_ABORT();

        /* put a fake with no scope and unknown type */
        name1 = sb::buildInitializedName(name,SgTypeUnknown::createType());
        name1->set_scope(scope); // NULL?
        vsym = new SgVariableSymbol(name1);
        vsym->set_parent(scope);
      }
      ret = new SgVarRefExp(vsym);
      ret->set_symbol(vsym);
      if(vsym)
        ret->get_symbol()->set_declaration(name1);
      si::setOneSourcePositionForTransformation(ret);
    }
  }
  else {
    //ret= sb::buildVarRefExp(name);
    //vsym could be SgAliasSymbol but SageBuilder cannot handle it.
    ret= new SgVarRefExp(vsym); // don't use SageBuilder.
    si::setOneSourcePositionForTransformation(ret);
  }
  ret->set_parent(astParent);

  return ret;
}
/** XML attribute writer of SgVarRefExp */
void XevSageVisitor::attribSgVarRefExp(SgNode* node)
{
  SgVarRefExp* n = isSgVarRefExp(node);
  if(n) {
    sstr() << " name=" << n->get_symbol()->get_name();
  }
  attribSgExpression(sstr(),node);
}
INODE_EXPR_DEFAULT(VarRefExp);

//-----------------------------------------------------------------------
// Builder functions for operators
//----------------------------------------------------------------------
#define VISIT_BOP(op)                                                   \
  /** Visitor of a Sg##op element in an XML document */                 \
  SgNode*                                                               \
  XevXmlVisitor::visitSg##op(xercesc::DOMNode* node, SgNode* astParent) \
  {                                                                     \
    SgExpression* lhs = 0;                                              \
    SgExpression* rhs = 0;                                              \
    xe::DOMNode* child=node->getFirstChild();                           \
    Sg##op* ret = sb::build##op(lhs,rhs);                               \
    ret->set_parent(astParent);                                         \
    ret->set_file_info(DEFAULT_FILE_INFO);                              \
    while(child) {                                                      \
      if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){       \
        SgNode* astchild = this->visit(child,ret);                      \
        if(lhs==0)                                                      \
          lhs = isSgExpression(astchild);                               \
        else if(rhs==0)                                                 \
          rhs = isSgExpression(astchild);                               \
      }                                                                 \
      child=child->getNextSibling();                                    \
    }                                                                   \
    if( lhs && rhs ){                                                   \
      ret->set_lhs_operand(lhs);                                        \
      ret->set_rhs_operand(rhs);                                        \
      return ret;                                                       \
    }                                                                   \
    else {                                                              \
      XEV_DEBUG_INFO(node);                                             \
      XEV_ABORT();                                                      \
    }                                                                   \
  }                                                                     \
  /** XML attribute writer of Sg##op */                                 \
  void XevSageVisitor::attribSg##op(SgNode* node)                       \
  {attribSgExpression(sstr(),node);}                                    \
  /** XML internal node writer of Sg##op */                             \
  void XevSageVisitor::inodeSg##op(SgNode* node)                        \
  {return;}

#define VISIT_UOP(op)                                                   \
  /** Visitor of a Sg##op element in an XML document */                 \
  SgNode*                                                               \
  XevXmlVisitor::visitSg##op(xercesc::DOMNode* node, SgNode* astParent) \
  {                                                                     \
    SgExpression* exp = 0;                                              \
    xe::DOMNode* child=node->getFirstChild();                           \
    while(child) {                                                      \
      if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){       \
        SgNode* astchild = this->visit(child);                          \
        if(exp==0)                                                      \
          exp = isSgExpression(astchild);                               \
      }                                                                 \
      child=child->getNextSibling();                                    \
    }                                                                   \
    SgNode* ret = NULL;                                                 \
    if( exp ) {                                                         \
        ret = sb::build##op(exp);                                       \
          if(ret==NULL) XEV_DEBUG_INFO(node);                           \
    }                                                                   \
    if(ret==NULL) {                                                     \
      XEV_DEBUG_INFO(node);                                             \
      XEV_ABORT();                                                      \
    }                                                                   \
    ret->set_parent(astParent);                                         \
    return ret;                                                         \
  }                                                                     \
  /** XML attribute writer of Sg##op */                                 \
  void XevSageVisitor::attribSg##op(SgNode* node)                       \
  {attribSgExpression(sstr(),node);}                                    \
  /** XML internal noed writer of Sg##op */                             \
  void XevSageVisitor::inodeSg##op(SgNode* node)                        \
  {return;}

#define VISIT_UOP_MODE(op)                                              \
  /** Visitor of a Sg##op element in an XML document */                 \
  SgNode*                                                               \
  XevXmlVisitor::visitSg##op(xercesc::DOMNode* node, SgNode* astParent) \
  {                                                                     \
    SgExpression* exp = 0;                                              \
    int imode = 0;                                                      \
    bool fmode= false;                                                  \
    fmode = XmlGetAttributeValue(node,"mode",&imode);                   \
    xe::DOMNode* child=node->getFirstChild();                           \
    while(child) {                                                      \
      if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){       \
        SgNode* astchild = this->visit(child);                          \
        if(exp==0)                                                      \
          exp = isSgExpression(astchild);                               \
      }                                                                 \
      child=child->getNextSibling();                                    \
    }                                                                   \
    SgNode* ret = NULL;                                                 \
    if( exp ) {                                                         \
      if(fmode)                                                         \
        ret = sb::build##op(exp,                                        \
                            imode?SgUnaryOp::postfix:SgUnaryOp::prefix);\
      else                                                              \
        ret = sb::build##op(exp);                                       \
          if(ret==NULL) XEV_DEBUG_INFO(node);                           \
    }                                                                   \
    if(ret==NULL) {                                                     \
      XEV_DEBUG_INFO(node);                                             \
      XEV_ABORT();                                                      \
    }                                                                   \
    ret->set_parent(astParent);                                         \
    return ret;                                                         \
  }                                                                     \
  /** XML attribute writer of Sg##op */                                 \
  void XevSageVisitor::attribSg##op(SgNode* node)                       \
  {                                                                     \
    SgUnaryOp* uop = isSgUnaryOp(node);                                 \
    if(uop){                                                            \
      sstr() << " mode=\""<< uop->get_mode() << "\" ";                  \
    }                                                                   \
    attribSgExpression(sstr(),node);                                    \
  }                                                                     \
  /** XML internal node writer of Sg##op */                             \
  void XevSageVisitor::inodeSg##op(SgNode* node)                        \
  {return;}


VISIT_BOP(AddOp);
VISIT_BOP(AndAssignOp);
VISIT_BOP(AndOp);
//VISIT_BOP(ArrowExp);
VISIT_BOP(AssignOp);
VISIT_BOP(BitAndOp);
VISIT_BOP(BitOrOp);
VISIT_BOP(BitXorOp);
VISIT_BOP(CommaOpExp);
VISIT_BOP(ConcatenationOp);
VISIT_BOP(DivAssignOp);
VISIT_BOP(DivideOp);
VISIT_BOP(EqualityOp);
VISIT_BOP(ExponentiationOp);
VISIT_BOP(GreaterOrEqualOp);
VISIT_BOP(GreaterThanOp);
VISIT_BOP(IorAssignOp);
VISIT_BOP(LessOrEqualOp);
VISIT_BOP(LessThanOp);
VISIT_BOP(LshiftAssignOp);
VISIT_BOP(LshiftOp);
VISIT_BOP(MinusAssignOp);
VISIT_BOP(ModAssignOp);
VISIT_BOP(ModOp);
VISIT_BOP(MultAssignOp);
VISIT_BOP(MultiplyOp);
VISIT_BOP(NotEqualOp);
VISIT_BOP(OrOp);
VISIT_BOP(PlusAssignOp);
//VISIT_BOP(PointerAssignOp);
// ===============================================================================
/// Visitor of a SgPointerAssignOp element in an XML document
SgNode*
XevXmlVisitor::visitSgPointerAssignOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgPointerAssignOp*    ret = 0;
  SgExpression*         lhs = 0;
  SgExpression*         rhs = 0;
  //std::vector< SgExpression * > exprs;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if( lhs==0 )
        lhs = isSgExpression(astchild);
      else if( rhs==0 )
        rhs = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = new SgPointerAssignOp(DEFAULT_FILE_INFO, lhs, rhs, rhs->get_type() );
  ret->set_parent(astParent);
  lhs->set_parent(ret);
  rhs->set_parent(ret);
  return ret;
}
EXPR_DEFAULT(PointerAssignOp);

VISIT_BOP(PntrArrRefExp);
VISIT_BOP(RshiftAssignOp);
VISIT_BOP(RshiftOp);
VISIT_BOP(SubtractOp);
VISIT_BOP(XorAssignOp);

VISIT_UOP(AddressOfOp);
VISIT_UOP(BitComplementOp);
VISIT_UOP_MODE(MinusMinusOp);
VISIT_UOP_MODE(MinusOp);
VISIT_UOP(NotOp);
VISIT_UOP_MODE(PlusPlusOp);
VISIT_UOP(UnaryAddOp);

/** Visitor of a SgUserDefinedBinaryOp element in an XML document */
SgNode*
XevXmlVisitor::visitSgUserDefinedBinaryOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgExpression* lhs = 0;
  SgExpression* rhs = 0;
  xe::DOMNode* child=node->getFirstChild();
  string name,sname;

  if(XmlGetAttributeValue(node,"name",&name) == false){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  //sname = "operator(" + name + ")"; // this is necessary?
  sname = name;
  SgFunctionSymbol* fsym= si::lookupFunctionSymbolInParentScopes(sname,sb::topScopeStack());
  if(fsym==0){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  SgUserDefinedBinaryOp* ret = new SgUserDefinedBinaryOp(lhs,rhs,NULL,name,fsym);
  ret->set_parent(astParent);
  //ret->set_file_info(DEFAULT_FILE_INFO);
  ret->set_startOfConstruct(DEFAULT_FILE_INFO);
  while(child) {
    if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if(lhs==0)
        lhs = isSgExpression(astchild);
      else if(rhs==0)
        rhs = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  }
  if( lhs && rhs ){
    ret->set_lhs_operand(lhs);
    ret->set_rhs_operand(rhs);
    return ret;
  }
  else {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
}
/** XML attribute writer of SgUserDefinedBinaryOp */
void XevSageVisitor::attribSgUserDefinedBinaryOp(SgNode* node)
{
  SgUserDefinedBinaryOp* n =  isSgUserDefinedBinaryOp(node);
  if(n){
    sstr() << " name=\"" << n->get_operator_name().getString() << "\" ";
  }
  attribSgExpression(sstr(),node);
}
/** XML internal node writer of SgUserDefinedBinaryOp */
void XevSageVisitor::inodeSgUserDefinedBinaryOp(SgNode* node)
{return;}


/** Visitor of a SgUserDefinedUnaryOp element in an XML document */
SgNode*
XevXmlVisitor::visitSgUserDefinedUnaryOp(xercesc::DOMNode* node, SgNode* astParent)
{
  SgExpression* lhs = 0;
  xe::DOMNode* child=node->getFirstChild();
  string name, func;

  if(XmlGetAttributeValue(node,"name"    ,&name) == false ){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  SgFunctionSymbol* fsym= si::lookupFunctionSymbolInParentScopes(name);
  if(fsym==0){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  SgUserDefinedUnaryOp* ret = new SgUserDefinedUnaryOp(lhs,lhs->get_type(),name,fsym);
    ret->set_parent(astParent);
    ret->set_file_info(DEFAULT_FILE_INFO);
    while(child) {
      if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){
        SgNode* astchild = this->visit(child,ret);
        if(lhs==0)
          lhs = isSgExpression(astchild);
      }
      child=child->getNextSibling();
    }
    if( lhs ){
      ret->set_operand(lhs);
      return ret;
    }
    else {
      XEV_DEBUG_INFO(node);
      XEV_ABORT();
    }
}
/** XML attribute writer of SgUserDefinedUnaryOp */
void XevSageVisitor::attribSgUserDefinedUnaryOp(SgNode* node)
{
  SgUserDefinedUnaryOp* n =  isSgUserDefinedUnaryOp(node);
  if(n){
    sstr() << " name=\"" << n->get_operator_name().getString() << "\" ";
  }
  attribSgExpression(sstr(),node);
}
/** XML internal node writer of SgUserDefinedUnaryOp */
void XevSageVisitor::inodeSgUserDefinedUnaryOp(SgNode* node)
{return;}
