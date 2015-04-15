/**
 * @file     support.cpp
 * @brief    Visitor Functions of SgSupport classes.
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

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;
using namespace XevXml;

#define ATTRIB_SUPP_DEFAULT(x)                          \
  /** XML attribute writer of Sg##x */                  \
  void XevSageVisitor::attribSg##x(SgNode* node)        \
  {                                                     \
    return;                                             \
  }

#define INODE_SUPP_DEFAULT(x)                           \
  /** XML internal node writer of Sg##x */              \
  void XevSageVisitor::inodeSg##x(SgNode* node)         \
  {                                                     \
    return;                                             \
  }

#define SUPP_DEFAULT(x)                         \
  ATTRIB_SUPP_DEFAULT(x)                        \
  INODE_SUPP_DEFAULT(x)

// ===============================================================================
/// Visitor of a SgDataStatementGroup element in an XML document
SgNode*
XevXmlVisitor::visitSgDataStatementGroup(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDataStatementGroup*   ret = new SgDataStatementGroup();
  ret->set_parent(NULL);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgDataStatementObject* obj = isSgDataStatementObject(astchild);
      SgDataStatementValue*  val = isSgDataStatementValue (astchild);

      if(obj)
        ret->get_object_list().push_back(obj);
      if(val)
        ret->get_value_list() .push_back(val);
    }
  SUBTREE_VISIT_END();

  return ret;
}
ATTRIB_SUPP_DEFAULT(DataStatementGroup);
/** XML internal node writer of SgDataStatementGroup */
void XevSageVisitor::inodeSgDataStatementGroup(SgNode* node){
  SgDataStatementObjectPtrList & lst =
    isSgDataStatementGroup(node)->get_object_list();
  for(size_t i=0;i<lst.size();i++)
    this->visit(lst[i]);

  SgDataStatementValuePtrList & val =
    isSgDataStatementGroup(node)->get_value_list();
  for(size_t i=0;i<val.size();i++){
    this->visit(val[i]);
  }
}


// ===============================================================================
/// Visitor of a SgDataStatementObject element in an XML document
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
  else{
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  ret->set_parent(NULL);
  return ret;
}
ATTRIB_SUPP_DEFAULT(DataStatementObject);
/** XML internal node writer of SgDataStatementObject */
void XevSageVisitor::inodeSgDataStatementObject(SgNode* node){
  SgDataStatementObject* obj = isSgDataStatementObject(node);
  this->visit(obj->get_variableReference_list());
}



