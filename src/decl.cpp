/**
 * @file     decl.cpp
 * @brief    Visitor Functions of SgDeclarationStatement classes.
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

extern void attribSgStatement(std::ostream& str,SgNode* node);

static void attribSgDeclarationStatement(ostream& istr, SgNode* node)
{
  attribSgStatement(istr,node);
  SgDeclarationStatement* decl = isSgDeclarationStatement(node);
  if(decl==NULL) return;

  SgDeclarationModifier& modifier = decl->get_declarationModifier();
  unsigned long mod = 0;
  SgBitVector vec = modifier.get_modifierVector();
  for(size_t i(0);i<vec.size();i++){
    mod |= (((unsigned int)vec[i]) << i );
  }
  if(mod>1)
    istr << " declaration_modifier=\"" << mod << "\" ";
  mod = 0;
  vec = modifier.get_typeModifier().get_modifierVector();
  for(size_t i(0);i<vec.size();i++){
    mod |= (((unsigned int)vec[i]) << i );
  }
  if(mod>2) // 1:error, 2:default
    istr << " type_modifier=\"" << mod << "\" ";

  if(modifier.get_typeModifier().get_constVolatileModifier().get_modifier()
     != SgConstVolatileModifier::e_default)
    istr << " cv_modifier=\"" <<  modifier.get_typeModifier().get_constVolatileModifier().get_modifier()<< "\" ";
  if(modifier.get_typeModifier().get_gnu_attribute_alignment()>=0)
    istr << " alignment=\"" << modifier.get_typeModifier().get_gnu_attribute_alignment() << "\" ";
  if( si::is_Fortran_language() && TransformationSupport::getModuleStatement(decl)!=NULL )
    // C++ is not considered for now.
    istr << " access_modifier=\"" <<  modifier.get_accessModifier().get_modifier()<< "\" ";
  if(modifier.get_storageModifier().get_modifier() != SgStorageModifier::e_default){
    istr << " storage_modifier=\"" <<  modifier.get_storageModifier().get_modifier()<< "\" ";
  }
  if(modifier.get_storageModifier().get_thread_local_storage() == true ){
    istr << " thread_local=\"1\" ";
  }

  if(decl->get_declarationModifier().get_gnu_attribute_visability()
     > SgDeclarationModifier::e_unspecified_visibility)
    istr << " visibility=\""
	 << decl->get_declarationModifier().get_gnu_attribute_visability()
	 << "\" ";

  if(si::is_Fortran_language()){
    if(decl->get_binding_label().size()){
      istr << " bind=\"" <<decl->get_binding_label() << "\" " << endl;
    }
    if(decl->get_linkage().size()){
      istr << " link=\"" <<decl->get_linkage() << "\" " << endl;
    }
  }
}

static void copySgInitializedName(SgInitializedName* src, SgInitializedName* dst)
{
  if(src != NULL && dst !=NULL){
    /* some attributes of SgInitializedName must be copied excplititly */
    dst->set_gnu_attribute_alignment(src->get_gnu_attribute_alignment());
    dst->set_hasArrayTypeWithEmptyBracketSyntax(src->get_hasArrayTypeWithEmptyBracketSyntax());
    dst->set_prev_decl_item(src->get_prev_decl_item());
    dst->get_storageModifier().set_modifier(src->get_storageModifier().get_modifier());
    dst->set_gnu_attribute_modifierVector(src->get_gnu_attribute_modifierVector());

    if(src->get_attachedPreprocessingInfoPtr()){
      dst->set_attachedPreprocessingInfoPtr
        (src->get_attachedPreprocessingInfoPtr());
    }
  }
}

#define INODE_DECL_DEFAULT(x)                           \
  /** XML internal node writer of Sg##x */              \
  void XevSageVisitor::inodeSg##x(SgNode* node)         \
  { return; }
#define ATTRIB_DECL_DEFAULT(x)                          \
  /** XML attribute writer of Sg##x */                  \
  void XevSageVisitor::attribSg##x(SgNode* node)        \
  { attribSgDeclarationStatement(sstr(),node);          \
    return; }

#define DECL_DEFAULT(x)                         \
  ATTRIB_DECL_DEFAULT(x)                        \
  INODE_DECL_DEFAULT(x)


