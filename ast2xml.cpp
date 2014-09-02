/**
 * @file     ast2xml.cpp
 * @brief    ROSE Sage III AST is converted to an XML document.
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
#include "ast2xml.hpp"
#include "attrib.hpp"
#include <string>
#include <algorithm>

using namespace std;


namespace xevxml {
  //void Ast2Xml(stringstream& sstr, SgProject* prj, int fileid)
  void Ast2Xml(stringstream& sstr, SgFile* file, Ast2XmlOpt* opt)
  {
    Ast2XmlVisitor visitor(sstr);
    Ast2XmlInheritedAttribute att(opt);
    visitor.traverseWithinFile(file,att);
    
    return;
  }
}

using namespace xevxml;
/*!
* @brief        hasInternalNode
*/
static bool hasInternalNode(SgNode* n)
{
  if(isSgArrayType(n)){
    return true;
  }
  /*
  if(isSgFunctionParameterList(n)){
    return true;
  }
  */
  if(isSgAttributeSpecificationStatement(n))
    return true;

  if(isSgSizeOfOp(n)){
    string s = n->class_name();
    if( s == "SgSizeOfOp" )
      return true;
  }

  if(isSgFormatStatement(n))
    return true;

  if(isSgInterfaceBody(n))  
    return true;

  if(isSgNamelistStatement(n))
    return true;

  if(isSgArithmeticIfStatement(n))
    return true;

  if(isSgPointerDerefExp(n))     
    return true;

  if(isSgVarArgOp(n)) 
    return true;

  if(isSgEquivalenceStatement(n))
    return true;

  if(isSgFunctionParameterTypeList(n))
    return true;

  if(isSgInquireStatement(n)) 
    return true;

  if(isSgTypedefDeclaration(n))
    return true;

  if(isSgDataStatementGroup(n)) 
    return true;

  if(isSgInquireStatement(n))
    return true;


  return false;
}

static void writeFortranPragma(stringstream& sstr_,  AttachedPreprocessingInfoType* info, PreprocessingInfo::RelativePositionType pos=PreprocessingInfo::before)
{
  std::string str;
  int idx;

  if(info){
    for(size_t i(0);i<(*info).size();i++) {
      if((*info)[i]->getRelativePosition()==pos){
	str = (*info)[i]->getString();
	std::transform(str.begin(),str.end(),str.begin(),::tolower);
	idx = str.find( XEV_PRAGMA_PREFIX );
	if( idx >= 0 ) {
	  str = (*info)[i]->getString(); // read the string again
	  sstr_ << "<SgPragmaDeclaration >\n";
	  sstr_ << "  "; // indent
	  sstr_ << "<SgPragma pragma=\"";
	  // assuming Fortran directives start with !$
	  sstr_ << XmlStr2Entity(str.substr( idx+strlen("!$") )) << "\" />\n";
	  sstr_ << "</SgPragmaDeclaration >\n";
	}
      }
    }
  }
}

static AttachedPreprocessingInfoType* 
writePreprocessingInfo(stringstream& sstr_,SgNode* n)
{
  SgLocatedNode* loc = isSgLocatedNode(n);
  AttachedPreprocessingInfoType* info=0;
  std::string str;

  if(loc)
    info = loc->getAttachedPreprocessingInfo();

  if(info){
    for(size_t i(0);i<(*info).size();i++) {
      str = (*info)[i]->getString();
      str = XmlStr2Entity( str );                     
      sstr_ << "<PreprocessingInfo pos=\"";
      sstr_ << (*info)[i]->getRelativePosition() <<"\" ";
      sstr_ << " type=\"";
      sstr_ << (*info)[i]->getTypeOfDirective() << "\" >\n";
      //sstr_ << (*info)[i]->getString() << "\n";
      sstr_ << str << "\n";
      sstr_ << "</PreprocessingInfo>\n";
    }
  }

  return info;
}