// ===============================================================================
/// Visitor of a SgDataStatementValue element in an XML document
SgNode*
XevXmlVisitor::visitSgDataStatementValue(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDataStatementValue*  ret = new SgDataStatementValue();
  SgExprListExp* ilst = 0;
  SgExpression*  rept = 0;
  SgExpression*  cnst = 0;
  int format = 0;

  XmlGetAttributeValue(node,"fmt",&format);

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
/** XML attribute writer of SgDataStatementValue */
void XevSageVisitor::attribSgDataStatementValue(SgNode* node)
{
  SgDataStatementValue* n = isSgDataStatementValue(node);
  if(n){
    sstr() << " fmt=\"" << n->get_data_initialization_format() << "\" ";
  }
}
/** XML interanal node writer of SgDataStatementValue */
void XevSageVisitor::inodeSgDataStatementValue(SgNode* node)
{
  SgDataStatementValue* v = isSgDataStatementValue(node);
  this->visit(v->get_initializer_list());
  this->visit(v->get_repeat_expression());
  this->visit(v->get_constant_expression());
}



// ===============================================================================
/// Visitor of a SgFormatItem element in an XML document
SgNode*
XevXmlVisitor::visitSgFormatItem(xe::DOMNode* node, SgNode* astParent)
{
  SgExpression*         exp=NULL;
  SgStringVal*          val=NULL;
  //SgExpression*         val;
  //string             fmt;
  int                sgl=0;
  int                dbl=0;
  int                lst=0;
  int                rpt=0;
  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt = 0;

  //XmlGetAttributeValue(node,"fmt"   ,&fmt);
  XmlGetAttributeValue(node,"single"  ,&sgl);
  XmlGetAttributeValue(node,"double"  ,&dbl);
  XmlGetAttributeValue(node,"list"    ,&lst);
  XmlGetAttributeValue(node,"repeat"  ,&rpt);
  //cerr << "SgFormatItem |" << fmt << "| end" << endl;

  SgFormatItem*     ret = new SgFormatItem();
  SgFormatItemList* il = new SgFormatItemList();
  //if(lst==0){
    SUBTREE_VISIT_BEGIN(node,astchild,ret)
      {
        if(exp==0)
          exp = isSgExpression(astchild);
        if(isSgFormatItem(astchild))
          il->get_format_item_list().push_back(isSgFormatItem(astchild));
      }
    SUBTREE_VISIT_END();
    if(exp){
      ret->set_data(exp);
    }
    if(il->get_format_item_list().size()){
      ret->set_format_item_list(il);
    }

    if(exp && rpt==0 && il->get_format_item_list().size()==0) {
      XEV_DEBUG_INFO(node);
      XEV_ABORT();
    }
    val = isSgStringVal( exp );
    if( val && sgl == 1 ) {                     // add (0821)
      val->set_usesSingleQuotes(true);
    }
    if( val && dbl == 1 ) {                     // add (0821)
      val->set_usesDoubleQuotes(true);
    }
    if( rpt )
      ret->set_repeat_specification(rpt);
    //}
    /*
    else {
    SgFormatItemList* il = new SgFormatItemList();
    SUBTREE_VISIT_BEGIN(node,astchild,ret)
      {
        if(isSgFormatItem(astchild))
          il->get_format_item_list().push_back(isSgFormatItem(astchild));
      }
    SUBTREE_VISIT_END();
    ret->set_format_item_list(il);
    }*/
  ret->set_parent(NULL);
  return ret;
}
/** XML attribute writer of SgFormatItem */
void XevSageVisitor::attribSgFormatItem(SgNode* node)
{
  SgFormatItem* n = isSgFormatItem(node);

  if(n) {
    SgStringVal* v = isSgStringVal(n->get_data());
    if(v) {
      //sstr() << " fmt=\"" << XevXml::XmlStr2Entity(v->get_value()) << "\" ";
      if( v->get_usesSingleQuotes() == true )
        sstr() << " single=\"1\" ";
      if( v->get_usesDoubleQuotes() == true )
        sstr() << " double=\"1\" ";
    }
    if (n->get_format_item_list()){
      sstr() << " list=\"1\" ";
    }
    if(n->get_repeat_specification()){
      sstr() << " repeat=\""<< n->get_repeat_specification() <<"\" ";
    }
  }
}
/** XML interanal node writer of SgFormatItemValue */
void XevSageVisitor::inodeSgFormatItem(SgNode* node)
{
  SgFormatItem* n = isSgFormatItem(node);
  SgFormatItemList* l = n->get_format_item_list();

  if(n->get_data())
    this->visit(n->get_data());
  if(l){
    SgFormatItemPtrList& pl =l->get_format_item_list();
    for(size_t i(0);i< pl.size();i++){
      this->visit(pl[i]);
    }
  }
  //else XEV_ABORT();
}
//INODE_SUPP_DEFAULT(FormatItem);


// ===============================================================================
/// Visitor of a SgFunctionParameterTypeList element in an XML document
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
    exp = new SgExprListExp(DEFAULT_FILE_INFO );

  ret =  sb::buildFunctionParameterTypeList( exp );
  ret->set_parent(astParent);
  exp->set_parent(ret);

  return ret;
}
ATTRIB_SUPP_DEFAULT(FunctionParameterTypeList);
/** XML internal node writer of SgFunctionParameterTypeList */
void XevSageVisitor::inodeSgFunctionParameterTypeList(SgNode* node)
{
  SgTypePtrList & lst = isSgFunctionParameterTypeList(node)->get_arguments();
  for(size_t i=0;i<lst.size();i++)
    this->visit(lst[i]);
}

// ===============================================================================
/// Visitor of a SgNameGroup element in an XML document
SgNode*
XevXmlVisitor::visitSgNameGroup(xe::DOMNode* node, SgNode* astParent)
{
  SgNameGroup* ret = new SgNameGroup();
  string names;
  string gname;

  if(XmlGetAttributeValue(node,"group",&gname) && XmlGetAttributeValue(node,"names",&names)){
    char* str = (char*)names.c_str();
    char* c;
    SgStringList slst;
    ret->set_group_name( gname );;

    c = strtok(str,",");
    if(c) slst.push_back(c);
    while(c){
      c = strtok(NULL,",");
      if(c) slst.push_back(c);
    }
    ret->get_name_list() = slst;
  }
  else{
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }

  ret->set_parent(NULL);//parent must be set to NULL.
  return ret;
}
/** XML attribute writer of SgNameGroup */
void XevSageVisitor::attribSgNameGroup(SgNode* node)
{
  SgNameGroup* n = isSgNameGroup(node);
  if(n) {
    sstr() << " group=\"" << n->get_group_name() << "\" ";;

    SgStringList& nl = n->get_name_list();
    if(nl.size()){ // this should be true.
      sstr() << " names=\"";
      for(size_t i(0);i<nl.size();i++){
        sstr() << nl[i];
        if( i < nl.size()-1)
          sstr() << ",";
      }
      sstr() << "\" ";
    }
  }
}
INODE_SUPP_DEFAULT(NameGroup);