// =====================================================================
/// Visitor of a SgAsmStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgAsmStmt(xe::DOMNode* node, SgNode* astParent)
{
  SgAsmStmt*     ret = 0;
  SgExpression*  op = 0;
  string                      asm_code;
  string                      regs;
  int                         typ=0;
  int                         gnu=0;


  if(XmlGetAttributeValue(node,"code",&asm_code) == false){
    XEV_MISSING_ATTR(SgAsmStmt,code,true);
  }
  asm_code = XmlEntity2Str( asm_code );
  ret = sb::buildAsmStatement( asm_code );
  //ret->set_isVolatile( typ );
  ret->set_parent(astParent);

  XmlGetAttributeValue(node,"gnu",&gnu);
  ret->set_useGnuExtendedFormat(gnu);

  if(XmlGetAttributeValue(node,"regs",&regs)){
    char* str = (char*)regs.c_str();
    char* c;
    int e;
    SgAsmStmt::AsmRegisterNameList& lst = ret->get_clobberRegisterList();
    c = strtok(str,",");
    if(c) {
      e = atoi(c);
      lst.push_back((SgInitializedName::asm_register_name_enum)e);
    }
    while(c){
      c = strtok(NULL,",");

      if(c) {
        e = atoi(c);
        lst.push_back((SgInitializedName::asm_register_name_enum)e);
      }
    }
  }

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      op = isSgExpression(astchild);
      if(op){
        ret->get_operands().push_back(op);
        op->set_parent(ret);
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
/** XML attribute writer of SgAsmStmt */
void XevSageVisitor::attribSgAsmStmt(SgNode* node)
{
  SgAsmStmt*      n = isSgAsmStmt(node);
  std::string       str;

  if(n) {
    str = n->get_assemblyCode();
    str = XmlStr2Entity( str );
    //sstr() << " volatile=\"" << n->get_isVolatile() << "\"";
    sstr() << " code=\"" << str << "\"";
    sstr() << " gnu=\"" << n->get_useGnuExtendedFormat() << "\"";
    if(n->get_clobberRegisterList().size()>0){
      sstr() << " regs=\"";
      for(size_t i(0);i<n->get_clobberRegisterList().size();i++){
        sstr() << n->get_clobberRegisterList()[i];
        if(i< n->get_clobberRegisterList().size()-1)
          sstr() << ",";
      }
      sstr() << "\"";
    }
  }
  attribSgDeclarationStatement(sstr(),node);
}
INODE_DECL_DEFAULT(AsmStmt);



// =====================================================================
/// Visitor of a SgAttributeSpecificationStatement element in an XML document
//@note not fully implemented yet
SgNode*
XevXmlVisitor::visitSgAttributeSpecificationStatement(xe::DOMNode* node, SgNode* astParent)
{
    SgAttributeSpecificationStatement*  ret = 0;
    SgExpression*                       exp = 0;
    int                                 kind;

    if(XmlGetAttributeValue(node,"kind",&kind)==false){
      XEV_MISSING_ATTR(SgAttributeSpecificationStatement,kind,true);
    }
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

    int intent = 0;
    SgAttributeSpecificationStatement::attribute_spec_enum  ekind
      = (SgAttributeSpecificationStatement::attribute_spec_enum)kind;

    // See sb::buildAttributeSpecificationStatement in sageBuilder_fortran.C
    ret = new SgAttributeSpecificationStatement( DEFAULT_FILE_INFO );
    ret->set_attribute_kind(ekind);
    ret->set_definingDeclaration(ret);
    ret->set_firstNondefiningDeclaration(ret);
    ret->set_parent(astParent);

    switch (kind){
    case SgAttributeSpecificationStatement::e_parameterStatement:
    case SgAttributeSpecificationStatement::e_externalStatement:
    case SgAttributeSpecificationStatement::e_allocatableStatement:
    case SgAttributeSpecificationStatement::e_bindStatement:
      SUBTREE_VISIT_BEGIN(node,astchild,ret)
        {
          exp = isSgExpression(astchild);
          if( exp ) {
#if 0
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
#endif
            lst.push_back(exp);
            //ret->get_parameter_list()->prepend_expression(exp);
          }
        }
      SUBTREE_VISIT_END();

      elst = sb::buildExprListExp( lst );
      elst->set_parent(ret);
      if(ekind != SgAttributeSpecificationStatement::e_bindStatement)
        ret->set_parameter_list( elst );
      else
        ret->set_bind_list( elst );
      break;

    case SgAttributeSpecificationStatement::e_dimensionStatement:
      SUBTREE_VISIT_BEGIN(node,astchild,ret)
        {
          exp = isSgExpression(astchild);
#if 0
          // search SgVariableDeclaration            add (0828)
          if( (aref=isSgPntrArrRefExp(exp)) != 0 ){
            if( (vref=isSgVarRefExp(aref->get_lhs_operand())) != 0 ){
              if( (simb=vref->get_symbol() ) != 0 ){
                inam = simb->get_declaration();
                prnt = inam->get_parent();
                if( prnt == 0 ){
                  // variable with no located declatation stmt is listed in this stmt.
                  lst.push_back(exp);
                }
                else if(isSgFunctionParameterList(prnt)){
                  SgFunctionDefinition * f_def = si::getEnclosingProcedure (sb::topScopeStack());
                  XEV_ASSERT(f_def!=0);
                  inam->set_scope(f_def);
                  inam->set_definition(ret);
                  lst.push_back(exp);
                }
              }
            }
          }
#else
          if( exp && (inam = si::convertRefToInitializedName(exp)) ){
            prnt = inam->get_parent();
            if( prnt == 0 )
              lst.push_back(exp);
            else if(isSgFunctionParameterList(prnt)){
              SgFunctionDefinition * f_def = si::getEnclosingProcedure (sb::topScopeStack());
              XEV_ASSERT(f_def!=0);
              inam->set_scope(f_def);
              inam->set_definition(ret);
              lst.push_back(exp);
            }
          }
#endif
        }
      SUBTREE_VISIT_END();
      elst = sb::buildExprListExp( lst );
      elst->set_parent(ret);
      ret->set_parameter_list( elst );
      break;

    case SgAttributeSpecificationStatement::e_dataStatement :
      SUBTREE_VISIT_BEGIN(node,astchild,ret)
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
    case SgAttributeSpecificationStatement::e_accessStatement_private:
    case SgAttributeSpecificationStatement::e_accessStatement_public:
    case SgAttributeSpecificationStatement::e_intentStatement:
    case SgAttributeSpecificationStatement::e_intrinsicStatement:
    case SgAttributeSpecificationStatement::e_optionalStatement:
    case SgAttributeSpecificationStatement::e_pointerStatement:
    case SgAttributeSpecificationStatement::e_protectedStatement:
    case SgAttributeSpecificationStatement::e_saveStatement :
    case SgAttributeSpecificationStatement::e_targetStatement :
    case SgAttributeSpecificationStatement::e_valueStatement :
    case SgAttributeSpecificationStatement::e_volatileStatement :
      SUBTREE_VISIT_BEGIN(node,astchild,ret)
        {
          str = isSgStringVal(astchild);
          if( str ) {
            slst.push_back(  str->get_value() );
          }
        }
      SUBTREE_VISIT_END();
      ret->get_name_list() = slst;

      if(XmlGetAttributeValue(node,"intent",&intent))
        ret->set_intent(intent);

      if(ekind==SgAttributeSpecificationStatement::e_protectedStatement){
        for(size_t i(0);i<slst.size();i++){
          SgVariableSymbol* vsym
            = si::lookupVariableSymbolInParentScopes( slst[i], sb::topScopeStack() );
          if(vsym){
            SgVariableDeclaration* vdecl = isSgVariableDeclaration(vsym->get_declaration()->get_parent());
            if(vdecl){
              for(size_t i(0);i<vdecl->get_variables().size();i++)
                if( vdecl->get_variables()[i]->get_name().getString()==slst[i])
                  vdecl->get_variables()[i]->set_protected_declaration(true);
            }
          }
          else{
            // is this an error?
            XEV_DEBUG_INFO(node);
            XEV_FATAL("the variable symbol not found");
          }
        }
      }
      break;
    default:
      XEV_DEBUG_INFO(node);
      XEV_FATAL("unknown kind found at SgAttributeSpecificationStatement");
      break;
    }

    return ret;
}
/** XML attribute writer of SgAttributeSpecificationStatement */
void XevSageVisitor::attribSgAttributeSpecificationStatement(SgNode* node)
{
  SgAttributeSpecificationStatement* n = isSgAttributeSpecificationStatement(node);

  if(n) {
    SgAttributeSpecificationStatement::attribute_spec_enum kind =
      n->get_attribute_kind();
    sstr() << " kind=\"" << kind << "\" ";
    //if( n->get_bind_list() )
    //sstr() << " bind_list=\"1\" ";
    if( n->get_intent() )
      sstr() << " intent=\"" << n->get_intent() << "\" ";
  }
  attribSgDeclarationStatement(sstr(),node);
}
/** XML internal node writer of SgAttributeSpecificationStatement */
void XevSageVisitor::inodeSgAttributeSpecificationStatement(SgNode* node)
{
  SgAttributeSpecificationStatement* n = isSgAttributeSpecificationStatement(node);
  if(n) {
    SgExprListExp* lste = 0;
    lste = n->get_parameter_list();
    if( lste){
      SgExpressionPtrList& lst = lste->get_expressions();

      for(size_t i=0;i<lst.size();i++){
        this->visit(lst[i]);
      }
    }
    lste = n->get_bind_list();
    if( lste) {
      SgExpressionPtrList& lst = lste->get_expressions();

      for(size_t i=0;i<lst.size();i++){
        this->visit(lst[i]);
      }
    }

    SgDataStatementGroupPtrList & dlst = n->get_data_statement_group_list();
    for(size_t i=0;i<dlst.size();i++){
      this->visit(dlst[i]);
    }
    SgStringList & slst = n->get_name_list();
    string s;
    for(size_t i=0;i<slst.size();i++){
      s = slst[i];
      SgStringVal *sv = SageBuilder::buildStringVal(s);
      this->visit(sv);
    }
  }
}



// =====================================================================
/// Visitor of a SgClassDeclaration element in an XML document
SgNode*
XevXmlVisitor::visitSgClassDeclaration(xercesc::DOMNode* node, SgNode* astParent)
{
  SgClassDeclaration*   ret=0;
  SgClassDefinition*    exp=0;
  SgClassDeclaration*   decl=0;

  SgScopeStatement* scope = sb::topScopeStack();    //?
  //SgScopeStatement* scope = sb::ScopeStack.front(); // true

  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt=0;
  string                name,val;
  int                   typ=0;
  bool                  unnamed = false;
  int                   auton=1;

  XmlGetAttributeValue(node,"type",&typ);
  XmlGetAttributeValue(node,"name",&name);
  XmlGetAttributeValue(node,"unnamed",&unnamed);
  XmlGetAttributeValue(node,"autonomous",&auton);

  SgClassSymbol* csym = si::lookupClassSymbolInParentScopes(name);
  if(csym==NULL) {
    decl = new SgClassDeclaration( DEFAULT_FILE_INFO, name,
                                   (SgClassDeclaration::class_types)typ);
    decl->set_firstNondefiningDeclaration(decl);
    decl->set_definition(NULL);
    decl->set_definingDeclaration(NULL);
    decl->set_class_type( (SgClassDeclaration::class_types)typ  );
    decl->set_type(SgClassType::createType(decl));
    decl->set_parent(scope);
    decl->set_scope(scope);
    //decl->setForward();
    decl->set_forward(true);
    decl->set_isUnNamed(unnamed);
    //defn->set_type(SgClassType::createType(defn));
    csym = new SgClassSymbol(decl);
    csym->set_declaration(decl);
    scope->insert_symbol(name,csym);
  }
  else {
    decl = csym->get_declaration();
    //nondefn->set_firstNondefiningDeclaration(nondefn);
    //nondefn->set_definition(NULL);
    //nondefn->set_definingDeclaration(NULL);
    //nondefn->set_class_type( (SgClassDeclaration::class_types)typ  );
  }

  ret = new SgClassDeclaration( DEFAULT_FILE_INFO, name,
				(SgClassDeclaration::class_types)typ);
  ret->set_parent(astParent);
  ret->set_scope(scope);
  ret->set_firstNondefiningDeclaration(decl);
  ret->set_class_type( (SgClassDeclaration::class_types)typ);
  ret->set_type(SgClassType::createType(decl));

  ret->set_isUnNamed(unnamed);
  ret->set_isAutonomousDeclaration(auton);

  if(decl->get_definingDeclaration()){
    SgClassDeclaration* cdecl
      = isSgClassDeclaration(decl->get_definingDeclaration());
    // avoid processing the class definition twice
    ret->set_definition(cdecl->get_definition());
    ret->set_definingDeclaration(cdecl);
    ret->setForward();
    ret->set_forward(true);
    return ret;
  }

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( exp==NULL )
        exp = isSgClassDefinition( astchild );
    }
  SUBTREE_VISIT_END();

  if(exp!=NULL){
    // defining declaration
    ret->set_definition( exp );
    ret->set_definingDeclaration(ret);
    decl->set_definingDeclaration(ret);
    exp->set_declaration(ret);
    exp->set_parent(ret);
  }
  else{
    // non-defining declaration
    ret->set_definition(NULL);
    ret->set_definingDeclaration(decl->get_definingDeclaration());
    ret->setForward();
    ret->set_forward(true);
  }
  return ret;
}
/** XML attribute writer of SgClassDeclaration */
void XevSageVisitor::attribSgClassDeclaration(SgNode* node)
{
  SgClassDeclaration*      n = isSgClassDeclaration(node);

  if(n) {
    if(n->get_isAutonomousDeclaration()==false)
      sstr() << " autonomous=\"0\" ";
    if( n->get_isUnNamed())
      sstr() << " unnamed=\"1\" ";
    sstr() << " name=" << n->get_name() << " ";
    sstr() << " type=\"" << n->get_class_type() << "\" ";
  }
  attribSgDeclarationStatement(sstr(),node);
}
INODE_DECL_DEFAULT(ClassDeclaration);


// =====================================================================
/// Visitor of a SgCommonBlock element in an XML document
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
DECL_DEFAULT(CommonBlock);


// =====================================================================
/// Visitor of a SgContainsStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgContainsStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgContainsStatement* ret = new SgContainsStatement(DEFAULT_FILE_INFO);
  //si::setSourcePosition(ret);
  ret->set_definingDeclaration(ret);
  ret->set_parent(astParent);
  return ret;
}
DECL_DEFAULT(ContainsStatement);


