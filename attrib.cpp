/**
 * @file     attrib.cpp
 * @brief    Writing XML attriutes to be required at reconstruction
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
#include <rose.h>
#include "xevxml.hpp"
#include "ast2xml.hpp"
#include "attrib.hpp"

using namespace std;
using namespace xevxml;

#define CALL_ATTRIB(ty) attrib##ty(istr,node)
/*
  Sg*Val represents a literal data. 
  So we expect that iostream will properly spell out the data. 
*/
#define DEFINE_VALUE_EXP(ty)					\
  static void attrib##ty(stringstream& istr,SgNode* node)	\
  {								\
    ty* n = is##ty(node);					\
    if(n) {							\
      istr << " value=\"" << n->get_value() << "\" ";		\
    }								\
  }

DEFINE_VALUE_EXP(SgBoolValExp);
DEFINE_VALUE_EXP(SgCharVal);
DEFINE_VALUE_EXP(SgWcharVal);
//DEFINE_VALUE_EXP(SgComplexVal);
static void attribSgComplexVal(stringstream& istr,SgNode* node)	
{
  SgComplexVal* n = isSgComplexVal(node);
  SgFloatVal*   f;
  if(n) {
    f = isSgFloatVal( n->get_real_value() );
    istr << " real=\"" << f->get_value() << "\" ";
    f = isSgFloatVal( n->get_imaginary_value() );
    istr << " imaginary=\"" << f->get_value() << "\" ";
  }
}
DEFINE_VALUE_EXP(SgDoubleVal);
DEFINE_VALUE_EXP(SgFloatVal);
DEFINE_VALUE_EXP(SgIntVal);
DEFINE_VALUE_EXP(SgLongIntVal);
DEFINE_VALUE_EXP(SgLongLongIntVal);
DEFINE_VALUE_EXP(SgEnumVal);
DEFINE_VALUE_EXP(SgLongDoubleVal);
DEFINE_VALUE_EXP(SgShortVal);
//DEFINE_VALUE_EXP(SgStringVal);
DEFINE_VALUE_EXP(SgUnsignedCharVal);
DEFINE_VALUE_EXP(SgUnsignedShortVal);
DEFINE_VALUE_EXP(SgUnsignedIntVal);
DEFINE_VALUE_EXP(SgUnsignedLongVal);
DEFINE_VALUE_EXP(SgUnsignedLongLongIntVal);
static void attribSgUnaryOp(stringstream& istr,SgNode* node)	
{								
  SgUnaryOp* n = isSgUnaryOp(node);					
  if(n) {							
    istr << " mode=\"" << n->get_mode() << "\" ";		
  }								
}

static void attribSgStringVal(stringstream& istr,SgNode* node)
{
  SgStringVal*      n = isSgStringVal(node);
  std::string       str;

  if(n) {
    str = n->get_value();
    str = XmlStr2Entity( str );
    istr << " value=\"" << str << "\"";

    if( n->get_usesSingleQuotes() == true )
      istr << " SingleQuote=\"1\" ";
    /*
    else
      istr << " SingleQuote=\"0\" ";
    if( n->get_usesDoubleQuotes() == true )
      istr << " DoubleQuote=\"1\" ";
    else
      istr << " DoubleQuote=\"0\" ";
    */
  }
}


static void writeValueAttribs(stringstream& istr,SgNode* node)
{

  CALL_ATTRIB(SgBoolValExp);
  CALL_ATTRIB(SgCharVal);
  CALL_ATTRIB(SgWcharVal);
  CALL_ATTRIB(SgComplexVal);
  CALL_ATTRIB(SgDoubleVal);
  CALL_ATTRIB(SgFloatVal);
  CALL_ATTRIB(SgIntVal);
  CALL_ATTRIB(SgLongIntVal);
  CALL_ATTRIB(SgLongLongIntVal);
  CALL_ATTRIB(SgEnumVal);
  CALL_ATTRIB(SgLongDoubleVal);
  CALL_ATTRIB(SgShortVal);
  CALL_ATTRIB(SgStringVal);
  CALL_ATTRIB(SgUnsignedCharVal);
  CALL_ATTRIB(SgUnsignedShortVal);
  CALL_ATTRIB(SgUnsignedIntVal);
  CALL_ATTRIB(SgUnsignedLongVal);
  CALL_ATTRIB(SgUnsignedLongLongIntVal);
}