// ===============================================================================
/// Visitor of a SgPragma element in an XML document
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
  else {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  return ret;
}
/** XML attribute writer of SgPragma */
void XevSageVisitor::attribSgPragma(SgNode* node)
{
  SgPragma* n = isSgPragma(node);
  /* todo: the line should be split into tokens, and written as attriutes */
  if(n) {
    sstr() << " pragma=\"" << XmlStr2Entity(n->get_pragma()) << "\" ";
  }
}
INODE_SUPP_DEFAULT(Pragma);


// ===============================================================================
/// Visitor of a SgSourceFile element in an XML document
SgNode*
XevXmlVisitor::visitSgSourceFile(xe::DOMNode* node, SgNode* astParent)
{
  xe::DOMNode* child=node->getFirstChild();
  int langid;
  int fmtid;

  Sg_File_Info* info = DEFAULT_FILE_INFO;
  _file->set_file_info(info);
  info->set_parent(_file);

  if(XmlGetAttributeValue(node,"lang",&langid)==false)
    XEV_WARN("Language attribute is missing.");
  if(XmlGetAttributeValue(node,"fmt",&fmtid)==false)
    XEV_WARN("Format attribute is missing.");

  // 0:error, 1: unknown, 2:C, 3:C++, 4:Fortran
  // C++ is not supported for now. trying to output as a C program
  if(langid==2 || langid == 3 ){
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
  }

  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      this->visit(child,_file);
    }
    child=child->getNextSibling();
  }

  return _file;
}
/** XML attribute writer of SgSourceFile */
void XevSageVisitor::attribSgSourceFile(SgNode* node)
{
  SgSourceFile*  n = isSgSourceFile(node);
  if(n) {
    sstr() << " file=\"" << n->get_sourceFileNameWithoutPath() << "\"";
    sstr() << " lang=\"" << n->get_outputLanguage() << "\"";
    sstr() << " fmt=\"" << n->get_outputFormat() << "\"";
  }
}
INODE_SUPP_DEFAULT(SourceFile);


// ===============================================================================
/// Visitor of a SgTypedefSeq element in an XML document
//@note I don't fully understand what this class represents.
SgNode*
XevXmlVisitor::visitSgTypedefSeq(xe::DOMNode* node, SgNode* astParent)
{
  SgTypedefSeq* ret = 0;

  ret = new SgTypedefSeq();
  ret->set_parent(NULL);
  return ret;
}
SUPP_DEFAULT(TypedefSeq);



// ===============================================================================
/// Visitor of a SgCommonBlockObject element in an XML document
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

  if(para==0) {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  ret->set_variable_reference_list(para);
  para->set_parent(ret);
  ret->set_parent(astParent);

  return ret;
}
/** XML attribute writer of SgCommonBlockObject */
void XevSageVisitor::attribSgCommonBlockObject(SgNode* node)
{
  SgCommonBlockObject* n = isSgCommonBlockObject(node);
  if(n) {
    sstr() << " name=\"" << n->get_block_name() << "\" ";
  }
}
INODE_SUPP_DEFAULT(CommonBlockObject);



// ===============================================================================
/// Visitor of a SgInitializedName element in an XML document
SgNode*
XevXmlVisitor::visitSgInitializedName(xe::DOMNode* node, SgNode* astParent)
{
  SgInitializedName* ret = 0;
  SgInitializer*     ini = 0;
  SgType*            typ = 0;
  SgScopeStatement* scope = sb::topScopeStack();

  string               name;
  string               prev;

  XmlGetAttributeValue(node,"name",&name);

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(ini==0)
        ini = isSgInitializer(astchild);
      if(typ==0)
        typ = isSgType(astchild);
    }
  SUBTREE_VISIT_END();