// =====================================================================
/// Visitor of a SgDerivedTypeStatement element in an XML document
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

  if(XmlGetAttributeValue(node,"name",&name)==false){
    // name attribute is mandatory
    XEV_MISSING_ATTR(SgDerivedTypeStatement,name,true);
  }
  //XmlGetAttributeValue(node,"type",&typ); // not used

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
      nondefn->set_parent(scope);
      nondefn->set_scope(scope);
      nondefn->set_firstNondefiningDeclaration(nondefn);
      nondefn->setForward();
      nondefn->set_definingDeclaration(ret);
      SgClassSymbol* newsym = new SgClassSymbol(nondefn);
      //nondefn->set_definition(exp);
      newsym->set_declaration(nondefn);

      scope->insert_symbol(name,newsym);
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
/** XML attribute writer of SgDerivedTypeStatement */
void XevSageVisitor::attribSgDerivedTypeStatement(SgNode* node)
{
  attribSgClassDeclaration(node);
}
INODE_DECL_DEFAULT(DerivedTypeStatement);


// =====================================================================
/// Visitor of a SgEntryStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgEntryStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgEntryStatement*       ret = 0;
  SgType*                 typ = 0;
  SgFunctionDefinition*   def = 0;
  SgFunctionParameterList* lst = 0;
  string                  name;
  string                  rname;

  XmlGetAttributeValue(node,"name",&name);

  SUBTREE_VISIT_BEGIN(node,astchild,astParent)
    {
      if( typ==0 )
        typ = isSgType(astchild);
      if( lst==0 )
        lst = isSgFunctionParameterList(astchild);
      if( def==0 )
        def = isSgFunctionDefinition(astchild);
    }
  SUBTREE_VISIT_END();

  SgFunctionType* ftyp = new SgFunctionType(typ,false);
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
/** XML attribute writer of SgEntryStatement */
void XevSageVisitor::attribSgEntryStatement(SgNode* node)
{
  SgEntryStatement* n = isSgEntryStatement(node);
  if(n){
    if(n->get_result_name())
      sstr() << " result=" << n->get_result_name()->get_name() << " ";
  }
  attribSgFunctionDeclaration(node);
}
/** XML internal node writer of SgEntryStatement */
void XevSageVisitor::inodeSgEntryStatement(SgNode* node)
{
  inodeSgProcedureHeaderStatement(node);
}


// =====================================================================
/// Visitor of a SgEnumDeclaration element in an XML document
SgNode*
XevXmlVisitor::visitSgEnumDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgEnumDeclaration*    ret = 0;
  SgScopeStatement*     scope = sb::topScopeStack();    //?
  SgEnumSymbol*         esym = 0;
  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt=0;
  string                name;
  bool unnamed=false;

  if( XmlGetAttributeValue(node,"name",&name)==false )
    XEV_MISSING_ATTR(SgEnumDeclaration,name,true);
  ret = sb::buildEnumDeclaration( name, scope );
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  ret->setForward();
  ret->set_definingDeclaration(NULL);

  esym = si::lookupEnumSymbolInParentScopes(name);
  XEV_ASSERT(esym!=NULL);

  if( XmlGetAttributeValue(node,"unnamed",&unnamed) ){
    esym->get_declaration()->set_isUnNamed(unnamed);
  }
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgInitializedName* ini = isSgInitializedName(astchild);
      if(ini){
        ret->append_enumerator(ini);
        ini->set_parent(ret);
        ret->set_definingDeclaration(ret);
        esym->get_declaration()->set_definingDeclaration(ret);
        //esym->set_declaration(ret);
        ret->unsetForward();
      }
    }
  SUBTREE_VISIT_END();
#if 0
  esym = si::lookupEnumSymbolInParentScopes(name);
  XEV_ASSERT(esym!=NULL);
#endif
  return ret;
}
/** XML attribute writer of SgEnumDeclaration */
void XevSageVisitor::attribSgEnumDeclaration(SgNode* node)
{
  SgEnumDeclaration*      n = isSgEnumDeclaration(node);

  if(n) {
    if( n->get_isUnNamed())
      sstr() << " unnamed=\"1\" ";
    sstr() << " name=" << n->get_name() << " ";
  }
  attribSgDeclarationStatement(sstr(),node);
}
INODE_DECL_DEFAULT(EnumDeclaration);


// =====================================================================
/// Visitor of a SgEquivalenceStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgEquivalenceStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgEquivalenceStatement* ret = new SgEquivalenceStatement(DEFAULT_FILE_INFO);
  SgExprListExp*          lst = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( lst==0 )
        lst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  if( lst==NULL )
    XEV_MISSING_NODE(SgEquivalenceStatement,SgExprListExp,true);

  ret->set_equivalence_set_list(lst);
  ret->set_definingDeclaration(ret);
  lst->set_parent(ret);
  if(lst->get_startOfConstruct()==0)
    lst->set_startOfConstruct(DEFAULT_FILE_INFO);

  return ret;
}
ATTRIB_DECL_DEFAULT(EquivalenceStatement);
/** XML internal node writer of SgEquivalenceStatement */
void XevSageVisitor::inodeSgEquivalenceStatement(SgNode* node)
{
  this->visit(isSgEquivalenceStatement(node)->get_equivalence_set_list());
}



