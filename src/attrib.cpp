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
#include <xevxml.hpp>
#include "common.hpp"
#include "ast2xml.hpp"
#include "attrib.hpp"

using namespace std;
using namespace XevXML;

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

#define DEFINE_VALUE_AND_STRING(ty)				\
  static void attrib##ty(stringstream& istr,SgNode* node)	\
  {								\
    ty* n = is##ty(node);					\
    if(n) {							\
      istr << " value=\"" << n->get_value() << "\" ";		\
      if(n->get_valueString().size())				\
	istr << " string=\"" << n->get_valueString() << "\" ";	\
    }								\
  }

DEFINE_VALUE_EXP(SgBoolValExp);
DEFINE_VALUE_EXP(SgEnumVal);
DEFINE_VALUE_AND_STRING(SgCharVal);
DEFINE_VALUE_AND_STRING(SgWcharVal);
//DEFINE_VALUE_EXP(SgComplexVal);
DEFINE_VALUE_AND_STRING(SgDoubleVal);
DEFINE_VALUE_AND_STRING(SgFloatVal);
DEFINE_VALUE_AND_STRING(SgIntVal);
DEFINE_VALUE_AND_STRING(SgLongIntVal);
DEFINE_VALUE_AND_STRING(SgLongLongIntVal);
DEFINE_VALUE_AND_STRING(SgLongDoubleVal);
DEFINE_VALUE_AND_STRING(SgShortVal);
//DEFINE_VALUE_EXP(SgStringVal);
DEFINE_VALUE_AND_STRING(SgUnsignedCharVal);
DEFINE_VALUE_AND_STRING(SgUnsignedShortVal);
DEFINE_VALUE_AND_STRING(SgUnsignedIntVal);
DEFINE_VALUE_AND_STRING(SgUnsignedLongVal);
DEFINE_VALUE_AND_STRING(SgUnsignedLongLongIntVal);
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
  //CALL_ATTRIB(SgComplexVal); // deleted (2014.09.25)
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
    istr << " pragma=\"" << XmlStr2Entity(n->get_pragma()) << "\" ";
  }
}

static void attribSgVariableDeclaration(stringstream& istr,SgNode* node)
{
  SgVariableDeclaration* n = isSgVariableDeclaration(node);
  if(n) {
#if 0
    SgStorageModifier m = (n->get_declarationModifier()).get_storageModifier();
    unsigned long mod = 0;
    if(m.isUnknown() )
      mod |= (1U << 0);
    if(m.isStatic())
      mod |= (1U << 1);
    if(m.isExtern())
      mod |= (1U << 2);
    if(m.isAuto())
      mod |= (1U << 3);
    if(m.isRegister())
      mod |= (1U << 4);
    if(m.isTypedef())
      mod |= (1U << 5);
    if(n->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isConst())
      mod |= (1U << 6);
    if(n->get_declarationModifier().get_typeModifier().isAllocatable())
      mod |= (1U << 7);
    if(n->get_declarationModifier().get_typeModifier().isAsynchronous())
      mod |= (1U << 8);
    if(n->get_declarationModifier().get_typeModifier().isIntent_in())
      mod |= (1U << 9);
    if(n->get_declarationModifier().get_typeModifier().isIntent_out())
      mod |= (1U << 10);
    if(n->get_declarationModifier().get_typeModifier().isIntent_inout())
      mod |= (1U << 11);
    if(n->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isVolatile())
      mod |= (1U << 12);
    if(n->get_declarationModifier().get_storageModifier().isExtern())
      mod |= (1U << 13);
    if(n->get_declarationModifier().get_accessModifier().isPublic())
      mod |= (1U << 14);
    if(n->get_declarationModifier().get_accessModifier().isPrivate())
      mod |= (1U << 15);
    if(n->get_declarationModifier().get_typeModifier().isIntrinsic())
      mod |= (1U << 16);
    if(n->get_declarationModifier().get_typeModifier().isOptional())
      mod |= (1U << 17);
    if(n->get_declarationModifier().get_typeModifier().isSave())
      mod |= (1U << 18);
    if(n->get_declarationModifier().get_typeModifier().isTarget())
      mod |= (1U << 19);
    if(n->get_declarationModifier().get_typeModifier().isValue())
      mod |= (1U << 20);
    istr << " modifier=\"" << mod << "\" ";
#endif
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
#if 0
    unsigned long mod = 0U;

    if(m.isUnknown() )
      mod |= (1U<<0);
    if(m.isStatic())
      mod |= (1U<<1);
    if(m.isExtern())
      mod |= (1U<<2);
    if(m.isAuto())
      mod |= (1U<<3);
    if(m.isRegister())
      mod |= (1U<<4);
    if(m.isTypedef())
      mod |= (1U<<5);
    istr << " modifier=\"" << mod << "\" ";
#endif
    istr << " end_name=\"" << n->get_named_in_end_statement() << "\" ";
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
    if(n->get_result_name()->get_name() != n->get_name())
      istr << " result_name=\"" << n->get_result_name()->get_name().getString() << "\" ";
    if(n->get_functionModifier().isPure())
      istr << " pure=\"1\"";
    if(n->get_functionModifier().isElemental())
      istr << " elemental=\"1\"";
    if(n->get_functionModifier().isRecursive())
      istr << " recursive=\"1\"";
  }
}