static void attribSgInitializedName(stringstream& istr,SgNode* node)
{
  SgInitializedName* n = isSgInitializedName(node);
  if(n) {
    istr << " name=" << n->get_name();
  }
}

static void attribSgVarRefExp(stringstream& istr,SgNode* node)
{
  SgVarRefExp* n = isSgVarRefExp(node);
  if(n) {
    istr << " name=" << n->get_symbol()->get_name();
  }
}

static void attribSgPragma(stringstream& istr,SgNode* node)
{
  SgPragma* n = isSgPragma(node);
  /* todo: the line should be split into tokens, and written as attriutes */
  if(n) {
    istr << " pragma=\"" << n->get_pragma() << "\" ";
  }
}

static void attribSgVariableDeclaration(stringstream& istr,SgNode* node)
{
  SgVariableDeclaration* n = isSgVariableDeclaration(node);
  if(n) {
    SgStorageModifier m = (n->get_declarationModifier()).get_storageModifier();
    if(m.isUnknown() )
      istr << " modifier=\"unknown\" ";
    else if(m.isStatic())
      istr << " modifier=\"static\" " ;
    else if(m.isExtern())
      istr << " modifier=\"extern\" " ;
    else if(m.isAuto())
      istr << " modifier=\"auto\" " ;
    else if(m.isRegister())
      istr << " modifier=\"register\" ";
    else if(m.isTypedef())
      istr << " modifier=\"typedef\" ";

    else if(n->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isConst())
      istr << " modifier=\"const\" ";
    else if(n->get_declarationModifier().get_typeModifier().isAllocatable())
      istr << " modifier=\"ALLOCATABLE\" ";
    else if(n->get_declarationModifier().get_typeModifier().isAsynchronous())
      istr << " modifier=\"ASYNCHRONOUS\" ";
    else if(n->get_declarationModifier().get_typeModifier().isIntent_in())
      istr << " modifier=\"INTENT(IN)\" ";
    else if(n->get_declarationModifier().get_typeModifier().isIntent_out())
      istr << " modifier=\"INTENT(OUT)\" ";
    else if(n->get_declarationModifier().get_typeModifier().isIntent_inout())
      istr << " modifier=\"INTENT(INOUT)\" ";
    else if(n->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isVolatile())
      istr << " modifier=\"VOLATILE\" ";
    else if(n->get_declarationModifier().get_storageModifier().isExtern())
      istr << " modifier=\"EXTERNAL\" ";
    else if(n->get_declarationModifier().get_accessModifier().isPublic())
      istr << " modifier=\"PUBLIC\" ";
    else if(n->get_declarationModifier().get_accessModifier().isPrivate())
      istr << " modifier=\"PRIVATE\" ";
    else if(n->get_declarationModifier().get_typeModifier().isIntrinsic())
      istr << " modifier=\"INTRINSIC\" ";
    else if(n->get_declarationModifier().get_typeModifier().isOptional())
      istr << " modifier=\"OPTIONAL\" ";
    else if(n->get_declarationModifier().get_typeModifier().isSave())
      istr << " modifier=\"SAVE\" ";
    else if(n->get_declarationModifier().get_typeModifier().isTarget())
      istr << " modifier=\"TARGET\" ";
    else if(n->get_declarationModifier().get_typeModifier().isValue())
      istr << " modifier=\"VALUE\" ";

    SgUnsignedLongVal *bit = n->get_bitfield();
    if( bit )
      istr << " bitfield=\"" << bit->get_value() << "\" ";
  }
}