// =====================================================================
/// Visitor of a SgFormatStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgFormatStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgFormatItem*           itm = 0;
  SgFormatItemList*       lst = new SgFormatItemList();
  SgFormatStatement*      ret = 0;
  SUBTREE_VISIT_BEGIN(node,astchild,astParent)
    {
      if((itm = isSgFormatItem(astchild))!=0){
        lst->get_format_item_list().insert(
              //lst->get_format_item_list().begin(), itm );
              lst->get_format_item_list().end(), itm );
      }
    }
  SUBTREE_VISIT_END();

  ret = new SgFormatStatement(DEFAULT_FILE_INFO,lst);
  XEV_ASSERT(ret!=NULL);
  XEV_ASSERT(lst!=NULL);
  lst->set_parent( ret );
  ret->set_parent(astParent);
  return ret;
}
ATTRIB_DECL_DEFAULT(FormatStatement);
/** XML internal node writer of SgFOrmatStatement */
void XevSageVisitor::inodeSgFormatStatement(SgNode* node)
{
  SgFormatItemPtrList & lst =
    isSgFormatStatement(node)->get_format_item_list()->get_format_item_list();
  for(size_t i=0;i<lst.size();i++)
    this->visit(lst[i]);
}



// =====================================================================
/// Visitor of a SgFortranIncludeLine element in an XML document
SgNode*
XevXmlVisitor::visitSgFortranIncludeLine(xe::DOMNode* node, SgNode* astParent)
{
  SgFortranIncludeLine* ret = 0;
  //xe::DOMNamedNodeMap* amap = node->getAttributes();
  //xe::DOMNode* nameatt = 0;
  std::string name;

  if( XmlGetAttributeValue(node,"filename",&name) ==false){
    XEV_MISSING_ATTR(SgFortranIncludeLine,filename,true);
  }

  ret = sb::buildFortranIncludeLine( name );
  XEV_ASSERT(ret!=NULL);
  return ret;
}
/** XML attribute writer of SgFortranIncludeLine */
void XevSageVisitor::attribSgFortranIncludeLine(SgNode* node)
{
  SgFortranIncludeLine* n = isSgFortranIncludeLine(node);
  if(n) {
    //sstr() << " filename=\"" << (strrchr( (char*)n->get_filename().c_str(), '/')+1) << "\" "; // File Name
    sstr() << " filename=\"" << n->get_filename() << "\" "; // File Name
  }
  attribSgStatement(sstr(),node);
}
INODE_DECL_DEFAULT(FortranIncludeLine);


// =====================================================================
/// Visitor of a SgFunctionDeclaration element in an XML document
SgNode*
XevXmlVisitor::visitSgFunctionDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgFunctionDeclaration*   ret = 0;
  SgFunctionParameterList* lst = 0;
  SgFunctionDefinition*    def = 0;
  //SgBasicBlock*            def = 0;
  SgType*                  typ = 0;
  int                      old = 0;
  int                      hasdef=0;
  int                      noparams=0;
  SgInitializedNamePtrList oldlst;

  string name;

  if( XmlGetAttributeValue(node,"name",&name) == false )
    XEV_MISSING_ATTR(SgFunctionDeclaration,name,true);
  XmlGetAttributeValue(node,"old"         ,&old   );
  XmlGetAttributeValue(node,"definition"  ,&hasdef);
  XmlGetAttributeValue(node,"no_params"   ,&noparams);

  FUNCTION_HEADER_VISIT_BEGIN(node,astchild,astParent)
    {
      if(lst==0)
        lst = isSgFunctionParameterList(astchild);
      if(typ==0)
        typ = isSgType(astchild);
    }
  FUNCTION_HEADER_VISIT_END();
  if(typ==NULL){
    XEV_MISSING_NODE(SgFunctionDeclaration,SgType,true);
  }
  if(lst==NULL) {
    XEV_MISSING_NODE(SgFunctionDeclaration,SgFunctionParameterList,true);
  }
  if(hasdef){
    ret
      = sb::buildDefiningFunctionDeclaration(SgName(name.c_str()), typ, lst);
    //def = new SgFunctionDefinition(DEFAULT_FILE_INFO);
    //def->set_parent(ret);
    //ret->set_definition(def);
  }
  else
    ret
      = sb::buildNondefiningFunctionDeclaration(SgName(name.c_str()), typ, lst);

  def = 0; // for visiting SgFunctionDefinition
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  ret->set_prototypeIsWithoutParameters(noparams);
  FUNCTION_BODY_VISIT_BEGIN(node,astchild,ret)
    {
      if(def==0)
        def = isSgFunctionDefinition(astchild);
    }
  FUNCTION_BODY_VISIT_END();

  if(hasdef == 0 ) {
    //Sg_File_Info* info = DEFAULT_FILE_INFO;
    //info->setOutputInCodeGeneration();
    //def = new SgFunctionDefinition(info);
    ret->set_forward(true);
    //ret->get_declarationModifier().get_storageModifier().setExtern();
  }
  if(def && si::is_C_language()){
    ret->set_oldStyleDefinition(old);
    oldlst = ret->get_args();
    for(size_t i(0);i<oldlst.size();i++){
      //cerr << i << " " << oldlst[i]->get_name().getString() << endl;
      SgVariableSymbol* vsym =new SgVariableSymbol(oldlst[i]);
      def->get_symbol_table()->insert(oldlst[i]->get_name(),vsym);
    }
  }

  if(def)
    def->set_parent(ret);
  lst->set_parent(ret);
  ret->set_definition(def);

  int mod=0;
  string str;
  if(XmlGetAttributeValue(node,"gnu_visibility",&mod)){
    ret->set_gnu_extension_visability
      ((SgDeclarationStatement::gnu_extension_visability_attribute_enum)mod);
  }
  if(XmlGetAttributeValue(node,"gnu_alias",&str)){
    ret->get_functionModifier().set_gnu_attribute_named_alias(str);
  }

  return ret;
}
/** XML attribute writer of SgFunctionDeclaration */
void XevSageVisitor::attribSgFunctionDeclaration(SgNode* node)
{
  SgFunctionDeclaration* n = isSgFunctionDeclaration(node);
  if(n) {
    sstr() << " name=" << n->get_name() << " ";
    if(n->get_definition()){
      sstr() << " definition=\"1\" ";
    }
    SgStorageModifier m = (n->get_declarationModifier()).get_storageModifier();
    if(si::is_Fortran_language()){
      sstr() << " end_name=\"" << n->get_named_in_end_statement() << "\" ";
    }
    else {
      if(n->get_oldStyleDefinition())
	sstr() << " old=\"1\" ";
    }
    SgFunctionModifier& f = n->get_functionModifier();
    SgBitVector bit = f.get_modifierVector();
    unsigned long fmod = 0;
    for(size_t i(0);i<bit.size();i++){
      if(bit[i]){
        fmod |= (1<<i);
      }
    }
    if(fmod>2) { //m!= e_unknown && m!= e_default
      sstr() << " function_modifier=\"" << fmod << "\"";
    }
    if(n->get_asm_name().size() > 0 ){
      sstr() << " asm_name=\"" << n->get_asm_name() << "\"";
    }
    if(n->get_prototypeIsWithoutParameters() ){
      sstr() << " no_params=\"1\"";
    }
    /* GNU extensions */
    if(n->get_gnu_regparm_attribute()>0 ){
      sstr() << " regparm=\""
             << n->get_gnu_regparm_attribute()
             << "\"";
    }
    if(n->get_gnu_extension_visability()
       !=  (SgDeclarationStatement::e_gnu_attribute_visability_unspecified))
      sstr() << " gnu_visibility=\"" << n->get_gnu_extension_visability() << "\" ";

    /* SgFunctionDeclaration::get_gnu_extension_alias?? */
    if(f.get_gnu_attribute_named_alias().size() > 0 )
      sstr() << " gnu_alias=\""
             << f.get_gnu_attribute_named_alias() << "\"";
  }
  attribSgDeclarationStatement(sstr(),node);
}
/** XML internal node writer of SgFunctionDeclaration */
void XevSageVisitor::inodeSgFunctionDeclaration(SgNode* node)
{
  SgFunctionDeclaration* n = isSgFunctionDeclaration(node);
  if(n){
    SgType* t = isSgFunctionDeclaration(node)->get_type()->get_return_type();
    this->visit(t);
  }
}