//  if(isSgArrayType(typ))
  //  typ = isSgArrayType(typ)->get_base_type();
  ret = sb::buildInitializedName(name.c_str(),typ,ini);
  ret->set_parent(astParent);
  ret->set_scope(scope);// This was NG for s009 but needed by s005
  if(typ) {
    typ->set_parent(ret); // this must be true
  }
  if(ini) ini->set_parent(ret);

  unsigned long mod = (unsigned long)SgStorageModifier::e_default;
  if(XmlGetAttributeValue(node,"storage_modifier",&mod)){
    ret->get_storageModifier().set_modifier((SgStorageModifier::storage_modifier_enum)mod);
  }

  // for SgTypeCrayPointer support
  if(isSgTypeCrayPointer(typ) && XmlGetAttributeValue(node,"prev",&prev)){
    SgVariableSymbol* vsym = si::lookupVariableSymbolInParentScopes(SgName(prev.c_str()));
    if(vsym==0) {
      XEV_DEBUG_INFO(node);XEV_ABORT();
    }
    SgInitializedName* ini = vsym->get_declaration();
    if(ini==0) {
      XEV_DEBUG_INFO(node);XEV_ABORT();
    }
    ret->set_prev_decl_item(ini);
  }
  return ret;
}
/** XML attribute writer of SgInitializedName */
void XevSageVisitor::attribSgInitializedName(SgNode* node)
{
  SgInitializedName* n = isSgInitializedName(node);
  if(n) {
    sstr() << " name=" << n->get_name() << " ";
    if(n->get_prev_decl_item()){
      // used for SgTypeCrayPointer
      sstr() << " prev=" << n->get_prev_decl_item()->get_name() << " ";
    }
    if(n->get_storageModifier().get_modifier() !=SgStorageModifier::e_default)
      sstr() << " storage_modifier=\"" << n->get_storageModifier().get_modifier() <<"\" ";
  }
}
/** XML internal node writer of SgInitializedName */
void XevSageVisitor::inodeSgInitializedName(SgNode* node)
{
  this->visit(isSgInitializedName(node)->get_typeptr());
}


// ===============================================================================
/// Visitor of a SgInterfaceBody element in an XML document
SgNode*
XevXmlVisitor::visitSgInterfaceBody(xercesc::DOMNode* node, SgNode* astParent)
{
  SgInterfaceBody*                ret=NULL;
  SgFunctionDeclaration*          bdy=NULL;
  //SgProcedureHeaderStatement*     def=0;
  string                          name;

  XmlGetAttributeValue(node,"name",&name);
  //ret = new SgInterfaceBody(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  //ret = new SgInterfaceBody( astParent->get_file_info() );
  ret = new SgInterfaceBody(name, NULL, true );
  ret->set_file_info(astParent->get_file_info());
  ret->set_parent(astParent);
#if 0
  /*--- 2013.08.05 delete --*/
  if( name.size() ) {
    ret->set_function_name( SgName( name.c_str() ) );
    ret->set_use_function_name( true );
  }
  /*---*/
#endif
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( bdy==0 )
        bdy = isSgFunctionDeclaration(astchild);
      //if(def==0)
      //def = isSgProcedureHeaderStatement(astchild);
    }
  SUBTREE_VISIT_END();

  if(bdy){
    ret->set_functionDeclaration( bdy );
    ret->set_use_function_name( false );
    bdy->set_parent(ret);
  }
  else if( name.size() ) {
    ret->set_function_name( SgName( name.c_str() ) );
    ret->set_use_function_name( true );
  }
  else {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  return ret;
}
/** XML attribute writer of SgInterfaceBody */
void XevSageVisitor::attribSgInterfaceBody(SgNode* node)
{
  SgInterfaceBody* n = isSgInterfaceBody(node);
  if(n) {
    if( n->get_use_function_name() == true )
      sstr() << " name=" << n->get_function_name() << " ";
  }
}
/** XML internal node writer of SgInterfaceBody */
void XevSageVisitor::inodeSgInterfaceBody(SgNode* node)
{
  SgInterfaceBody* n = isSgInterfaceBody(node);
  if(n && n->get_use_function_name() ==false ){
    this->visit(n->get_functionDeclaration());
  }
}



// ===============================================================================
/// Visitor of a SgRenamePair element in an XML document
SgNode*
XevXmlVisitor::visitSgRenamePair(xercesc::DOMNode* node, SgNode* astParent)
{
  SgRenamePair*     ret = 0;
  string                      lname;
  string                      uname;

  if(XmlGetAttributeValue(node,"lname",&lname)==false
     || XmlGetAttributeValue(node,"uname",&uname)==false )
    XEV_ABORT();

  ret = new SgRenamePair(lname,uname);
  if(ret) {
    ret->set_parent(astParent);
#if 0
    // add this SgRenamePair object to * its parent node *
    SgUseStatement* useStmt = isSgUseStatement(astParent);
    if(useStmt){ // this should be true
      useStmt->get_rename_list().push_back(ret);
    }
#endif
  }
  else {
    XEV_DEBUG_INFO(node);
    XEV_ABORT();
  }
  Sg_File_Info* info = DEFAULT_FILE_INFO;
  info->setOutputInCodeGeneration();
  ret->set_file_info(info);
  return ret;
}
/** XML attribute writer of SgRenamePair */
void XevSageVisitor::attribSgRenamePair(SgNode* node)
{
  SgRenamePair*  n = isSgRenamePair(node);
  if(n) {
    sstr() << " lname=" << n->get_local_name() << "";
    sstr() << " uname=" << n->get_use_name() << "";
  }
}
INODE_SUPP_DEFAULT(RenamePair);