static void attribSgFunctionDeclaration(stringstream& istr,SgNode* node)
{
  SgFunctionDeclaration* n = isSgFunctionDeclaration(node);
  if(n) {
    SgStorageModifier m = (n->get_declarationModifier()).get_storageModifier();
    istr << " name=" << n->get_name() << " ";
    if(m.isUnknown() )
      istr << " modifier=\"unknown\" ";
    else if(m.isStatic())
      istr << " modifier=\"static\" " ;
    else if(m.isExtern())
      istr << " modifier=\"extern\" " ;  // not get
    else if(m.isAuto())
      istr << " modifier=\"auto\" " ;
    else if(m.isRegister())
      istr << " modifier=\"register\" ";
    else if(m.isTypedef())
      istr << " modifier=\"typedef\" ";
  }
}

static void attribSgGotoStatement(stringstream& istr,SgNode* node)
{
  SgGotoStatement* n = isSgGotoStatement(node);
  if(n) {
    if( n->get_label() )
      istr << " s_label=" << n->get_label()->get_label();

    if( n->get_label_expression() )                 // add (0826)
      istr << " n_label=\"" << n->get_label_expression()->get_numeric_label_value() << "\" ";

  }
}

static void attribSgLabelStatement(stringstream& istr,SgNode* node)
{
  SgLabelStatement* n = isSgLabelStatement(node);

  if(n) {
    if( n->get_numeric_label() )
      istr << " nlabel=" << n->get_numeric_label()->get_name()  << " ";
    istr << " slabel=" << n->get_label() ;
  }
}

static void attribSgProcedureHeaderStatement(stringstream& istr,SgNode* node)
{
  SgProcedureHeaderStatement* n = isSgProcedureHeaderStatement(node);
  if(n) {
    istr << " subprogram_kind=\"" << n->get_subprogram_kind () << "\" ";
  }
}

static void attribSgUseStatement(stringstream& istr,SgNode* node)
{
  SgUseStatement* n = isSgUseStatement(node);
  if(n) {
    istr << " name=" << n->get_name() << " ";
  }
}

static void attribSgFortranIncludeLine(stringstream& istr,SgNode* node)
{
  SgFortranIncludeLine* n = isSgFortranIncludeLine(node);
  if(n) {
    istr << " filename=\"" << (strrchr( (char*)n->get_filename().c_str(), '/')+1) << "\" "; // File Name
  }
}

static void attribSgAttributeSpecificationStatement(stringstream& istr,SgNode* node)
{
  SgAttributeSpecificationStatement* n = isSgAttributeSpecificationStatement(node);

  if(n) {
    SgAttributeSpecificationStatement::attribute_spec_enum kind =
                                                        n->get_attribute_kind();
    istr << " kind=\"" << kind << "\" ";
    if( n->get_bind_list() )
      istr << " bind_list=\"1\" ";
    else
      istr << " bind_list=\"0\" ";
  }
}

static void attribSgFortranDo(stringstream& istr,SgNode* node)
{
  SgFortranDo*      n = isSgFortranDo(node);
  SgLabelRefExp*    l = 0;
  if(n) {
    istr << " style=\"" << n->get_old_style() << "\" ";
    istr << " end=\"" << n->get_has_end_statement() << "\" ";
    l = n->get_end_numeric_label();
    if(l){
      istr << " nlabel=\"" << l->get_numeric_label_value() << "\" ";
      istr << " slabel=\"\" ";
    }
    else {
      istr << " nlabel=\"\" ";
      istr << " slabel=\"" << n->get_string_label() << "\" ";
    }
  }
}

static void attribSgFunctionRefExp(stringstream& istr,SgNode* node)
{
  SgFunctionRefExp*      n = isSgFunctionRefExp(node);

  if(n) {
    istr << " symbol=" << n->get_symbol()->get_name() << " ";
  }
}

static void attribSgSizeOfOp(stringstream& istr,SgNode* node)
{
  SgSizeOfOp*      n = isSgSizeOfOp(node);

  string s = node->class_name();
  if( s != "SgSizeOfOp" )
    return;

  if(n) {
    if( n->get_operand_type() )
      istr << " type=\"" << n->get_operand_type()->class_name() << "\"";
    else
    istr << " type=\"\"";
  }
  else
    istr << " type=\"\"";
}