// =====================================================================
/// Visitor of a SgFunctionParameterList element in an XML document
SgNode*
XevXmlVisitor::visitSgFunctionParameterList(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFunctionParameterList* ret = sb::buildFunctionParameterList();
  SgInitializedName* ini=0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if((ini = isSgInitializedName(astchild)) != NULL ){
        si::appendArg(ret,ini);
        ini->set_scope(sb::topScopeStack());
        ini->set_parent(ret);
      }
    }
  SUBTREE_VISIT_END();

  ret->set_definingDeclaration(NULL);
  ret->set_firstNondefiningDeclaration(ret);
  return ret;
}
DECL_DEFAULT(FunctionParameterList);



// =====================================================================
/// Visitor of a SgImplicitStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgImplicitStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgImplicitStatement*      ret   = new SgImplicitStatement(DEFAULT_FILE_INFO, true );
  SgInitializedName*        inam  = 0;
  SgInitializedNamePtrList  lst;

  XEV_ASSERT(ret!=NULL);
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
DECL_DEFAULT(ImplicitStatement);

// =====================================================================
/// Visitor of a SgImportStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgImportStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgImportStatement*        ret   = new SgImportStatement(DEFAULT_FILE_INFO);
  SgExpressionPtrList       lst;
  SgExpression*             exp = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  ret->set_definingDeclaration(ret);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      exp = isSgExpression(astchild);
      if(exp)
        lst.push_back(exp);
    }
  SUBTREE_VISIT_END();

  if(!lst.empty()) {
    ret->get_import_list() = lst;
    for(size_t i(0);i<lst.size();i++)
      lst[i]->set_parent(ret);
  }

  return ret;
}
DECL_DEFAULT(ImportStatement);



// =====================================================================
/// Visitor of a SgInterfaceStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgInterfaceStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgInterfaceStatement*      ret=0;
  SgInterfaceBody*           bdy=0;

  string                name;
  int                   typ=0;

  XmlGetAttributeValue(node,"name",&name);
  XmlGetAttributeValue(node,"type",&typ);

  ret = new SgInterfaceStatement(DEFAULT_FILE_INFO,
                 SgName( name.c_str() ),(SgInterfaceStatement::generic_spec_enum)typ );

  XEV_ASSERT(ret!=NULL);
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
/** XML attribute writer of SgInterfaceStatement */
void XevSageVisitor::attribSgInterfaceStatement(SgNode* node)
{
  SgInterfaceStatement* n = isSgInterfaceStatement(node);

  if(n) {
    sstr() << " name=" << n->get_name() << " ";
    sstr() << " type=\"" << n->get_generic_spec() << "\" ";
  }
  attribSgStatement(sstr(),node);
}
INODE_DECL_DEFAULT(InterfaceStatement);



// =====================================================================
/// Visitor of a SgModuleStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgModuleStatement(xe::DOMNode* node, SgNode* astParent)
{
  string                name;
  int                   typ=0;
  SgClassDefinition*    exp=0;
  SgModuleStatement*    non=0;
  XmlGetAttributeValue(node,"name",&name);
  XmlGetAttributeValue(node,"type",&typ);

  SgModuleStatement* ret = buildModuleStatementAndDefinition(
                SgName( name.c_str() ), sb::topScopeStack());
  Sg_File_Info* info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  XEV_ASSERT(info!=NULL);
  XEV_ASSERT(ret!=NULL);

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
  else {
    XEV_DEBUG_INFO(node);
    XEV_INFO("the same module name is found in a symbol table");
    XEV_FATAL("not implemented yet");
    //XEV_DEBUG_INFO(node);
    //XEV_ABORT(); //TODO
  }
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
  else {
    XEV_DEBUG_INFO(node);
    XEV_FATAL("nondefining module declaration is not implemented yet");
    //XEV_DEBUG_INFO(node);
    //XEV_ABORT();
  }
  return ret;
}
/** XML attribute writer of SgModuleStatement */
void XevSageVisitor::attribSgModuleStatement(SgNode* node)
{
  attribSgClassDeclaration(node);
}
INODE_DECL_DEFAULT(ModuleStatement);