static void writeModifierType(stringstream& istr,SgType* t)
{
  SgModifierType* n = isSgModifierType(t);
  if(n) {
    SgTypeModifier m = n->get_typeModifier();
    if( m.isRestrict() ) 
      istr << " modifier=\"restrict\" ";
    
    SgConstVolatileModifier cv = m.get_constVolatileModifier();
    if( cv.isConst() ) 
      istr << " modifier=\"const\" ";
    else if(cv.isVolatile() ) 
      istr << " modifier=\"volatile\" ";
  }
}

static void writeTypesRecursive(stringstream& sstr,
				SgType* t, 
				Ast2XmlInheritedAttribute att,bool f=true)
{
  if(t==0) return;
  for(int j(0);j<att.level;j++)
    sstr << "  ";
  
  sstr << '<';
  sstr << t->class_name();
  if(att.opt->address){
    sstr << " address=\"";
    sstr.setf(ios::hex,ios::basefield);
    sstr << t << "\"";
    sstr.unsetf(ios::hex);
  }
  writeModifierType(sstr,t);
  
  string s = t->class_name();
  if( s == "SgTypedefType" ){
    SgTypedefType* n = isSgTypedefType(t);
    sstr << " type_name=" << n->get_name();
  }
  else if( s == "SgTypeString" ){
    SgIntVal* v = isSgIntVal( isSgTypeString(t)->get_lengthExpression() );
    if( v )
      sstr << " len=\"" << v->get_value() << "\" ";
    
    SgExpression*   exp = isSgTypeString(t)->get_lengthExpression();
    if( exp )
      sstr << " lengthExpression=\"" << exp->class_name() << "\" ";
  }
  else if( s == "SgTypeComplex" ) {
    SgIntVal* v = isSgIntVal( isSgTypeComplex(t)->get_type_kind() );
    sstr << " base_type=\"" << isSgTypeComplex(t)->get_base_type()->class_name() << "\" ";
    if( v )
      sstr << " type_kind=\"" << v->get_valueString() << "\" ";
    else
      sstr << " type_kind=\"\" ";
  }
  else if( s == "SgArrayType" ) {
    SgUnsignedLongVal* ul = isSgUnsignedLongVal( isSgArrayType(t)->get_index() );
    sstr << " rank=\"" << isSgArrayType(t)->get_rank() << "\" ";
    if( ul )
      sstr << " index=\"" << ul->get_value() << "\" ";
    else
      sstr << " index=\"\" ";
    
    sstr << " type=\"" << isSgArrayType(t)->get_base_type()->class_name() << "\" ";
  }
  else if( s == "SgClassType" ) {
    SgClassType* n = isSgClassType(t);
    SgDeclarationStatement* ds = n->get_declaration();
    SgClassDeclaration*     cd = isSgClassDeclaration(ds);
    if( cd->get_isUnNamed() )
      sstr << " tag_name=\"\" ";
    else
      sstr << " tag_name=" << n->get_name() << " ";
    sstr << " type=\"" << cd->get_class_type() << "\" ";
  }
  else if( s == "SgPointerType" ) {
    SgPointerType* n = isSgPointerType(t);
    if( n )
      sstr << " base_type=\"" << n->get_base_type()->class_name() << "\" ";
  }
  else if( s == "SgTypeImaginary" ) {
    SgIntVal* v = isSgIntVal( isSgTypeImaginary(t)->get_type_kind() );
    sstr << " base_type=\"" << isSgTypeImaginary(t)->get_base_type()->class_name() << "\" ";
    
    if( v )
      sstr << " type_kind=\"" << v->get_valueString() << "\" ";
    else
      sstr << " type_kind=\"\" ";
  }

  if(t->containsInternalTypes()==true && f){
    sstr << ">" << endl;
    Rose_STL_Container<SgType*> types = t->getInternalTypes();
    att.level += 1;
    
    for(size_t i(0);i<types.size();++i){
      writeTypesRecursive(sstr,types[i], att,true);
    }
    
    if( s == "SgArrayType" ) { 
      SgExprListExp* lste = isSgArrayType(t)->get_dim_info();
      if( lste ) {
        Ast2XmlVisitorInternal visitor(sstr);
        SgExpressionPtrList& lst = lste->get_expressions();
        for(size_t i=0;i<lst.size();i++){
          visitor.traverse(lst[i],att);
          //writeTypesRecursive(sstr,isSgType(lst[i]), att,true);
        }
      }
    }
    
    if( s == "SgFunctionType" ) { 
      SgFunctionParameterTypeList* lst = isSgFunctionType(t)->get_argument_list();
      if( lst ) {
          Ast2XmlVisitorInternal visitor(sstr);
          writeTypesRecursive(sstr,isSgFunctionType(t)->get_return_type(), att,true);
          visitor.traverse(lst,att);
      }
    }

    att.level -= 1;
    for(int j(0);j<att.level;j++)
      sstr << "  ";
    sstr << "</";
    sstr << t->class_name();
    sstr << ">" << endl;
  }
  else
    sstr << "/>" << endl;
}