static void attribSgClassDeclaration(stringstream& istr,SgNode* node)
{
  SgClassDeclaration*      n = isSgClassDeclaration(node);

  if(n) {
    if( n->get_isUnNamed() )
      istr << " tag_name=\"\" ";
    else
      istr << " tag_name=" << n->get_name() << " ";
    istr << " type=\"" << n->get_class_type() << "\" ";
  }
}

static void attribSgClassDefinition(stringstream& istr,SgNode* node)
{
  SgClassDefinition*      n = isSgClassDefinition(node);

  if(n) {
    if( n->get_declaration()->get_isUnNamed() )
      istr << " tag_name=\"\" ";
    else
      istr << " tag_name=" << n->get_declaration()->get_name() << " ";
  }
}

static void attribSgEnumDeclaration(stringstream& istr,SgNode* node)
{
  SgEnumDeclaration*      n = isSgEnumDeclaration(node);

  if(n) {
    if( n->get_isUnNamed() )
      istr << " tag_name=\"\" ";
    else
      istr << " tag_name=" << n->get_name() << " ";
  }
}

static void attribSgTypedefDeclaration(stringstream& istr,SgNode* node)
{
  SgTypedefDeclaration*      n = isSgTypedefDeclaration(node);

  if(n) {
    istr << " tag_name=" << n->get_name() << " ";
  }
}

static void attribSgPrintStatement(stringstream& istr,SgNode* node)
{
  SgPrintStatement*      n = isSgPrintStatement(node);
  std::string       str;

  if(n) {
    if( n->get_format() )
    istr << " fmt=\"" << n->get_format()->get_type()->class_name() << "\"";
  }
}

static void attribSgLabelRefExp(stringstream& istr,SgNode* node)
{
  SgLabelRefExp*      n = isSgLabelRefExp(node);

  if(n) {
    istr << " nlabel=\"" << n->get_symbol()->get_numeric_label_value() << "\" ";
  }
}

static void attribSgFormatStatement(stringstream& istr,SgNode* node)
{
  SgFormatStatement* n = isSgFormatStatement(node);

  if(n) {
    istr << " class=\"" << n->get_format_item_list()->class_name() << "\" ";
  }
}

static void attribSgFormatItem(stringstream& istr,SgNode* node)
{
  SgFormatItem* n = isSgFormatItem(node);

  if(n) {
    SgStringVal* v = isSgStringVal(n->get_data());
    if(v)
      istr << " fmt=\"" << xevxml::XmlStr2Entity(v->get_value()) << "\" ";
    if( v->get_usesSingleQuotes() == true )
      istr << " SingleQuote=\"1\" ";
    else
      istr << " SingleQuote=\"0\" ";
    if( v->get_usesDoubleQuotes() == true )
      istr << " DoubleQuote=\"1\" ";
    else
      istr << " DoubleQuote=\"0\" ";
  }
}

static void attribSgCommonBlockObject(stringstream& istr,SgNode* node)
{
  SgCommonBlockObject* n = isSgCommonBlockObject(node);

  if(n) {
    istr << " name=\"" << n->get_block_name() << "\" ";
  }
}

static void attribSgInterfaceStatement(stringstream& istr,SgNode* node)
{
  SgInterfaceStatement* n = isSgInterfaceStatement(node);

  if(n) {
    istr << " name=" << n->get_name() << " ";
    istr << " type=\"" << n->get_generic_spec() << "\" ";
  }
}

static void attribSgInterfaceBody(stringstream& istr,SgNode* node)
{
  SgInterfaceBody* n = isSgInterfaceBody(node);

  if(n) {
    if( n->get_use_function_name() )
      istr << " fnc_name=\"\" ";
    else
      istr << " fnc_name=" << n->get_function_name() << " ";
  }
}

static void attribSgStopOrPauseStatement(stringstream& istr,SgNode* node)
{
  SgStopOrPauseStatement*      n = isSgStopOrPauseStatement(node);
  
  if(n) {
    istr << " type=\"" << n->get_stop_or_pause () << "\" ";
  }
}

