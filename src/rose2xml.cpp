/**
 * @file     rose2xml.cpp
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

#include "rose2xml.hpp"

using namespace XevXml;

namespace XevXml {

  void XevInitialize(void) { XmlInitialize(); }

  void XevFinalize(void)   { XmlFinalize(); }

  bool XevConvertRoseToXml(std::ostream& sstr, SgProject** prj, XevXmlOption* opt)
  {
    XevSageVisitor visitor;
    if(opt==NULL)
      opt = new XevXmlOption();
    visitor.setXmlOption(opt);

    return visitor.write(sstr,prj);
  }
}


bool XevSageVisitor::write(std::ostream& str, SgProject** prj){
  ostr_ = &str;
  if(prj == 0 || *prj == 0 ){
    XEV_WARN("Invalid SgProject object. Conversion failed.");
    return false;
  }
  SgProject* p = *prj;
  if(p->numberOfFiles() > 1 ){
    // NOTE: only the last file is converted.
    // In the case of using Fortran mod files, this could happen.
  }

#if (ROSE_EDG_MAJOR_VERSION_NUMBER < 4)
  // assuming the previous(stable) version of ROSE
  SgFile* file = &p->get_file(p->numberOfFiles()-1);
#else
  // assuming EDG4X-ROSE
  SgFile* file = &p->get_file(0);
#endif
  //visitor.traverseWithinFile(file,help);
  char* enc = getenv(XEV_ENCODE);
  if(enc==0)
    sstr() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  else
    sstr() << "<?xml version=\"1.0\" encoding=\""<< enc << "\"?>" << std::endl;

  //visitor.setSgFileToVisit(file);
  this->visit(file);
  return true; // success
}

static bool hasPreprocessingInfo(SgNode* node)
{
  SgLocatedNode* loc = isSgLocatedNode(node);
  if(loc==NULL) return false;
  AttachedPreprocessingInfoType* info = loc->getAttachedPreprocessingInfo();

  return ( (info!=NULL && info->size()>0) ? true:false);
}

bool XevSageVisitor::hasInode(SgNode* node)
{
  SgType* t = isSgType(node);
  SgExpression* e = isSgExpression(node);
  if(t && t->containsInternalTypes())
    // such as SgArrayTYpe, SgPointerType, ...
    return true;
  if(t && t->get_type_kind())
    return true;
  if(t && t->get_modifiers())
    return true;
  if(t && isSgClassType(t) && isSgClassType(t)->get_autonomous_declaration())
    return true;
  if(t && isSgTypeOfType(t))
    return true;
  if( t==NULL && node->get_numberOfTraversalSuccessors()>0)
    return true;
  if( hasPreprocessingInfo(node) )
    return true;
  if( e && e->get_originalExpressionTree()){
    if(SageInterface::is_Fortran_language()==false)
      return true;
  }
  switch((int)node->variantT()) {
  case V_SgAggregateInitializer:
  case V_SgArithmeticIfStatement:
  case V_SgAttributeSpecificationStatement:
  case V_SgCompoundLiteralExp:
  case V_SgDataStatementGroup:
  case V_SgDataStatementObject:
  case V_SgDataStatementValue:
  case V_SgEquivalenceStatement:
  case V_SgFormatStatement:
  case V_SgFunctionParameterTypeList:
  case V_SgInquireStatement:
  case V_SgNamelistStatement:
  case V_SgPointerDerefExp:
  case V_SgPragma:
  case V_SgSizeOfOp:
  case V_SgStopOrPauseStatement:
  case V_SgTypedefDeclaration:
  case V_SgVarArgOp:
    return true;
  case V_SgTypeComplex:
  case V_SgTypeImaginary:
    //case V_SgTypeOfType:
  case V_SgTypeString:
    return true;
  case V_SgInterfaceBody:
    if(isSgInterfaceBody(node)->get_use_function_name()==false)
      return true;
    else return false;
    break;
  case V_SgFormatItem:
    //if(isSgFormatItem(node)->get_format_item_list())
    return true;
  default:
    return false;
  }
  // never reach here
  return false;
}

static bool isInCppFile(SgNode* node, SgFile* file){
  if(SageInterface::is_Fortran_language()==false)
    return false;

  Sg_File_Info* ninfo = node->get_file_info();
  Sg_File_Info* finfo = file->get_file_info();
  std::string fn = finfo->get_filenameString();
  SgSourceFile* src = isSgSourceFile(file);
  XEV_ASSERT(src!=NULL);

  std::string cppFile
    = src->generate_C_preprocessor_intermediate_filename(fn);
  if(ninfo->get_filenameString() == fn ) return true;
  if(ninfo->get_filenameString() == cppFile ) return true;
  return false;
}

/* check if the node is in the target file.
   See SgTreeTraversal_inFileToTraverse() in AstProcessing.h
*/
static bool isInSameFile(SgNode* node, SgFile* file){
  Sg_File_Info* info = node->get_file_info();
  if(info==NULL){

    if (isSgType(node) || isSgSupport(node) )
      return true;
    if(isSgProject(node)|| isSgAsmNode(node) )
      return true;
    else {
      SageInterface::dumpInfo(node);
      //XEV_ABORT();
      XEV_FATAL("unexpected kind of SgNode found");
    }
  }
  bool isFrontendSpecific = info->isFrontendSpecific();
  bool isCompilerGenerated = false;
  if(isFrontendSpecific==false)
    isCompilerGenerated = info->isCompilerGenerated()
      || info->isTransformation()
      || info->isOutputInCodeGeneration();
      // the following is needed for index of SgArrayType
  //|| info->isSourcePositionUnavailableInFrontend();
  bool isCode = false;
  if(SageInterface::is_Fortran_language()==false){
    isCode = node->get_parent() != NULL
      && !isSgGlobal(node->get_parent())
      && !isSgNamespaceDefinitionStatement(node->get_parent());
  }
  bool isRightFile = info->isSameFile(file) || isInCppFile(node,file);
#if 0
  if(isSgGlobal(node->get_parent()) || isSgInitializedName(node)){
    info->display(node->class_name());
    //std::cout << node->get_parent()->class_name() << std::endl;
    if(isCompilerGenerated || isRightFile || isCode)
      std::cout << "  true " << std::endl;
    else
      std::cout << "  false " << std::endl;
  }
#endif
  // isCode is not used to avoid printing the code included by SgFortranIncludeLine
  return isCompilerGenerated || isRightFile || isCode;
  //return isCompilerGenerated || isRightFile ;
}