// =====================================================================
/// Visitor of a SgNamelistStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgNamelistStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgNamelistStatement*    ret = new SgNamelistStatement(DEFAULT_FILE_INFO);
  //SgNameGroup*            grp = new SgNameGroup();
  SgNameGroup*            grp = 0;
  //SgNameGroupPtrList      glst;
  SgStringVal*            str = 0;

  //string                  name;
  SgNamelistStatement* nondefn = new SgNamelistStatement(DEFAULT_FILE_INFO);
  //XmlGetAttributeValue(node,"group",&name);

  //Rose_STL_Container<std::string> slst;
  XEV_ASSERT(ret!=NULL);
  XEV_ASSERT(nondefn!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      grp = isSgNameGroup(astchild);
      if( grp ) {
        grp->set_parent(NULL);//parent must be set to NULL.
        ret->get_group_list().push_back( grp );
      }
    }
  SUBTREE_VISIT_END();

  //grp->set_group_name( name );
  //grp->get_name_list() = slst;
  //grp->set_parent(NULL);
  //ret->get_group_list().push_back( grp );

  ret->set_firstNondefiningDeclaration(nondefn);
  ret->set_definingDeclaration(ret);
  nondefn->set_parent(sb::topScopeStack());
  nondefn->set_firstNondefiningDeclaration(nondefn);
  nondefn->set_definingDeclaration(ret);
  return ret;
}
/** XML attribute writer of SgNamelistStatement */
void XevSageVisitor::attribSgNamelistStatement(SgNode* node)
{
#if 0
  SgNamelistStatement*    nlst = isSgNamelistStatement(node);

  if( nlst ) {
    SgNameGroupPtrList & grp = nlst->get_group_list();
    SgNameGroup* nam = isSgNameGroup(grp[0]);
    sstr() << " group=\"" << nam->get_group_name() << "\" ";
  }
#endif
  attribSgStatement(sstr(),node);
}
/** XML internal node writer of SgNamelistStatement */
void XevSageVisitor::inodeSgNamelistStatement(SgNode* node)
{
  SgNameGroupPtrList & grp = isSgNamelistStatement(node)->get_group_list();
  for(size_t i=0;i<grp.size();i++) {
    SgNameGroup* nam = isSgNameGroup(grp[i]);
    /*
    SgStringList & nl = nam->get_name_list();
    string s;
    for(size_t j=0;j<nl.size();j++) {
      s = nl[j];
      //printf( "i=%d (%s)\n", j,s.c_str());
      SgStringVal *sv = SageBuilder::buildStringVal(s);
      this->visit(sv);
    }
    */
     if(nam) this->visit(nam);
   }
 }



 // =====================================================================
 /// Visitor of a SgPragmaDeclaration element in an XML document
 SgNode*
 XevXmlVisitor::visitSgPragmaDeclaration(xe::DOMNode* node, SgNode* astParent)
 {
   SgPragmaDeclaration* ret = 0;
   SgPragma* pr = 0;

   SUBTREE_VISIT_BEGIN(node,child,astParent)
     {
       if(pr==0)
         pr = isSgPragma(child);
     }
   SUBTREE_VISIT_END();

   if(pr==NULL)
     XEV_MISSING_NODE(SgPragmaDeclaration,SgPragma,true);
   ret = sb::buildPragmaDeclaration(pr->get_pragma());
   XEV_ASSERT(ret!=NULL);
   ret->set_parent(astParent);

   pr->set_parent(ret);
   return ret;
 }
 DECL_DEFAULT(PragmaDeclaration);



 // =====================================================================
 /// Visitor of a SgProcedureHeaderStatement element in an XML document
 SgNode*
 XevXmlVisitor::visitSgProcedureHeaderStatement(xe::DOMNode* node, SgNode* astParent)
 {
   SgProcedureHeaderStatement*     ret = 0;
   //SgFunctionType*                 typ = 0;
   SgType*                         typ = 0;
   SgFunctionParameterList*        lst = 0;
   SgFunctionParameterList*        lst2 = 0;
   SgScopeStatement*               scope = 0;
   SgBasicBlock*                   def = 0;
   SgFunctionDefinition*           fdf = 0;
   SgInitializedName*              var = 0;

   string                          name,rname;
   int                             kind=0;
   bool                            f_pure  = false;
   bool                            f_elem  = false;
   bool                            f_recur = false;
   bool                            f_old   = false;
   Sg_File_Info* info = DEFAULT_FILE_INFO;
   info->setOutputInCodeGeneration();

   XmlGetAttributeValue(node,"name",&name);
   XmlGetAttributeValue(node,"kind",&kind);
   XmlGetAttributeValue(node,"recursive",&f_recur);
   XmlGetAttributeValue(node,"pure",&f_pure);
   XmlGetAttributeValue(node,"elemental",&f_elem);
   XmlGetAttributeValue(node,"old",&f_old);

   // the scope must be SgGlobal of this SgProject.
   //scope = isSgScopeStatement(_file->get_globalScope());
   scope = sb::topScopeStack();
   while(scope && scope->containsOnlyDeclarations()==false){
     //cerr << scope->class_name() <<"->";;
     scope = isSgStatement(scope->get_parent())->get_scope();
     //cerr << scope->class_name() << endl;
   }

   // -----------------------------------------------------------------------
   // function header
   FUNCTION_HEADER_VISIT_BEGIN(node,astchild,astParent);
   {
     if(typ==0)
       typ = isSgType(astchild);
     //typ = isSgFunctionType(astchild);
     if(lst==0)
       lst = isSgFunctionParameterList(astchild);
     if(var==0)
       var = isSgInitializedName(astchild);
   }
   FUNCTION_HEADER_VISIT_END();

   if(lst==NULL)
     XEV_MISSING_NODE(SgProcedureHeaderStatement,SgParameterList,true);
   if(typ==NULL)
     XEV_MISSING_NODE(SgProcedureHeaderStatement,SgType,true);

   if( kind != SgProcedureHeaderStatement::e_block_data_subprogram_kind ){
     //lst2 = si::deepCopy(lst);
     lst2 = lst;
     ret = sb::buildProcedureHeaderStatement( (const char*)(name.c_str()), typ, lst2,
                                              (SgProcedureHeaderStatement::subprogram_kind_enum)kind, scope);
     XEV_ASSERT(ret!=NULL);
     //SgNode::get_globalFunctionTypeTable()->print_functypetable(std::cerr);
     //std::cerr << "--------------------" << ret->get_type() -> get_return_type() -> class_name() << std::endl;
     lst2->set_parent(ret);
   }
   else {
       // add (block data) 0828
     SgFunctionType* functionType = new SgFunctionType(SgTypeVoid::createType(), false);
     ret = new SgProcedureHeaderStatement( info,SgName(name), functionType,  fdf/*=NULL*/ );
     XEV_ASSERT(ret!=NULL);
     ret->set_type(functionType);
     ret->set_subprogram_kind( (SgProcedureHeaderStatement::subprogram_kind_enum)kind );
   }
   XEV_ASSERT(ret!=NULL);
   ret->set_scope(scope);
   ret->set_parent(astParent);
   ret->set_oldStyleDefinition(f_old);
   lst->set_parent(ret);

   if(var){
     var->set_parent(ret);
     var->set_scope(scope); // is this OK??
     //var->set_scope(sb::topScopeStack());
   }

   // -----------------------------------------------------------------------
   // "*" in parameter list is considered an alternative return
   if(lst2 && si::is_Fortran_language()==true){
     int counter=1;
     SgScopeStatement* scope = ret->get_definition();
     for(size_t i(0);i<lst2->get_args().size();i++){
       SgInitializedName* ini = lst2->get_args()[i];
       // for each parameter, find its symbol in the function definition scope
       SgSymbol* sym = scope->lookup_symbol(ini->get_name());

       if(sym!=NULL){
         //ini->set_scope(fdf); // this was needed but now unnecessary
         ini->set_type(sym->get_type());

         if(ini->get_name().getString() =="*"){
           // replace a variable symbol to a label symbol
           scope->get_symbol_table()->remove(sym);
           delete sym;
           SgInitializedName* ini2 = isSgInitializedName(si::deepCopyNode(ini));
           SgLabelSymbol* lsym = new SgLabelSymbol();
           lsym->set_fortran_alternate_return_parameter(ini);
           lsym->set_numeric_label_value(counter);
           scope->insert_symbol(ini2->get_name(),lsym);
           counter++;
         }

         if(isSgVariableSymbol(sym))
           ini->set_declptr(isSgVariableSymbol(sym)->get_declaration()->get_declptr());
       }
       else{
         XEV_DEBUG_INFO(node);
         XEV_FATAL("unexpected error"); // never reach here
       }
     }
   } // if lst2

   // -----------------------------------------------------------------------
   // function body
   ret->set_parent(astParent);
   FUNCTION_BODY_VISIT_BEGIN(node,astchild,ret);
   {
     if(fdf==0)
       fdf = isSgFunctionDefinition(astchild);
     // for block data
     if(kind == SgProcedureHeaderStatement::e_block_data_subprogram_kind && def==0)
       def = isSgBasicBlock(astchild);
   }
   FUNCTION_BODY_VISIT_END();

   if( fdf==0&&kind == SgProcedureHeaderStatement::e_block_data_subprogram_kind ){
     fdf = new SgFunctionDefinition( info,def );
     if(def)
       def->set_parent(fdf);
     fdf->set_parent(ret);
   }

   if(fdf==NULL)
     XEV_MISSING_NODE(SgProcedureHeaderStatement,SgFunctionDefinition,true);

   //si::replaceStatement( ret->get_definition()->get_body(),def,true );
   ret->set_definition(fdf);
   fdf->set_declaration(ret);
   fdf->set_parent(ret);
   VardefSearch search(name);
   SgInitializedName* ini = isSgInitializedName(search.visit(fdf));
   if(ini){
     ini->set_declptr(ret);
     if(ret->isFunction() == false || ret->get_oldStyleDefinition()){
       // This will remove type description before the function name
       ret->get_type()->set_return_type(SgTypeVoid::createType());
     }
   }
   var->set_scope(fdf);

   if( isSgClassType(typ) ){
     SgClassType* ctyp = isSgClassType(typ);
     SgClassSymbol* csym
       = si::lookupClassSymbolInParentScopes(ctyp->get_name(),fdf->get_body());
     //body->get_symbol_table()->print();
     if(csym!=NULL){
       ctyp->set_declaration(csym->get_declaration());
     }
   }

   if(f_pure)
     ret->get_functionModifier().setPure();
   if(f_elem)
     ret->get_functionModifier().setElemental();
   if(f_recur)
     ret->get_functionModifier().setRecursive();


   return ret;
 }
 /** XML attribute writer of SgProcedureHeaderStatement */
 void XevSageVisitor::attribSgProcedureHeaderStatement(SgNode* node)
 {
   SgProcedureHeaderStatement* n = isSgProcedureHeaderStatement(node);
   if(n) {
     sstr() << " kind=\"" << n->get_subprogram_kind () << "\" ";
     if(n->get_result_name()->get_name() != n->get_name())
       sstr() << " result=\"" << n->get_result_name()->get_name().getString() << "\" ";
     if(n->get_functionModifier().isPure())
       sstr() << " pure=\"1\"";
     if(n->get_functionModifier().isElemental())
       sstr() << " elemental=\"1\"";
     if(n->get_functionModifier().isRecursive())
       sstr() << " recursive=\"1\"";
   }
   attribSgFunctionDeclaration(node);
 }
 /** XML internal node writer of SgProcedureHeaderStatement */
 void XevSageVisitor::inodeSgProcedureHeaderStatement(SgNode* node)
 {
   inodeSgFunctionDeclaration(node);
 }



 // =====================================================================
 /// Visitor of a SgProgramHeaderStatement element in an XML document
 SgNode*
 XevXmlVisitor::visitSgProgramHeaderStatement(xe::DOMNode* node, SgNode* astParent)
 {
   SgType*               typ = 0;
   SgFunctionDefinition* fdf = 0;
   SgBasicBlock*         def = 0;
   SgFunctionParameterList*      lst = 0;

   string              name;
   int elabel = 0;

   if(XmlGetAttributeValue(node,"name",&name)==false){
     XEV_MISSING_ATTR(SgProgramHeaderStatement,name,true);
   }
   SgFunctionType* ftyp = new SgFunctionType(SgTypeVoid::createType(), false);
   XEV_ASSERT(ftyp!=NULL);

   SgProgramHeaderStatement* ret
     = new SgProgramHeaderStatement(astParent->get_file_info(),SgName(name), ftyp, NULL);
   XEV_ASSERT(ret!=NULL);
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
   XEV_ASSERT(fdf!=NULL);
   fdf->set_parent(ret);

   Sg_File_Info* info = DEFAULT_FILE_INFO;
   info->setOutputInCodeGeneration();
   fdf->set_file_info(info);

   ret->set_name(SgName(name.c_str()));
   ret->set_definition(fdf);
   // there must be a bug if these assertions are failed
   XEV_ASSERT(fdf->get_file_info()!=NULL);
   XEV_ASSERT(def==NULL || def->get_file_info()!=NULL);

   if(XmlGetAttributeValue(node,"elabel",&elabel)){
     SgLabelSymbol*  s = new SgLabelSymbol();
     s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
     s->get_fortran_statement()->set_parent(s);
     s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
     s->set_numeric_label_value( elabel );
     SgLabelRefExp*  l = new SgLabelRefExp( s );
     ret->set_end_numeric_label( l );
   }
   return ret;
 }
 /** XML attribute writer of SgProgramHeaderStatement */
 void XevSageVisitor::attribSgProgramHeaderStatement(SgNode* node)
 {
   SgProgramHeaderStatement* p = isSgProgramHeaderStatement(node);
   if(p){
     SgLabelRefExp* l = p->get_end_numeric_label();
     if(l)
       sstr() << " elabel=\"" << l->get_numeric_label_value() << "\" ";
   }
   attribSgFunctionDeclaration(node);
 }
 /** XML internal node writer of SgProgramHeaderStatement */
 void XevSageVisitor::inodeSgProgramHeaderStatement(SgNode* node)
 {
   inodeSgFunctionDeclaration(node);
 }



 // =====================================================================
 /// Visitor of a SgTypedefDeclaration element in an XML document
 SgNode*
 XevXmlVisitor::visitSgTypedefDeclaration(xe::DOMNode* node, SgNode* astParent)
 {
   SgTypedefDeclaration*     ret  = 0;
   SgDeclarationStatement*   decl = 0;
   SgType*                   typ  = 0;
   int                       lst  = 0;
   //xe::DOMNamedNodeMap* amap = node->getAttributes();
   //xe::DOMNode* satt = 0;
   string name;
   bool first = false;
   SgTypedefSymbol* tsym = 0;

   if(XmlGetAttributeValue(node,"name",&name)==false){
     XEV_MISSING_ATTR(SgTypedefDeclaration,name,true);
   }
   XmlGetAttributeValue(node,"list",&lst);

   tsym = si::lookupTypedefSymbolInParentScopes(name);
   if(lst && tsym==0)
     first = true;

   SUBTREE_VISIT_BEGIN(node,astchild,astParent)
     {
       if(typ==0)
         typ = isSgType(astchild);
       if(decl==0)
         decl = isSgDeclarationStatement(astchild);
     }
   SUBTREE_VISIT_END();

   if(typ==NULL){
     XEV_MISSING_NODE(TypedefDeclaration,SgType,true);
   }
   ret = sb::buildTypedefDeclaration(name, typ);
   XEV_ASSERT(ret!=NULL);
   ret->set_parent(astParent);

   if(decl) {
     tsym = si::lookupTypedefSymbolInParentScopes(name);
     XEV_ASSERT(tsym!=NULL);
     ret->set_declaration(tsym->get_declaration());
     ret->set_definingDeclaration(ret);
     ret->set_requiresGlobalNameQualificationOnType(true);
     if(si::is_Fortran_language()==false)
       ret->set_typedefBaseTypeContainsDefiningDeclaration(true);
     decl->set_parent(ret);
     tsym->get_declaration()->set_definingDeclaration(ret);
   }

   // If one typedef stmt declares multiple names, it is split into
   // multiple stmts. In such a case, the class cannot be unnamed.
   if(lst){
     ret->set_isAssociatedWithDeclarationList(true);
     SgClassDeclaration* cdecl = isSgClassDeclaration(decl);
     if(cdecl) {
       cdecl->set_isUnNamed(false);
       isSgClassDeclaration(cdecl->get_firstNondefiningDeclaration())
         ->set_isUnNamed(false);
     }
   }
   return ret;
 }
 /** XML attribute writer of SgTypedefDeclaration */
 void XevSageVisitor::attribSgTypedefDeclaration(SgNode* node)
 {
   SgTypedefDeclaration*      n = isSgTypedefDeclaration(node);

   if(n) {
     sstr() << " name=" << n->get_name() << " ";
     if(n->get_isAssociatedWithDeclarationList())
       sstr() << " list=\"1\" ";
   }
   attribSgDeclarationStatement(sstr(),node);
 }
 /** XML internal node writer of SgTypedefDeclaration */
 void XevSageVisitor::inodeSgTypedefDeclaration(SgNode* node)
 {
   this->visit(isSgTypedefDeclaration(node)->get_base_type());
 }


 // =====================================================================
 /// Visitor of a SgUseStatement element in an XML document
 SgNode*
 XevXmlVisitor::visitSgUseStatement(xe::DOMNode* node, SgNode* astParent)
 {
   SgUseStatement* ret = 0;
   string name;
   int only=0;
   SgRenamePair* rn = 0;

   XEV_ASSERT(astParent!=0);
   if(XmlGetAttributeValue(node,"name",&name)==false)
     XEV_MISSING_ATTR(SgUseStatement,name,true);
   if(XmlGetAttributeValue(node,"only",&only)==false)
     XEV_MISSING_ATTR(SgUseStatement,only,true);

   ret = new SgUseStatement(DEFAULT_FILE_INFO,name,only);
   XEV_ASSERT(ret!=0);
   ret->set_parent(astParent);
   SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if((rn=isSgRenamePair(astchild))!=0){
        ret->get_rename_list().push_back(rn);
      }
    }
  SUBTREE_VISIT_END();

  // SgModuleSymbol is not used??
  SgClassSymbol* csym = si::lookupClassSymbolInParentScopes(name);
  if(csym) {
    //XEV_WARN("found");
    // TODO: we have to fully support R1109 in SageBuiler.C (but now only partially).
    SgClassDeclaration* decl = csym->get_declaration();
    SgModuleStatement* mstmt = isSgModuleStatement(decl);
    if(mstmt==0) XEV_FATAL("SgUseStatement is associated with an invalid symbol");
    ret->set_module(mstmt);
    SgClassDefinition* defn = isSgClassDeclaration(decl->get_definingDeclaration())->get_definition();
    if(defn==0)  XEV_FATAL("SgUseStatement is associated with an undefined symbol");
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
/** XML attribute writer of SgUseStatement */
void XevSageVisitor::attribSgUseStatement(SgNode* node)
{
  SgUseStatement* n = isSgUseStatement(node);
  if(n) {
    sstr() << " name=" << n->get_name() << " ";
    sstr() << " only=\"" << n->get_only_option() << "\"";
  }
  attribSgStatement(sstr(),node);
}
INODE_DECL_DEFAULT(UseStatement);


// =====================================================================
/// Visitor of a SgVariableDeclaration element in an XML document
SgNode*
XevXmlVisitor::visitSgVariableDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgVariableDeclaration*                    ret  = 0;
  SgInitializedName*                        name = 0;
  SgInitializedName*                        tmp  = 0;
  SgDeclarationStatement*                   cls  = 0;
  SgExpression*                             bit  = 0;
  Rose_STL_Container<SgInitializedName*>    varList;
  //string            bitstr;

  //XmlGetAttributeValue(node,"bitfield",&bitstr);

  SUBTREE_VISIT_BEGIN(node,astchild,astParent)
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

      if(bit==0){
	bit = isSgExpression(astchild);
      }
    }
  SUBTREE_VISIT_END();


  if(name==NULL) {
    XEV_MISSING_NODE(SgVariableDeclaration,SgInitializedName,true);
  }

  SgVariableSymbol* vsym = si::lookupVariableSymbolInParentScopes(name->get_name());