static void attribSgUseStatement(stringstream& istr,SgNode* node)
{
  SgUseStatement* n = isSgUseStatement(node);
  if(n) {
    istr << " name=" << n->get_name() << " ";
    istr << " only=\"" << n->get_only_option() << "\"";
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

static void attribSgWhileStmt(stringstream& istr,SgNode* node)
{
  SgWhileStmt*      n = isSgWhileStmt(node);
  SgLabelRefExp*    l = 0;

  if(n) {
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
    SgProcedureHeaderStatement* h =
      isSgProcedureHeaderStatement( n->get_symbol()->get_declaration() );
    if(h){
      istr << " subprogram_kind=\"" << h->get_subprogram_kind () << "\" ";
    }
  }
}

// 2014.9.21 deleted
/*
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
*/

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

    istr << " sequence=\"" << n->get_isSequence() << "\" ";
    istr << " private=\"" << n->get_isPrivate() << "\" ";
    istr << " abstract=\"" << n->get_isAbstract() << "\" ";
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

static void attribSgWriteStatement(stringstream& istr,SgNode* node)
{
  SgWriteStatement* n = isSgWriteStatement(node);
  if(n) {
    if( n->get_format() )
      istr << " fmt=\"1\"";
    if( n->get_iostat() )
      istr << " iostat=\"1\"";
    if( n->get_rec() )
      istr << " rec=\"1\"";
    if( n->get_err() )
      istr << " err=\"1\"";
    if( n->get_namelist() )
      istr << " nml=\"1\"";
  }
}

static void attribSgReadStatement(stringstream& istr,SgNode* node)
{
  SgReadStatement* n = isSgReadStatement(node);
  if(n) {
    if( n->get_format() )
      istr << " fmt=\"1\"";
    if( n->get_iostat() )
      istr << " iostat=\"1\"";
    if( n->get_rec() )
      istr << " rec=\"1\"";
    if( n->get_end() )
      istr << " end=\"1\"";
    if( n->get_err() )
      istr << " err=\"1\"";
  }
}

static void attribSgLabelRefExp(stringstream& istr,SgNode* node)
{
  SgLabelRefExp*      n = isSgLabelRefExp(node);

  if(n) {
    istr << " nlabel=\"" << n->get_symbol()->get_numeric_label_value() << "\" ";
    istr << " type=\"" << n->get_symbol()->get_label_type() << "\" ";
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
      istr << " fmt=\"" << XevXML::XmlStr2Entity(v->get_value()) << "\" ";
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
    if( n->get_use_function_name() == true )
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

static void writeFlopsAttribs(stringstream& istr,SgNode* node,
			      XevXML::XevConversionHelper* help)
{
#ifdef XEV_USE_ROSEHPCT
  if(help->rosehpct==0) return;
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

static void attribSgSourceFile(stringstream& istr,SgNode* node)
{
  SgSourceFile*  n = isSgSourceFile(node);  
  if(n) {
    istr << " filename=\"" << n->get_sourceFileNameWithoutPath() << "\"";
    istr << " language=\"" << n->get_outputLanguage() << "\"";
    istr << " format=\"" << n->get_outputFormat() << "\"";
  }
}

static void attribSgRenamePair(stringstream& istr,SgNode* node)
{
  SgRenamePair*  n = isSgRenamePair(node);  
  if(n) {
    istr << " lname=" << n->get_local_name() << "";
    istr << " uname=" << n->get_use_name() << "";
  }
}

static void attribSgBreakStmt(stringstream& istr,SgNode* node)
{
  SgBreakStmt*  n = isSgBreakStmt(node);  
  if(n) {
    if(n->get_do_string_label().size())
      istr << " label=\"" << n->get_do_string_label() << "\"";
  }
}

static void attribSgContinueStmt(stringstream& istr,SgNode* node)
{
  SgContinueStmt*  n = isSgContinueStmt(node);  
  if(n) {
    if(n->get_do_string_label().size())
      istr << " label=\"" << n->get_do_string_label() << "\"";
  }
}

static void attribSgExpression(stringstream& istr,SgNode* node)
{
  SgExpression*  n = isSgExpression(node);  
  if(n==0)return;

  if(n && n->get_need_paren())
    istr << " paren=\"" << n->get_need_paren() << "\"";
  if(n->get_lvalue())
    istr << " lvalue=\"" << n->get_lvalue() << "\"";
}

static void attribSgIfStmt(stringstream& istr,SgNode* node)
{
  SgIfStmt*  n = isSgIfStmt(node);  
  if(n) {
    istr << " end=\"" << n->get_has_end_statement() << "\" ";
    istr << " use=\"" << n->get_use_then_keyword() << "\" ";
  }
}

static void attribSgConstructorInitializer(stringstream& istr, SgNode* node)
{
  SgConstructorInitializer* n = isSgConstructorInitializer(node);
  if(n){
    istr << " need_name=\"" << n->get_need_name() << "\" ";
    istr << " need_qual=\"" << n->get_need_qualifier() << "\" ";
    istr << " need_paren=\"" << n->get_need_parenthesis_after_name() << "\" ";
    istr << " unknown_class=\"" << n->get_associated_class_unknown() << "\" ";
  }

}

static void attribSgDataStatementValue(stringstream& istr, SgNode* node)
{
  SgDataStatementValue* n = isSgDataStatementValue(node);
  if(n){
    istr << " format=\"" << n->get_data_initialization_format() << "\" ";
  }

}

static void attribSgEntryStatement(stringstream& istr, SgNode* node)
{
  SgEntryStatement* n = isSgEntryStatement(node);
  if(n){
    if(n->get_result_name())
      istr << " result_name=" << n->get_result_name()->get_name() << " ";
  }
}

static void attribSgDeclarationStatement(stringstream& istr, SgNode* node)
{
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
  if(modifier.get_storageModifier().get_modifier() != SgStorageModifier::e_default)
    istr << " storage_modifier=\"" <<  modifier.get_storageModifier().get_modifier()<< "\" ";
}

static void attribSgLocatedNode(stringstream& istr, SgNode* node)
{
  SgLocatedNode* loc = isSgLocatedNode(node);
  if(loc){
    istr << " file_info=\"" << loc->get_file_info()->get_file_id()
	 << " " << loc->get_file_info()->get_raw_line()
	 << " " << loc->get_file_info()->get_raw_col() << "\" ";
  }
}

void writeXmlAttribs(stringstream& istr,SgNode* node,
		     XevXML::XevConversionHelper* help)
{

  writeValueAttribs(istr,node);
  writeFlopsAttribs(istr,node,help);

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
  attribSgWhileStmt(istr,node);
  attribSgFunctionRefExp(istr,node);                    
  attribSgPrintStatement(istr,node);                    
  attribSgWriteStatement(istr,node);                    
  attribSgReadStatement(istr,node);                    
  //attribSgSizeOfOp(istr,node);  // 2014.09.21 (deleted)
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
  attribSgRenamePair(istr,node);
  attribSgBreakStmt(istr,node);
  attribSgContinueStmt(istr,node);
  attribSgIfStmt(istr,node);
  attribSgConstructorInitializer(istr,node);
  attribSgDataStatementValue(istr,node);
  attribSgEntryStatement(istr,node);

  attribSgExpression(istr,node);
  attribSgDeclarationStatement(istr,node);
  attribSgLocatedNode(istr,node);
  return;
}