static void attribFortranNumericlabel(stringstream& istr,SgNode* node)
{
  SgStatement*    stmt = isSgStatement(node);
  
  if( stmt ) {
    SgLabelRefExp*  l = stmt->get_numeric_label();
    if(l)
      istr << " s_nlabel=\"" << l->get_numeric_label_value() << "\" ";
  }
}

static void attribSgNamelistStatement(stringstream& istr,SgNode* node)
{
  SgNamelistStatement*    nlst = isSgNamelistStatement(node);
  
  if( nlst ) {
    SgNameGroupPtrList & grp = nlst->get_group_list();
    SgNameGroup* nam = isSgNameGroup(grp[0]);
    istr << " group=\"" << nam->get_group_name() << "\" ";
  }
}

static void attribSgAsmStmt(stringstream& istr,SgNode* node)
{
  SgAsmStmt*      n = isSgAsmStmt(node);
  std::string       str;
  
  if(n) {
    str = n->get_assemblyCode();
    str = XmlStr2Entity( str );
    istr << " volatile=\"" << n->get_isVolatile() << "\"";
    istr << " asm_code=\"" << str << "\"";
  }
}

static void attribSgOpenStatement(stringstream& istr,SgNode* node)
{
  SgOpenStatement*      n = isSgOpenStatement(node);

  if(n) {
    if( n->get_unit() )         istr << " unit=\"1\" ";

    if( n->get_iostat() )       istr << " iostat=\"1\" ";

    if( n->get_err() )          istr << " err=\"1\" ";

    if( n->get_file() )         istr << " file=\"1\" ";

    if( n->get_status() )       istr << " status=\"1\" ";

    if( n->get_access() )       istr << " access=\"1\" ";

    if( n->get_form() )         istr << " form=\"1\" ";

    if( n->get_recl() )         istr << " recl=\"1\" ";

    if( n->get_blank() )        istr << " blank=\"1\" ";

    if( n->get_position() )     istr << " position=\"1\" ";

    if( n->get_action() )       istr << " action=\"1\" ";

    if( n->get_delim() )        istr << " delim=\"1\" ";

    if( n->get_pad() )          istr << " pad=\"1\" ";

    if( n->get_iomsg() )        istr << " iomsg=\"1\" ";

    if( n->get_round() )        istr << " round=\"1\" ";

    if( n->get_sign() )         istr << " sign=\"1\" ";

    if( n->get_asynchronous() ) istr << " asynchronous=\"1\" ";

  }
}

static void attribSgInquireStatement(stringstream& istr,SgNode* node)
{
  SgInquireStatement*      n = isSgInquireStatement(node);
  
  if(n) {
    if( n->get_iolengthExp() )  istr << " iolength=\"1\" ";
    
    if( n->get_unit() )         istr << " unit=\"1\" ";
    
    if( n->get_iostat() )       istr << " iostat=\"1\" ";
    
    if( n->get_err() )          istr << " err=\"1\" ";
    
    if( n->get_iomsg() )        istr << " iomsg=\"1\" ";
    
    if( n->get_file() )         istr << " file=\"1\" ";
    
    if( n->get_access() )       istr << " access=\"1\" ";
    
    if( n->get_form() )         istr << " form=\"1\" ";
    
    if( n->get_recl() )         istr << " recl=\"1\" ";
    
    if( n->get_blank() )        istr << " blank=\"1\" ";
    
    if( n->get_exist() )        istr << " exist=\"1\" ";
    
    if( n->get_opened() )       istr << " opened=\"1\" ";
      
    if( n->get_number() )       istr << " number=\"1\" ";
    
    if( n->get_named() )        istr << " named=\"1\" ";
    
    if( n->get_name() )         istr << " name=\"1\" ";
    
    if( n->get_sequential() )   istr << " sequential=\"1\" ";
    
    if( n->get_direct() )       istr << " direct=\"1\" ";
    
    if( n->get_formatted() )    istr << " formatted=\"1\" ";
    
    if( n->get_unformatted() )  istr << " unformatted=\"1\" ";
      
    if( n->get_nextrec() )      istr << " nextrec=\"1\" ";
      
    if( n->get_position() )     istr << " position=\"1\" ";
    
    if( n->get_action() )       istr << " action=\"1\" ";
    
    if( n->get_read() )         istr << " read=\"1\" ";
    
    if( n->get_write() )        istr << " write=\"1\" ";
      
    if( n->get_readwrite() )    istr << " readwrite=\"1\" ";
    
    if( n->get_delim() )        istr << " delim=\"1\" ";
    
    if( n->get_pad() )          istr << " pad=\"1\" ";
    
    if( n->get_asynchronous() ) istr << " asynchronous=\"1\" ";
    
    if( n->get_stream() )       istr << " decimal=\"1\" ";
    
    if( n->get_stream() )       istr << " stream=\"1\" ";
    
    if( n->get_size() )         istr << " size=\"1\" ";
    
    if( n->get_pending() )      istr << " pending=\"1\" ";
  }
}