/* check if the corresponding element needs indentation */
static bool needIndent(SgNode* node)
{
  //if(node->variantT()==V_SgPragma)
  //return false;

  return true;
}

static void visitSuccessors(SgNode *node, XevSageVisitor* visitor)
{
  if(isSgType(node)==NULL && isSgIOStatement(node)==NULL ){
    for(size_t i(0);i<node->get_numberOfTraversalSuccessors();i++){
      SgNode* succ = node->get_traversalSuccessorByIndex(i);
      if(succ!= NULL)
        visitor->visit(succ);
    }
  }
}

void XevSageVisitor::visit(SgNode* node)
{
  if(isSgFile(node)!=NULL)
    this->setSgFileToVisit(isSgFile(node));
  if(node==NULL||isInSameFile(node,this->getSgFileToVisit())==false )
    return;
  if(getXmlOption()->getSkipCompilerGeneratedFlag()){
    Sg_File_Info* info = node->get_file_info();
    if(info && info->isCompilerGenerated()){
      // skip the current node and visit its successors.
      visitSuccessors(node,this);
      return;
    }
  }

  if(getXmlOption()->getFortranPragmaFlag() && isSgGlobal(node) == NULL)
    writeFortranPragma(this,node,PreprocessingInfo::before,this->file_);
  // --- write the element name ---
  if(needIndent(node))
    writeIndent();
  sstr() << "<" << node->class_name();

  // --- write attributes of this node ---
  switch((int)node->variantT()) {
#define SAGE3(x)                                                        \
    case V_Sg##x:                                                       \
      attribSg##x(node);                                                \
      break;
#include "sgnode.hpp"
  default:
    XEV_FATAL("unknown Sage AST node found \"" << node->class_name() << "\"");
  }
  if(isSgBasicBlock(node->get_parent())||isSgGlobal(node->get_parent())){
    Sg_File_Info* info = node->get_file_info();
    if(info && info->isCompilerGenerated()) {
      sstr() << " unparse=\"0\" ";
      //info->display(node->class_name());
    }
  }
  /*
  if(node->get_file_info()
     && node->get_file_info()->isSameFile(this->getSgFileToVisit())==false
     && isInCppFile(node,this->getSgFileToVisit()) == false )
    sstr() << " samefile=\"0\" ";
  */
  //sstr() << " filename=\""<<node->get_file_info()->get_filename() <<"\" ";
  if( hasInode(node) )
    sstr() << ">" << std::endl;
  else {
    sstr() << "/>" << std::endl;
    return;
  }
  depth_  = depth_ + 1;

  // write pragmas within the global scope
  if(getXmlOption()->getFortranPragmaFlag() && isSgGlobal(node) != NULL)
    writeFortranPragma(this,node,PreprocessingInfo::before,this->file_);

  // --- write successor nodes and inodes ---
  visitSuccessors(node,this);
  switch((int)node->variantT()) {
#define SAGE3(x)                                                        \
    case V_Sg##x:                                                       \
      inodeSg##x(node);                                                 \
      break;
#include "sgnode.hpp"
  default:
    XEV_FATAL("unknown Sage AST node found");
  }
  depth_  = depth_ - 1;

  writePreprocessingInfo(sstr(),node);
  if(getXmlOption()->getFortranPragmaFlag())
    writeFortranPragma(this,node,PreprocessingInfo::inside,this->file_);

  if(needIndent(node))
    writeIndent();
  sstr() << "</" << node->class_name() << ">" << std::endl;
  if(getXmlOption()->getFortranPragmaFlag())
    writeFortranPragma(this,node,PreprocessingInfo::after,this->file_);
  return;
}