/* --- check if the node needs to write SgType elements --- */
static SgType* hasType(SgNode* node)
{
  if(isSgInitializer(node)) 
    return isSgInitializer(node)->get_type();
  else if (isSgInitializedName(node)) 
    return isSgInitializedName(node)->get_type();
  else if (isSgFunctionDeclaration(node)) 
    return isSgFunctionDeclaration(node)->get_orig_return_type();
  else if (isSgCastExp(node)) 
    return isSgCastExp(node)->get_type();
  return 0;
}

/* --- write SgType elements of a node --- */
static bool writeTypes(stringstream& sstr,
		       SgNode* node, 
		       Ast2XmlInheritedAttribute att)
{
  SgType* t=hasType(node);
  if(t==0) return false;

  att.level += 1;
  writeTypesRecursive(sstr,t,att);
  att.level -= 1;

  return true;
}

/* --- check if the node is a leaf node --- */
static bool isLeafNode(SgNode* node)
{
  if( node->numberOfNodesInSubtree() > 1 ) 
    return false;
  if( hasInternalNode(node) )
    return false;
  if( hasType(node) )
    return false;
  
  return true;
}

#define TRAVERSE_IF_EXISTS(x) if(x){visitor.traverse(x,att);}

/*!
* @brief        writeInternalNode
*/
static void writeInternalNode(stringstream& sstr,
			      SgNode* n, 
			      Ast2XmlInheritedAttribute att)
{
  if(hasInternalNode(n)==false) return;

  Ast2XmlVisitorInternal visitor(sstr);
  if(isSgArrayType(n)) {
    visitor.traverse(isSgArrayType(n)->get_dim_info(),att);
  }
  else if(isSgFunctionParameterList(n)){
    SgFunctionParameterList* plst = isSgFunctionParameterList(n);
    SgInitializedNamePtrList& args = plst->get_args();
    // args.size() must be divided by sizeof(void*) ???
    //for(size_t i(0);i<args.size()/sizeof(void*);i++)
    for(size_t i(0);i<args.size();i++)
      visitor.traverse(args[i],att);
  }

  else if(isSgAttributeSpecificationStatement(n)) {
    SgExprListExp* lste = 0;
    lste = isSgAttributeSpecificationStatement(n)->get_parameter_list();
    if( lste){
      SgExpressionPtrList& lst = lste->get_expressions();
      
      for(size_t i=0;i<lst.size();i++){
        visitor.traverse(lst[i],att);
      }
    }

    lste = isSgAttributeSpecificationStatement(n)->get_bind_list();
    if( lste) {
      SgExpressionPtrList& lst = lste->get_expressions();
      
      for(size_t i=0;i<lst.size();i++){
        visitor.traverse(lst[i],att);
      }
    }

    SgDataStatementGroupPtrList & lst 
      = isSgAttributeSpecificationStatement(n)->get_data_statement_group_list();
    for(size_t i=0;i<lst.size();i++){
        visitor.traverse(lst[i],att);
    }


    SgStringList & slst =isSgAttributeSpecificationStatement(n)->get_name_list();
      string s;
    for(size_t i=0;i<slst.size();i++){
      s = slst[i];
      SgStringVal *sv = SageBuilder::buildStringVal(s);
      visitor.traverse(sv,att);
    }
  }

  else if(isSgDataStatementGroup(n)){ 
    SgDataStatementObjectPtrList & lst =
      isSgDataStatementGroup(n)->get_object_list();
    for(size_t i=0;i<lst.size();i++)
      visitor.traverse(lst[i]->get_variableReference_list(),att);

    SgDataStatementValuePtrList & val =
      isSgDataStatementGroup(n)->get_value_list();
    for(size_t i=0;i<val.size();i++)
      visitor.traverse(val[i]->get_initializer_list(),att);
  }

  else if(isSgSizeOfOp(n)){ 
    string s = n->class_name();
    if( s == "SgSizeOfOp" ){
      SgType* typ = isSgSizeOfOp(n)->get_operand_type();
      if( typ )
        writeTypesRecursive( sstr,typ,att,true );
    }
  }

  else if(isSgFormatStatement(n)){
    SgFormatItemPtrList & lst =
      isSgFormatStatement(n)->get_format_item_list()->get_format_item_list();
    for(size_t i=0;i<lst.size();i++)
      visitor.traverse(lst[i],att);
  }

  else if(isSgInterfaceBody(n)){  
    visitor.traverse(isSgInterfaceBody(n)->get_functionDeclaration(),att);
  }

  else if(isSgArithmeticIfStatement(n)){ 
    visitor.traverse(isSgArithmeticIfStatement(n)->get_less_label(),att);
    visitor.traverse(isSgArithmeticIfStatement(n)->get_equal_label(),att);
    visitor.traverse(isSgArithmeticIfStatement(n)->get_greater_label(),att);
  }

  else if(isSgNamelistStatement(n)){ 
    SgNameGroupPtrList & grp = isSgNamelistStatement(n)->get_group_list();
    for(size_t i=0;i<grp.size();i++) {
      SgNameGroup* nam = isSgNameGroup(grp[i]);
      SgStringList & nl = nam->get_name_list();
      string s;
      for(size_t j=0;j<nl.size();j++) {
          s = nl[j];
          //printf( "i=%d (%s)\n", j,s.c_str());
          SgStringVal *sv = SageBuilder::buildStringVal(s);
        visitor.traverse(sv,att);
      }
    }
  }

  else if(isSgPointerDerefExp(n)){ 
    writeTypesRecursive( sstr,isSgPointerDerefExp(n)->get_type(),att,false );
  }

  else if(isSgVarArgOp(n)){ 
    writeTypesRecursive( sstr,isSgVarArgOp(n)->get_expression_type(),att,false );
  }

  else if(isSgEquivalenceStatement(n)){ 
    visitor.traverse(isSgEquivalenceStatement(n)->get_equivalence_set_list(),att);
  }

  else if(isSgFunctionParameterTypeList(n)){ 
    SgTypePtrList & lst = isSgFunctionParameterTypeList(n)->get_arguments();
    for(size_t i=0;i<lst.size();i++)
      visitor.traverse(lst[i],att);
  }

  else if(isSgInquireStatement(n)){ 
    SgInquireStatement* inq = isSgInquireStatement(n);
    TRAVERSE_IF_EXISTS(inq->get_iolengthExp());
    TRAVERSE_IF_EXISTS(inq->get_unit());
    TRAVERSE_IF_EXISTS(inq->get_iostat());
    TRAVERSE_IF_EXISTS(inq->get_err());
    TRAVERSE_IF_EXISTS(inq->get_iomsg());
    TRAVERSE_IF_EXISTS(inq->get_file());
    TRAVERSE_IF_EXISTS(inq->get_access());
    TRAVERSE_IF_EXISTS(inq->get_form());
    TRAVERSE_IF_EXISTS(inq->get_recl());
    TRAVERSE_IF_EXISTS(inq->get_blank());
    TRAVERSE_IF_EXISTS(inq->get_exist());
    TRAVERSE_IF_EXISTS(inq->get_opened());
    TRAVERSE_IF_EXISTS(inq->get_number());
    TRAVERSE_IF_EXISTS(inq->get_named());
    TRAVERSE_IF_EXISTS(inq->get_name());
    TRAVERSE_IF_EXISTS(inq->get_sequential());
    TRAVERSE_IF_EXISTS(inq->get_direct());
    TRAVERSE_IF_EXISTS(inq->get_formatted());
    TRAVERSE_IF_EXISTS(inq->get_unformatted());
    TRAVERSE_IF_EXISTS(inq->get_nextrec());
    TRAVERSE_IF_EXISTS(inq->get_position());
    TRAVERSE_IF_EXISTS(inq->get_action());
    TRAVERSE_IF_EXISTS(inq->get_read());
    TRAVERSE_IF_EXISTS(inq->get_write());
    TRAVERSE_IF_EXISTS(inq->get_readwrite());
    TRAVERSE_IF_EXISTS(inq->get_delim());
    TRAVERSE_IF_EXISTS(inq->get_pad());
    TRAVERSE_IF_EXISTS(inq->get_asynchronous());
    TRAVERSE_IF_EXISTS(inq->get_decimal());
    TRAVERSE_IF_EXISTS(inq->get_stream());
    TRAVERSE_IF_EXISTS(inq->get_size());
    TRAVERSE_IF_EXISTS(inq->get_pending());
  }
  else if(isSgTypedefDeclaration(n)){
      writeTypesRecursive(sstr,isSgTypedefDeclaration(n)->get_base_type(), att,false);
  }

  return;
}