#if 0
  if(vsym){
    cerr << "found " << name->get_name().getString() << "@ visitSgVariableDeclaration" << endl;
    cerr << vsym->get_type()->class_name() << endl;
  }
  else
    cerr << "NOT found " << name->get_name().getString() << "@ visitSgVariableDeclaration" << endl;
#endif
  // defining declaration
  if( cls ) {
    SgType * typ = name->get_type();
    SgNamedType *namedType = isSgNamedType(typ->findBaseType());

    //cls->set_parent( typ );
    //namedType->set_declaration (cls);
    //namedType->get_declaration()->set_definingDeclaration(cls);
    ret = sb::buildVariableDeclaration(name->get_name(),
                                       typ,
                                       name->get_initializer());

    XEV_ASSERT(ret!=NULL);
    ret->set_baseTypeDefiningDeclaration( cls );
    cls->set_parent(ret);
  }
  // non-defining declaration
  else if(name->get_name().is_null()==false){
    if(vsym==NULL ||  si::is_Fortran_language()==false ) {
      ret = sb::buildVariableDeclaration(name->get_name(),
                                         name->get_type(),
                                         name->get_initializer());
      XEV_ASSERT(ret!=NULL);
    }
    else {
      // this variable would already be defined in SgFunctionParameterList
      // calling buildVariableDeclaration craches so it is not called below.
      ret = new SgVariableDeclaration(DEFAULT_FILE_INFO);
      XEV_ASSERT(ret!=NULL);
      if(vsym->get_declaration()){
	if(isSgVariableDeclaration(vsym->get_declaration()->get_definition()))
	  ret->set_definingDeclaration(vsym->get_declaration()->get_definition());
	else{
	  ret->set_definingDeclaration(ret);
	  vsym->get_declaration()->set_definition(ret);
	}
      }
      if(vsym && isSgFunctionParameterList(vsym->get_declaration()->get_parent())){
        vsym->get_declaration()->set_parent(ret);
      }
    }
  }
  else{
    // probably, an unnamed structure or union member
    SgVariableDefinition* def = new SgVariableDefinition(DEFAULT_FILE_INFO);
    ret = new SgVariableDeclaration(DEFAULT_FILE_INFO);
    XEV_ASSERT(def!=NULL);
    XEV_ASSERT(ret!=NULL);

    name->set_declptr(def);
    name->set_definition(def);
    ret->append_variable(name,name->get_initializer());
    def->set_parent(ret);
  }

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  ret->set_definingDeclaration(ret);
  SgInitializedName* decl_item = ret->get_decl_item(name->get_name());
  //  XEV_ASSERT(decl_item!=NULL);
  copySgInitializedName(name,decl_item);
  //if(name->get_name().is_null()==false)
  //name->set_definition(ret->get_definition());
  // see buildVariableDeclaration in fortran_support.C
  if(si::is_Fortran_language())
    ret->set_variableDeclarationContainsBaseTypeDefiningDeclaration(false);

  if( varList.size() > 0 && si::is_Fortran_language() )
    // NOTE: in the case of Fortran, append_statement does not work?
    // So this is necessary to define the second variable and later.
    ret->get_variables() = varList;
  for(size_t i(0);i<varList.size();i++){
    if(i>0 && si::is_Fortran_language() == false )
      ret->append_variable(varList[i],varList[i]->get_initializer());
    varList[i]->set_parent(ret);
    //varList[i]->set_declptr(ret->get_definition());
    if(varList[i]->get_name().is_null()==false){
      varList[i]->set_declptr(ret);
      varList[i]->set_scope(name->get_scope());
    }
    //varList[i]->set_type(name->get_type());
  }

  // this is necessary because declaration is required for each variable.
  for(size_t i(1);i<varList.size();i++){
    vsym = si::lookupVariableSymbolInParentScopes(varList[i]->get_name());
    if(vsym==NULL && varList[i]->get_name().is_null() == false){
      SgVariableDeclaration* decl =sb::buildVariableDeclaration(varList[i]->get_name(),
                                                                varList[i]->get_type(),
                                                                varList[i]->get_initializer());
      XEV_ASSERT(decl!=NULL);
      //varList[i]->set_definition(decl->get_definition());
      varList[i]->set_declptr(decl);
      decl->append_variable(varList[i],varList[i]->get_initializer());
    }
  }


  // set bitfield (2015.07.18)
  if(bit && si::is_Fortran_language() == false){
    ret->set_bitfield (isSgValueExp(bit));
    bit->set_parent(ret);
  }

  // check the result variable
  if( si::is_Fortran_language() ){
    SgProcedureHeaderStatement* prc
      = isSgProcedureHeaderStatement(si::getEnclosingFunctionDeclaration(ret));
    if(prc) {
      for(size_t i(0);i<varList.size();i++){
        if(prc->get_name() == varList[i]->get_name()){
          prc->set_result_name(varList[i]);
          prc->get_result_name()->set_definition(varList[i]->get_definition());
          break;
        }
      }
      // TODO: this will never be true because "result" is not set yet.
      if(prc->get_result_name() && prc->get_result_name()->get_definition()==NULL ){
        for(size_t i(0);i<varList.size();i++){
          if(prc->get_result_name()->get_name() == varList[i]->get_name().getString()){
            prc->get_result_name()->set_definition(varList[i]->get_definition());
            break;
          }
        }
      }
    }
  }
  int mod=0;
  string str;
  int adecl=1;

  if(XmlGetAttributeValue(node,"gnu_visibility",&mod)){
    ret->set_gnu_extension_visability
      ((SgDeclarationStatement::gnu_extension_visability_attribute_enum)mod);
  }
  if(XmlGetAttributeValue(node,"gnu_declaration",&mod)){
    ret->set_gnu_extension_declaration_attribute
      ((SgVariableDeclaration::gnu_extension_declaration_attributes_enum)mod);
  }
  if(XmlGetAttributeValue(node,"gnu_section",&str)){
    ret->set_gnu_extension_section(str);
  }

  if(XmlGetAttributeValue(node,"gnu_alias",&str)){
    ret->set_gnu_extension_alias(str);
  }
  return ret;
}
/** XML attribute writer of SgVariableDeclaration */
void XevSageVisitor::attribSgVariableDeclaration(SgNode* node)
{
  SgVariableDeclaration* n = isSgVariableDeclaration(node);
  if(n){
    if(n->get_gnu_extension_visability()
       !=  (SgDeclarationStatement::e_gnu_attribute_visability_unspecified))
      sstr() << " gnu_visibility=\"" << n->get_gnu_extension_visability() << "\" ";
    if(n->get_gnu_extension_declaration_attribute()
       !=  (SgVariableDeclaration::e_gnu_attribute_unspecified))
      sstr() << " gnu_declaration=\"" << n->get_gnu_extension_declaration_attribute() << "\" ";

    if(n->get_gnu_extension_section().size()>0)
      sstr() << " gnu_section=\"" << n->get_gnu_extension_section() << "\" ";
    if(n->get_gnu_extension_alias().size()>0)
      sstr() << " gnu_alias=\"" << n->get_gnu_extension_alias() << "\" ";

  }
  attribSgDeclarationStatement(sstr(),node);
}
//INODE_DECL_DEFAULT(VariableDeclaration);
void XevSageVisitor::inodeSgVariableDeclaration(SgNode* node)
{
  SgVariableDeclaration* n = isSgVariableDeclaration(node);
  if( si::is_Fortran_language() == false && n && n->get_bitfield() ) {
    this->visit(n->get_bitfield());
  }
}