#if 0
static void writeInode(SgNode* node,  XevConversionHelper* help)
{
  if(hasInode(node)==false) return;

  switch(node->variantT()) {
  case V_SgArithmeticIfStatement:
    inodeSgArithmeticIfStatement(node,help);
    break;
  case V_SgArrayType:
    inodeSgArrayType(node,help);
    break;
  case V_SgAttributeSpecificationStatement:
    inodeSgAttributeSpecificationStatement(node,help);
    break;;
  case V_SgDataStatementGroup:
    inodeSgDataStatementGroup(node,help);
    break;
  case V_SgDataStatementObject:
    inodeSgDataStatementObject(node,help);
    break;
  case V_SgDataStatementValue:
    inodeSgDataStatementValue(node,help);
    break;
  case V_SgEquivalenceStatement:
    inodeSgEquivalenceStatement(node,help);
    break;
  case V_SgFormatStatement:
    inodeSgFormatStatement(node,help);
    break;
  case V_SgFunctionParameterTypeList:
    inodeSgFunctionParameterTypeList(node,help);
    break;
  case V_SgInquireStatement:
    inodeSgInquireStatement(node,help);
    break;
  case V_SgNamelistStatement:
    inodeSgNamelistStatement(node,help);
    break;
  case V_SgPointerDerefExp:
    inodeSgPointerDerefExp(node,help);
    break;
  case V_SgSizeOpType:
    inodeSgSizeOfType(node,help);
    break;
  case V_SgTypedefDeclaration:
    indoeSgTypedefDeclaration(node,help);
    break;
  case V_SgVarArgOp:
    indoeSgVarArgOp(node,help);
    break;
  case V_SgInterfaceBody:
    inodeSgInterfaceBody(node,help);
    break;
  default:
    XEV_ABORT();
  }
}