void writeFlopsAttribs(stringstream& istr,SgNode* node,
		       xevxml::Ast2XmlOpt* opt)
{
#ifdef XEV_USE_ROSEHPCT
  if(opt->rosehpct==0) return;
  SgLocatedNode * loc = isSgLocatedNode(node);
  if(loc && loc->attributeExists ("PAPI_FP_OPS") ){
    const RoseHPCT::MetricAttr* flops_attr =
      dynamic_cast<const RoseHPCT::MetricAttr *> 
    (loc->getAttribute ("PAPI_FP_OPS"));
    
    if(flops_attr) {
      istr << " flops=";
      istr << const_cast<RoseHPCT::MetricAttr *> (flops_attr)->toString ();
    }
  }
#endif
}

void attribSgSourceFile(stringstream& istr,SgNode* node)
{
  SgSourceFile*  n = isSgSourceFile(node);  
  if(n) {
    istr << " filename=\"" << n->get_sourceFileNameWithoutPath() << "\"";
    istr << " language=\"" << n->get_outputLanguage() << "\"";
    istr << " format=\"" << n->get_outputFormat() << "\"";
  }
}

void writeXmlAttribs(stringstream& istr,SgNode* node,
		     xevxml::Ast2XmlOpt* opt)
{

  writeValueAttribs(istr,node);
  writeFlopsAttribs(istr,node,opt);

  attribSgSourceFile(istr,node);
  attribSgInitializedName(istr,node);
  attribSgVarRefExp(istr,node);
  attribSgPragma(istr,node);
  attribSgVariableDeclaration(istr,node);
  attribSgFunctionDeclaration(istr,node);
  attribSgUnaryOp(istr,node);

  attribSgGotoStatement(istr,node);                     
  attribSgLabelStatement(istr,node);                    
  attribSgProcedureHeaderStatement(istr,node);          
  attribSgUseStatement(istr,node);                      
  attribSgFortranIncludeLine(istr,node);                
  attribSgAttributeSpecificationStatement(istr,node);   
  attribSgFortranDo(istr,node);                         
  attribSgFunctionRefExp(istr,node);                    
  attribSgPrintStatement(istr,node);                    
  attribSgSizeOfOp(istr,node);                          
  attribSgClassDeclaration(istr,node);                  
  attribSgClassDefinition(istr,node);                   
  attribSgEnumDeclaration(istr,node);                   
  attribSgTypedefDeclaration(istr,node);                
  attribSgLabelRefExp(istr,node);                       
  attribSgFormatStatement(istr,node);                   
  attribSgFormatItem(istr,node);                        
  attribSgCommonBlockObject(istr,node);                 
  attribSgInterfaceStatement(istr,node);                
  attribSgInterfaceBody(istr,node);                     
  attribSgStopOrPauseStatement(istr,node);              
  attribFortranNumericlabel(istr,node);                 
  attribSgNamelistStatement(istr,node);                 
  attribSgAsmStmt(istr,node);                           
  attribSgOpenStatement(istr,node);                     
  attribSgInquireStatement(istr,node);                  

  return;
}
