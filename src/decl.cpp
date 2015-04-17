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
  istr << " declaration_modifier=\"" << mod << "\" ";
  mod = 0;
  vec = modifier.get_typeModifier().get_modifierVector();
  for(size_t i(0);i<vec.size();i++){
    mod |= (((unsigned int)vec[i]) << i );
  }
  istr << " type_modifier=\"" << mod << "\" ";

  if(modifier.get_typeModifier().get_constVolatileModifier().get_modifier()
     != SgConstVolatileModifier::e_default)
    istr << " cv_modifier=\"" <<  modifier.get_typeModifier().get_constVolatileModifier().get_modifier()<< "\" ";
  if( SageInterface::is_C_language() == false )
    istr << " access_modifier=\"" <<  modifier.get_accessModifier().get_modifier()<< "\" ";
  if(modifier.get_storageModifier().get_modifier() != SgStorageModifier::e_default){
    istr << " storage_modifier=\"" <<  modifier.get_storageModifier().get_modifier()<< "\" ";
  }

  if(si::is_Fortran_language()){
    if(decl->get_binding_label().size()){
      istr << " bind=\"" <<decl->get_binding_label() << "\" " << endl;
    }
    if(decl->get_linkage().size()){
      istr << " link=\"" <<decl->get_linkage() << "\" " << endl;
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
  string                      asm_code;
  string                      vol;
  int                         typ=0;

  if(XmlGetAttributeValue(node,"code",&asm_code) == false
     ||   XmlGetAttributeValue(node,"volatile",&typ) == false ){
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }

  ret = sb::buildAsmStatement( asm_code );
  ret->set_isVolatile( typ );
  ret->set_parent(astParent);

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
    sstr() << " volatile=\"" << n->get_isVolatile() << "\"";
    sstr() << " code=\"" << str << "\"";
  }
  attribSgStatement(sstr(),node);
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

    int intent = 0;

    switch (kind){
    case SgAttributeSpecificationStatement::e_parameterStatement:
    case SgAttributeSpecificationStatement::e_externalStatement:
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

    case SgAttributeSpecificationStatement::e_dimensionStatement:
      ret = sb::buildAttributeSpecificationStatement( (SgAttributeSpecificationStatement::attribute_spec_enum)  kind  );
      SUBTREE_VISIT_BEGIN(node,astchild,ret)
        {
          exp = isSgExpression(astchild);
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
                  if(f_def==0) XEV_ABORT();
                  inam->set_scope(f_def);
                  inam->set_definition(ret);
                  lst.push_back(exp);
                }
              }
            }
          }
        }
      SUBTREE_VISIT_END();
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
    case SgAttributeSpecificationStatement::e_accessStatement_private:
    case SgAttributeSpecificationStatement::e_accessStatement_public:
    case SgAttributeSpecificationStatement::e_intentStatement:
    case SgAttributeSpecificationStatement::e_intrinsicStatement:
    case SgAttributeSpecificationStatement::e_optionalStatement:
    case SgAttributeSpecificationStatement::e_pointerStatement:
    case SgAttributeSpecificationStatement::e_saveStatement :
    case SgAttributeSpecificationStatement::e_targetStatement :

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

      if(XmlGetAttributeValue(node,"intent",&intent))
        ret->set_intent(intent);
      break;
    default:
      XEV_DEBUG_INFO(node);
      XEV_ABORT();
      break;
    }
    //TODO: need to consider forward declaration?
    if(ret) ret->set_definingDeclaration(ret);
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
    if( n->get_bind_list() )
      sstr() << " bind_list=\"1\" ";
    if( n->get_intent() )
      sstr() << " intent=\"" << n->get_intent() << "\" ";
  }
  attribSgStatement(sstr(),node);
}
/** XML internal node writer of SgAttributeSpecificationStatement */
void XevSageVisitor::inodeSgAttributeSpecificationStatement(SgNode* node)
{
  if(isSgAttributeSpecificationStatement(node)) {
    SgExprListExp* lste = 0;
    lste = isSgAttributeSpecificationStatement(node)->get_parameter_list();
    if( lste){
      SgExpressionPtrList& lst = lste->get_expressions();

      for(size_t i=0;i<lst.size();i++){
        this->visit(lst[i]);
      }
    }
    lste = isSgAttributeSpecificationStatement(node)->get_bind_list();
    if( lste) {
      SgExpressionPtrList& lst = lste->get_expressions();

      for(size_t i=0;i<lst.size();i++){
        this->visit(lst[i]);
      }
    }

    SgDataStatementGroupPtrList & lst
      = isSgAttributeSpecificationStatement(node)->get_data_statement_group_list();
    for(size_t i=0;i<lst.size();i++){
      this->visit(lst[i]);
    }
    SgStringList & slst =isSgAttributeSpecificationStatement(node)->get_name_list();
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
  bool                  isNamed = true;

  XmlGetAttributeValue(node,"type",&typ);
  isNamed = XmlGetAttributeValue(node,"name",&name);

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

  // "decl" is passed to SgClassDefinition and later replaced with "ret"
  SUBTREE_VISIT_BEGIN(node,astchild,decl)
    {
      if( exp==NULL )
        exp = isSgClassDefinition( astchild );
    }
  SUBTREE_VISIT_END();

  if(exp!=NULL){
    // defining declaration
    ret = new SgClassDeclaration( DEFAULT_FILE_INFO, name,
                                  (SgClassDeclaration::class_types)typ);
    ret->set_definition( exp );
    ret->set_parent(astParent);
    ret->set_scope(scope);
    ret->set_firstNondefiningDeclaration(decl->get_firstNondefiningDeclaration());
    ret->set_type(decl->get_type());
    ret->set_definingDeclaration(ret);

    decl->set_definingDeclaration(ret);

    exp->set_declaration(ret);
    exp->set_parent(ret);
  }
  else{
    // non-defining declaration
    ret = new SgClassDeclaration( DEFAULT_FILE_INFO, name,
                                  (SgClassDeclaration::class_types)typ);
    ret->set_firstNondefiningDeclaration(decl->get_firstNondefiningDeclaration());
    ret->set_type(decl->get_type());
    ret->set_definition(NULL);
    ret->set_definingDeclaration(decl->get_definingDeclaration());
    ret->set_class_type( (SgClassDeclaration::class_types)typ  );
    ret->set_type(SgClassType::createType(decl));
    ret->set_parent(scope);
    ret->set_scope(scope);
    //ret->setForward();
    decl->set_forward(true);
    ret->set_definingDeclaration(decl->get_definingDeclaration());
  }
  //if(isNamed==false) ret->set_isUnNamed(true);
  return ret;
}
/** XML attribute writer of SgClassDeclaration */
void XevSageVisitor::attribSgClassDeclaration(SgNode* node)
{
  SgClassDeclaration*      n = isSgClassDeclaration(node);

  if(n) {
    //if( n->get_isUnNamed() == false )
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
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
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

  SUBTREE_VISIT_BEGIN(node,astchild,0)
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

  if( XmlGetAttributeValue(node,"name",&name) ){
    ret = sb::buildEnumDeclaration( SgName( name.c_str() ), scope );
    ret->set_name(SgName( name.c_str()));
    ret->set_isUnNamed( false );

    esym = si::lookupEnumSymbolInParentScopes(name);
    if(esym==0){
      XEV_DEBUG_INFO(node);
      XEV_ABORT();
    }
    //scope->get_symbol_table()->print();
  }
  else {
    ret = sb::buildEnumDeclaration( SgName( name.c_str() ), scope );
    ret->set_isUnNamed( true );

  }
  ret->set_parent(astParent);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgInitializedName* ini = isSgInitializedName(astchild);
      if(ini){
        ret->append_enumerator(ini);
        ini->set_parent(ret);
        ret->set_definingDeclaration(ret);
        esym->set_declaration(ret);
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
/** XML attribute writer of SgEnumDeclaration */
void XevSageVisitor::attribSgEnumDeclaration(SgNode* node)
{
  SgEnumDeclaration*      n = isSgEnumDeclaration(node);

  if(n) {
    //if( n->get_isUnNamed() ==false)
    sstr() << " name=" << n->get_name() << " ";
  }
  attribSgStatement(sstr(),node);
}
INODE_DECL_DEFAULT(EnumDeclaration);


// =====================================================================
/// Visitor of a SgEquivalenceStatement element in an XML document
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
    if(lst->get_startOfConstruct()==0)
      lst->set_startOfConstruct(DEFAULT_FILE_INFO);
  }
  else {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  ret->set_parent(astParent);

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

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if((itm = isSgFormatItem(astchild))!=0){
        lst->get_format_item_list().insert(
              //lst->get_format_item_list().begin(), itm );
              lst->get_format_item_list().end(), itm );
      }
    }
  SUBTREE_VISIT_END();

  SgFormatStatement*    ret = new SgFormatStatement(DEFAULT_FILE_INFO,lst);
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

  if( XmlGetAttributeValue(node,"filename",&name) ){
    ret = sb::buildFortranIncludeLine( name );
  }
  else {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
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
  SgInitializedNamePtrList oldlst;

  string name;

  if( XmlGetAttributeValue(node,"name",&name) == false )
    XEV_ABORT();
  XmlGetAttributeValue(node,"old"         ,&old   );
  XmlGetAttributeValue(node,"definition"  ,&hasdef);

  FUNCTION_HEADER_VISIT_BEGIN(node,astchild,0)
    {
      if(lst==0)
        lst = isSgFunctionParameterList(astchild);
      if(typ==0)
        typ = isSgType(astchild);
    }
  FUNCTION_HEADER_VISIT_END();
  if(lst) {
    if(hasdef){
      ret
        = sb::buildDefiningFunctionDeclaration(SgName(name.c_str()), typ, lst);
      def = new SgFunctionDefinition(DEFAULT_FILE_INFO);
      def->set_parent(ret);
      ret->set_definition(def);
    }
    else
      ret
        = sb::buildNondefiningFunctionDeclaration(SgName(name.c_str()), typ, lst);
  }
  else {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }

  if(def &&  si::is_C_language()){
    ret->set_oldStyleDefinition(old);
    oldlst = ret->get_args();
    for(size_t i(0);i<oldlst.size();i++){
      //cerr << i << " " << oldlst[i]->get_name().getString() << endl;
      SgVariableSymbol* vsym =new SgVariableSymbol(oldlst[i]);
      def->get_symbol_table()->insert(oldlst[i]->get_name(),vsym);
    }
  }
  def = 0; // for visiting SgFunctionDefinition
  FUNCTION_BODY_VISIT_BEGIN(node,astchild,ret)
    {
      if(def==0)
        def = isSgFunctionDefinition(astchild);
    }
  FUNCTION_BODY_VISIT_END();

  if(hasdef == 0 ) {
    Sg_File_Info* info = DEFAULT_FILE_INFO;
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
    sstr() << " end_name=\"" << n->get_named_in_end_statement() << "\" ";
    if(n->get_oldStyleDefinition())
      sstr() << " old=\"1\" ";
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

    for(size_t i(0);i<n->get_args().size();i++)
      this->visit(n->get_args()[i]);
  }
}


// =====================================================================
/// Visitor of a SgFunctionParameterList element in an XML document
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
        ini->set_parent(ret);
      }
    }
  SUBTREE_VISIT_END();
  ret->set_parent(astParent);

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
    XEV_WARN("not implemented yet");
    XEV_DEBUG_INFO(node);
    XEV_ABORT(); //TODO
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
    XEV_WARN("nondefining module declaration is not implemented yet");
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
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
  ret->set_parent(astParent);
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

   SUBTREE_VISIT_BEGIN(node,child,0)
     {
       if(pr==0)
         pr = isSgPragma(child);
     }
   SUBTREE_VISIT_END();

   if(pr) {
     ret = sb::buildPragmaDeclaration(pr->get_pragma());
   }
   else {
     XEV_DEBUG_INFO(node);
     XEV_ABORT();
   }
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
   FUNCTION_HEADER_VISIT_BEGIN(node,astchild,0);
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

   if(lst && typ){
     if( kind != SgProcedureHeaderStatement::e_block_data_subprogram_kind ){
       ret = sb::buildProcedureHeaderStatement( (const char*)(name.c_str()), typ, lst,
                                                (SgProcedureHeaderStatement::subprogram_kind_enum)kind, scope);
       //SgNode::get_globalFunctionTypeTable()->print_functypetable(std::cerr);
       //std::cerr << "--------------------" << ret->get_type() -> get_return_type() -> class_name() << std::endl;
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
   else {
     XEV_DEBUG_INFO(node);
     XEV_ABORT();
   }
   if(var){
     var->set_parent(ret);
     var->set_scope(scope); // is this OK??
     //var->set_scope(sb::topScopeStack());
   }
   ret->set_scope(scope);
   ret->set_parent(astParent);
   ret->set_oldStyleDefinition(f_old);

   // -----------------------------------------------------------------------
   // "*" in parameter list is considered an alternative return
   if(lst && si::is_Fortran_language()==true){
     int counter=1;
     SgScopeStatement* scope = ret->get_definition();
     for(size_t i(0);i<lst->get_args().size();i++){
       SgInitializedName* ini = lst->get_args()[i];
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
       else{ // probably never reach here
         XEV_DEBUG_INFO(node);
         XEV_ABORT();
       }
     }
   } // if lst

   // -----------------------------------------------------------------------
   // function body
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
   if(fdf){
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
   }
   else {
     XEV_DEBUG_INFO(node);
     XEV_ABORT();
   }
   if(f_pure)
     ret->get_functionModifier().setPure();
   if(f_elem)
     ret->get_functionModifier().setElemental();
   if(f_recur)
     ret->get_functionModifier().setRecursive();
   ret->set_parent(astParent);

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
     XEV_DEBUG_INFO(node);
     XEV_ABORT();
   }
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
   if(fdf->get_file_info()==0) {
     XEV_DEBUG_INFO(node);
     XEV_ABORT();
   }
   if(def && def->get_file_info()==0) {
     XEV_DEBUG_INFO(node);
     XEV_ABORT();
   }

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

   //xe::DOMNamedNodeMap* amap = node->getAttributes();
   //xe::DOMNode* satt = 0;
   string name;

   XmlGetAttributeValue(node,"name",&name);

   SUBTREE_VISIT_BEGIN(node,astchild,0)
     {
       if(typ==0)
         typ = isSgType(astchild);
       if(decl==0)
         decl = isSgDeclarationStatement(astchild);
     }
   SUBTREE_VISIT_END();

   if(decl) {
     SgNamedType *namedType = isSgNamedType(typ->findBaseType());
     ret = sb::buildTypedefDeclaration( name.c_str(),
                                        namedType,
                                        sb::topScopeStack());
     XEV_ASSERT(ret!=NULL);
     ret->set_declaration(decl->get_firstNondefiningDeclaration());
     ret->set_definingDeclaration(ret);
     ret->set_requiresGlobalNameQualificationOnType(true);
     if(si::is_Fortran_language()==false)
       ret->set_typedefBaseTypeContainsDefiningDeclaration(true);
     decl->set_parent(ret);

     SgTypedefSymbol* tsym =si::lookupTypedefSymbolInParentScopes(name);
     if(tsym){
       tsym->get_declaration()->set_definingDeclaration(ret);
     }
     else{
       XEV_DEBUG_INFO(node);
       XEV_ABORT();
     }
   }
   else if(typ) {                                         // add (0819)
     ret = sb::buildTypedefDeclaration( name.c_str(),
                                        typ,
                                        sb::topScopeStack());
   }
   else {
     XEV_DEBUG_INFO(node);
     XEV_ABORT();
   }
   ret->set_parent(astParent);
   return ret;
 }
 /** XML attribute writer of SgTypedefDeclaration */
 void XevSageVisitor::attribSgTypedefDeclaration(SgNode* node)
 {
   SgTypedefDeclaration*      n = isSgTypedefDeclaration(node);

   if(n) {
     sstr() << " name=" << n->get_name() << " ";
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
   if(XmlGetAttributeValue(node,"name",&name) && XmlGetAttributeValue(node,"only",&only))
     ret = new SgUseStatement(DEFAULT_FILE_INFO,name,only);
   else {
     XEV_DEBUG_INFO(node);
     XEV_ABORT();
   }
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
  Rose_STL_Container<SgInitializedName*>    varList;
  //unsigned long     storage=0U;
  string            bitstr;
  unsigned long     bit = 0;

  //XmlGetAttributeValue(node,"modifier",&storage);
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

      //cls->set_parent( typ );
      namedType->set_declaration (cls);
      namedType->get_declaration()->set_definingDeclaration(cls);
      ret = sb::buildVariableDeclaration(name->get_name(),
                                         namedType,
                                         name->get_initializer());

      ret->set_baseTypeDefiningDeclaration( cls );
      cls->set_parent(ret);
    }
    else{
      ret = sb::buildVariableDeclaration(name->get_name(),
                                         name->get_type(),
                                         name->get_initializer());
      name->set_definition(ret->get_definition());
    }
    if(ret==0) {
      XEV_DEBUG_INFO(node);XEV_ABORT();
    }
    ret->set_parent(astParent);
    ret->set_definingDeclaration(ret);

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
      varList[i]->set_declptr(ret);
      varList[i]->set_scope(name->get_scope());
      //varList[i]->set_type(name->get_type());
    }

    // this is necessary because declaration is required for each variable.
    for(size_t i(1);i<varList.size();i++){
      SgVariableDeclaration* decl =sb::buildVariableDeclaration(varList[i]->get_name(),
                                                                varList[i]->get_type(),
                                                                varList[i]->get_initializer());
      varList[i]->set_definition(decl->get_definition());
    }
  }
  else{
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }

#if 0 // this is no longer required
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
          SgNode * old_parent = new_initName->get_parent();
          //XEV_WARN("old:" << name->get_name() << " parent="<<old_parent->class_name());
          //if(old_parent==0 || isSgFunctionParameterList(old_parent)==0) XEV_ABORT();
          if(isSgFunctionParameterList(old_parent)) {
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
  }
  if (! isFortranParameter)
    si::fixVariableDeclaration(ret,ret->get_scope());
#endif

  // set bitfield (2013.08.06)
  if( bitstr.size() ) {
    bit = strtoul( bitstr.c_str(),0,0 );
    SgUnsignedLongVal* val = new SgUnsignedLongVal( bit,bitstr );
    val->set_startOfConstruct(DEFAULT_FILE_INFO);
    ret->set_bitfield (val);
    val->set_parent(ret);
  }

  // check the result variable
  if( si::is_Fortran_language() ){
    SgProcedureHeaderStatement* prc = isSgProcedureHeaderStatement(si::getEnclosingFunctionDeclaration(ret));
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
  return ret;
}
/** XML attribute writer of SgVariableDeclaration */
void XevSageVisitor::attribSgVariableDeclaration(SgNode* node)
{
  SgVariableDeclaration* n = isSgVariableDeclaration(node);
  if(n) {
    SgUnsignedLongVal *bit = isSgUnsignedLongVal(n->get_bitfield());
    if( bit )
      sstr() << " bitfield=\"" << bit->get_value() << "\" ";
  }
  attribSgDeclarationStatement(sstr(),node);
}
INODE_DECL_DEFAULT(VariableDeclaration);