static bool hasInode(SgNode* node)
{
  switch(node->variantT()) {
  case V_SgArithmeticIfStatement:
  case V_SgArrayType:
  case V_SgAttributeSpecificationStatement:
  case V_SgDataStatementGroup:
  case V_SgDataStatementObject:
  case V_SgDataStatementValue:
  case V_SgEquivalenceStatement:
  case V_SgFormatStatement:
  case V_SgFunctionParameterTypeList:
  case V_SgInquireStatement:
  case V_SgNamelistStatement:
  case V_SgPointerDerefExp:
  case V_SgSizeOpType:
  case V_SgTypedefDeclaration:
  case V_SgVarArgOp:
    return true;
  case V_SgInterfaceBody:
    if(isSgInterfaceBody(node)->get_use_function_name()==false)
      return true;
    return false;
  default:
    return false;
  }
}

static SgType* hasType(SgNode* node)
{
  if(isSgContructorInitializer(node))
    return isSgConstructorInitializer(node)->get_type();
  else if (isSgInitializedName(node))
    return isSgInitializedName(node)->get_typeptr();
  else if (isSgFunctionDeclaration(node)) {
    return isSgFunctionDeclaration(node)->get_type()->get_return_type();
  }
  else if (isSgCastExp(node))
    return isSgCastExp(node)->get_type();
  else if (isSgPointerDeref(node))
    return isSgPointerDeref(node)->get_type();
  return NULL;
}

static bool isLeafNode(SgNode* node)
{
  if(node->numberOfNodesInSubtree() > 1 )
    return false;
  if(hasInternal(node)==true)
    return false;
  if(hasType(node)!=NULL)
    return false;
  return true;
}

bool XevXml::writeXmlElement(SgNode* node,  XevConversionHelper* help)
{
  SgLocatedNode* loc = isSgLocatedNode(node);
  AttachedPreprocessingInfoType* info=0;

  /* user-defined callback function call */
  help->beforeXmlElement(node);

  if(loc)
    info = loc->getAttachedPreprocessingInfo();
  if(isSgSourceFile(node))
    help->setOutputLanguage(isSgSourceFile(node)->get_outputLanguage());
  if(info && help->getOutputLanguage()==SgFile::e_Fortran_output_language)
    if(help->getFortranPragmaFlag()) writeFortranPragma(sstr_,info);

  for(int i(0);i<help->getLevel();i++)
    sstr_ << "  "; // indent
  help->setLevel(help->getLevel()+1);
  sstr_ << '<';
  sstr_ << node->class_name();

  /* user-defined callback function call */
  help->beforeXmlAttribute(node);

  if(help->getAddressFlag()){
    sstr_ << " address=\"";
    sstr_.setf(ios::hex,ios::basefield);
    sstr_ << node << "\"";
    sstr_.unsetf(ios::hex);
  }

  /* write attributes of this element */
  writeXmlAttribs(sstr_,node,help);

  /* user-defined callback function call */
  help->afterXmlAttribute(node);

  if (isLeafNode(node) && info == 0)
    sstr_ << "/>" << endl;
  else
    sstr_ << '>' << endl;

  /* user-defined callback function call */
  help->afterXmlElement(node);

  if( isLeafNode(node)==false ){
    for(int i(0);i<node->get_numberOfTraversalSuccessors();i++){
      SgNode* cld = node->get_traversalSuccessorByIndex(i);
      if( cld != NULL )
        writeXmlElement(cld,help);
    }

    if(loc)
      if( loc->get_file_info()->isCompilerGenerated()==false
          || help->getRemoveParenFlag() == false )
        writeTypes(sstr_,node,help);

    /* write internal nodes as child nodes */
    writeInternalNode(sstr_,node,help);

    help->beforeXmlClosingElement(node);

    if(info && help->getOutputLanguage()==SgFile::e_Fortran_output_language)
      writeFortranPragma(sstr_,info,PreprocessingInfo::inside);

    if ( isLeafNode(node) == false || info != 0) {
      for(int i(0);i<help->getLevel()-1;i++)
        sstr_ << ' ' << ' '; // indent
      sstr_ << "</";
      sstr_ << node->class_name() << '>' << endl;
    }

    if(info && help->getOutputLanguage()==SgFile::e_Fortran_output_language)
      writeFortranPragma(sstr_,info,PreprocessingInfo::after);

    help->setLevel(help->getLevel()-1);

    /* user-defined callback function call */
    help->afterXmlClosingElement(node);
  }
  return;
}

#endif