/* --- AST preprocessing (called before going down to the child nodes) --- */
Ast2XmlInheritedAttribute 
Ast2XmlVisitorInternal::evaluateInheritedAttribute(SgNode* node, 
						   Ast2XmlInheritedAttribute att)
{
  Ast2XmlInheritedAttribute retatt(att.opt);
  SgLocatedNode* loc = isSgLocatedNode(node);
  AttachedPreprocessingInfoType* info=0; 

  if(loc) 
    info = loc->getAttachedPreprocessingInfo();
  if(isSgSourceFile(node)) 
    outLang_ = isSgSourceFile(node)->get_outputLanguage();
  if(info && outLang_==SgFile::e_Fortran_output_language) 
    writeFortranPragma(sstr_,info);

  for(int i(0);i<att.level;i++)
    sstr_ << "  "; // indent
  retatt.level = att.level+1;
  sstr_ << '<';
  sstr_ << node->class_name();  
  if(att.opt->address){
    sstr_ << " address=\"";
    sstr_.setf(ios::hex,ios::basefield);
    sstr_ << node << "\"";
    sstr_.unsetf(ios::hex);
  }
  /* write attributes of this element */
  writeXmlAttribs(sstr_,node,att.opt);

  if (isLeafNode(node) && info == 0)
    sstr_ << "/>" << endl;
  else
    sstr_ << '>' << endl;

  writeTypes(sstr_,node,retatt);
  writeInternalNode(sstr_,node,retatt);

  return retatt;
}


/* --- AST postprocessing (called after coming back from the child nodes) --- */
void Ast2XmlVisitorInternal::destroyInheritedValue (SgNode* node,
						    Ast2XmlInheritedAttribute att)
{
  AttachedPreprocessingInfoType* info=writePreprocessingInfo(sstr_,node); 
  if(info && outLang_==SgFile::e_Fortran_output_language) 
    writeFortranPragma(sstr_,info,PreprocessingInfo::inside);

  if ( isLeafNode(node) == false || info != 0) {
    for(int i(0);i<att.level-1;i++)
      sstr_ << ' ' << ' '; // indent
    sstr_ << "</";
    sstr_ << node->class_name() << '>' << endl;
  }

  if(info && outLang_==SgFile::e_Fortran_output_language) 
    writeFortranPragma(sstr_,info,PreprocessingInfo::after);

  return;
}


