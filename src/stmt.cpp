/**
 * @file     stmt.cpp
 * @brief    Visitor Functions of SgStatement classes.
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
#include <map>

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;
using namespace XevXml;

map<string,int> g_iostmtAttrib;
enum iostmtAttrib {
  IOSTMT_FMT,
  IOSTMT_IOLENGTH,
  IOSTMT_UNIT,
  IOSTMT_IOSTAT,
  IOSTMT_ERR,
  IOSTMT_IOMSG,
  IOSTMT_FILE,
  IOSTMT_ACCESS,
  IOSTMT_FORM,
  IOSTMT_RECL,
  IOSTMT_BLANK,
  IOSTMT_EXIST,
  IOSTMT_OPENED,
  IOSTMT_NUMBER,
  IOSTMT_NAMED,
  IOSTMT_NAME,
  IOSTMT_SEQUENTIAL,
  IOSTMT_DIRECT,
  IOSTMT_FORMATTED,
  IOSTMT_UNFORMATTED,
  IOSTMT_NEXTREC,
  IOSTMT_POSITION,
  IOSTMT_ACTION,
  IOSTMT_READ,
  IOSTMT_WRITE,
  IOSTMT_READWRITE,
  IOSTMT_DELIM,
  IOSTMT_PAD,
  IOSTMT_ASYNCHRONOUS,
  IOSTMT_DECIMAL,
  IOSTMT_STREAM,
  IOSTMT_SIZE,
  IOSTMT_PENDING,
  IOSTMT_STATUS,
  IOSTMT_ROUND,
  IOSTMT_SIGN,
  IOSTMT_ADVANCE,
  IOSTMT_REC,
  IOSTMT_END,
  IOSTMT_EOR,
  IOSTMT_NML
};

static void allocIostmtAttrib(void){
  if(  g_iostmtAttrib.size() > 0 ) return;

  g_iostmtAttrib["fmt"]          = IOSTMT_FMT;      //write,read,print
  g_iostmtAttrib["iolength"]     = IOSTMT_IOLENGTH; // SgInquireStatement
  g_iostmtAttrib["unit"]         = IOSTMT_UNIT;     //write,read
  g_iostmtAttrib["iostat"]       = IOSTMT_IOSTAT;   //write,read
  g_iostmtAttrib["err"]          = IOSTMT_ERR;      //write,read
  g_iostmtAttrib["iomsg"]        = IOSTMT_IOMSG;    //write
  g_iostmtAttrib["file"]         = IOSTMT_FILE;     //write
  g_iostmtAttrib["access"]       = IOSTMT_ACCESS;   //write
  g_iostmtAttrib["form"]         = IOSTMT_FORM;     //write
  g_iostmtAttrib["recl"]         = IOSTMT_RECL;     //write
  g_iostmtAttrib["blank"]        = IOSTMT_BLANK;    //write
  g_iostmtAttrib["exist"]        = IOSTMT_EXIST;
  g_iostmtAttrib["opened"]       = IOSTMT_OPENED;
  g_iostmtAttrib["number"]       = IOSTMT_NUMBER;
  g_iostmtAttrib["named"]        = IOSTMT_NAMED;
  g_iostmtAttrib["name"]         = IOSTMT_NAME;
  g_iostmtAttrib["sequential"]   = IOSTMT_SEQUENTIAL;
  g_iostmtAttrib["direct"]       = IOSTMT_DIRECT;
  g_iostmtAttrib["formatted"]    = IOSTMT_FORMATTED;
  g_iostmtAttrib["unformatted"]  = IOSTMT_UNFORMATTED;
  g_iostmtAttrib["nextrec"]      = IOSTMT_NEXTREC;
  g_iostmtAttrib["position"]     = IOSTMT_POSITION; //write
  g_iostmtAttrib["action"]       = IOSTMT_ACTION; //write
  g_iostmtAttrib["read"]         = IOSTMT_READ;
  g_iostmtAttrib["write"]        = IOSTMT_WRITE;
  g_iostmtAttrib["readwrite"]    = IOSTMT_READWRITE;
  g_iostmtAttrib["delim"]        = IOSTMT_DELIM; //write
  g_iostmtAttrib["pad"]          = IOSTMT_PAD; //write
  g_iostmtAttrib["asynchronous"] = IOSTMT_ASYNCHRONOUS; //write.read
  g_iostmtAttrib["decimal"]      = IOSTMT_DECIMAL;
  g_iostmtAttrib["stream"]       = IOSTMT_STREAM;
  g_iostmtAttrib["size"]         = IOSTMT_SIZE;
  g_iostmtAttrib["pending"]      = IOSTMT_PENDING;
  g_iostmtAttrib["status"]       = IOSTMT_STATUS; // SgCloseStatement
  g_iostmtAttrib["round"]        = IOSTMT_ROUND; // SgWriteStatement
  g_iostmtAttrib["sign"]         = IOSTMT_SIGN; // SgWriteStatement
  g_iostmtAttrib["advance"]      = IOSTMT_ADVANCE; // SgReadStatement
  g_iostmtAttrib["rec"]          = IOSTMT_REC; // SgReadStatement
  g_iostmtAttrib["end"]          = IOSTMT_END; // SgReadStatement
  g_iostmtAttrib["eor"]          = IOSTMT_EOR; // SgReadStatement
  g_iostmtAttrib["nml"]          = IOSTMT_NML; // SgReadStatement

  return;
}

static PreprocessingInfo*
attachRawText(SgLocatedNode* node, const std::string& text,
	      PreprocessingInfo::RelativePositionType position=PreprocessingInfo::before)
{
  PreprocessingInfo* ret = 0;
  PreprocessingInfo::DirectiveType mytype
    = PreprocessingInfo::CpreprocessorCompilerGeneratedLinemarker;
  //PreprocessingInfo::DirectiveType mytype = PreprocessingInfo::RawText; //error
  //PreprocessingInfo::DirectiveType mytype = PreprocessingInfo::LineReplacement; //replace
  ret = new PreprocessingInfo (mytype,text, "transformation-generated", 0, 0, 0, position);
  node->addToAttachedPreprocessingInfo(ret);
  return ret;
}


/// Check if there is a text between two adjacent statement.
/// If such a text exists, it is attached to either of the two statements.
static
void traverseStatementsAndTexts(XevXmlVisitor* vis, xe::DOMNode* node, SgNode* blk)
{
  xe::DOMNode* child=node->getFirstChild();
  SgStatement* prev=NULL;
  std::string remain ="";
  SgBasicBlock* bb = isSgBasicBlock(blk);
  SgGlobal* global = isSgGlobal(blk);

  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgStatement* astchild = isSgStatement(vis->visit(child,blk));
      if(astchild) {
        if(bb) bb->append_statement(isSgStatement(astchild));
        else if (global) global->append_statement(isSgStatement(astchild));
        else
          si::appendStatement (astchild,isSgScopeStatement(blk));
        prev=isSgStatement(astchild);
        if(remain.length()){ //the text is located before the 1st statement
          //si::attachArbitraryText(prev,remain,PreprocessingInfo::before);
          attachRawText(prev,remain,PreprocessingInfo::before);
          remain="";
        }
      }
    }
    else if (child->getNodeType() == xe::DOMNode::TEXT_NODE){
      char* buf = xe::XMLString::transcode(child->getNodeValue());
      std::string tmp = buf;
      xe::XMLString::release(&buf);
      std::string text = "";
      std::string line = "";
      size_t i(0);
      bool flag = false;
      // skip white spaces
      for(i=0;i<tmp.length();i++) {
        if(!isspace(tmp[i])) flag = true;
        line += tmp[i];
        if(tmp[i]=='\n') {
          if(flag)
            text += line;
          line = "";
          flag = false;
        }
      }
      if(flag)
        text += line;
      if(text.length()){
        if(prev)
          //si::attachArbitraryText(prev,text,PreprocessingInfo::after);
          attachRawText(prev,text,PreprocessingInfo::after);
        else // the text is located before the 1st statement
          remain = text;
      }
    }
    child=child->getNextSibling();
  }
  if(remain.length()) // for empty basic block
    //si::attachArbitraryText(isSgLocatedNode(blk),remain,PreprocessingInfo::inside);
    attachRawText(isSgLocatedNode(blk),remain,PreprocessingInfo::inside);
}


void attribSgStatement(std::ostream& str,SgNode* node)
{
  SgStatement*    stmt = isSgStatement(node);

  if( stmt ) {
    SgLabelRefExp*  l = stmt->get_numeric_label();
    if(l)
      str << " label=\"" << l->get_numeric_label_value() << "\" ";
  }
  //attribSgIOStatement(str,node);
}
#define TRAVERSE_IF_EXISTS(n,x) if(x){                  \
    SgActualArgumentExpression* exp =                   \
    sb::buildActualArgumentExpression( n,x );           \
    this->visit(exp);                                   \
    delete exp;                                         \
  }

#define ATTRIB_STMT_DEFAULT(x)                          \
  /** XML attribute writer of Sg##x */                  \
  void XevSageVisitor::attribSg##x(SgNode* node)        \
  {                                                     \
    attribSgStatement(sstr(),node);                     \
    return;                                             \
  }

#define INODE_STMT_DEFAULT(x)                           \
  /** XML internal node writer of Sg##x */              \
  void XevSageVisitor::inodeSg##x(SgNode* node)         \
  {                                                     \
    SgIOStatement*      n = isSgIOStatement(node);      \
    if(n) {                                             \
      allocIostmtAttrib();                              \
      TRAVERSE_IF_EXISTS("unit",  n->get_unit());       \
      TRAVERSE_IF_EXISTS("iostat",n->get_iostat());     \
      TRAVERSE_IF_EXISTS("err",   n->get_err());        \
      TRAVERSE_IF_EXISTS("iomsg", n->get_iomsg());      \
    }                                                   \
    return;                                             \
  }

#define STMT_DEFAULT(x)                         \
  ATTRIB_STMT_DEFAULT(x)                        \
  INODE_STMT_DEFAULT(x)


// ===============================================================================
/// Visitor of a SgAllocateStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgAllocateStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgAllocateStatement*    ret=0;
  SgExprListExp*          exp=0;
  SgExpression*           stt=0;

  ret = new SgAllocateStatement(DEFAULT_FILE_INFO);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( exp==0 )
        exp = isSgExprListExp(astchild);
      else if( stt==0 )
        stt = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_parent(astParent);
  ret->set_expr_list(exp);
  if(stt)
    ret->set_stat_expression(stt);

  return ret;
}
STMT_DEFAULT(AllocateStatement);


// ===============================================================================
/// Visitor of a SgArithmeticIfStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgArithmeticIfStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgArithmeticIfStatement*    ret
    = new SgArithmeticIfStatement(DEFAULT_FILE_INFO);
  SgExpression*               cnd = NULL;
  SgLabelRefExp*              les = NULL;
  SgLabelRefExp*              eql = NULL;
  SgLabelRefExp*              grt = NULL;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      //assuming these stmts appear in this order
      if( cnd==NULL )
        cnd = isSgExpression(astchild);
      else if( les==NULL )
        les = isSgLabelRefExp(astchild);
      else if( eql==NULL )
        eql = isSgLabelRefExp(astchild);
      else if( grt==NULL )
        grt = isSgLabelRefExp(astchild);
    }
  SUBTREE_VISIT_END();

  if(grt==NULL)
    XEV_MISSING_NODES(SgArithmeticIfStatement,4,SgExpression,true);

  ret->set_conditional( cnd );
  ret->set_less_label( les );
  ret->set_equal_label( eql );
  ret->set_greater_label( grt );
  cnd->set_parent( ret );
  les->set_parent( ret );
  eql->set_parent( ret );
  grt->set_parent( ret );

  return ret;
}
ATTRIB_STMT_DEFAULT(ArithmeticIfStatement);
/** XML internal node writer of SgArithmeticIfStatement */
void XevSageVisitor::inodeSgArithmeticIfStatement(SgNode* node)
{
  SgArithmeticIfStatement* stmt = isSgArithmeticIfStatement(node);
  this->visit(stmt->get_less_label());
  this->visit(stmt->get_equal_label());
  this->visit(stmt->get_greater_label());
}

// ===============================================================================
/// Visitor of a SgAssociateStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgAssociateStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgAssociateStatement*   ret=0;
  SgBasicBlock*           body=0;
  SgVariableDeclaration*  decl=0;

  ret = new SgAssociateStatement(DEFAULT_FILE_INFO);
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( body==0 )
        body = isSgBasicBlock(astchild);
      if( decl==0 )
        decl = isSgVariableDeclaration(astchild);
    }
  SUBTREE_VISIT_END();

  if(body==NULL)
    XEV_MISSING_NODE(SgAssociateStatement,SgBasicBlock,true);
  if(decl==NULL)
    XEV_MISSING_NODE(SgAssociateStatement,SgVariableDeclaration,true);

  ret->set_body(body);
  ret->set_variable_declaration(decl);

  return ret;
}
STMT_DEFAULT(AssociateStatement);

// ===============================================================================
/// Visitor of a SgBackspaceStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgBackspaceStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgBackspaceStatement*   ret =
    new SgBackspaceStatement(DEFAULT_FILE_INFO);

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  allocIostmtAttrib();
  ret->set_io_statement(SgIOStatement::e_backspace);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgActualArgumentExpression* arg = isSgActualArgumentExpression(astchild);
      if(arg) {
        std::string   name = arg->get_argument_name().getString();
        if( g_iostmtAttrib.count(name) <= 0 ){
          XEV_FATAL("Unknown attribute \"" << name << "\" ");
        }
        int flg = g_iostmtAttrib[name];

        SgExpression* exp  = arg->get_expression();
        if( exp == 0 ){
          XEV_FATAL("Expression of \"" << name << "\" is null");
        }
        exp->set_parent(ret);
        exp->set_startOfConstruct(DEFAULT_FILE_INFO);

        switch(flg){
        case IOSTMT_UNIT:
          ret->set_unit( exp );
          break;
        case IOSTMT_IOSTAT:
          ret->set_iostat( exp );
          break;
        case IOSTMT_ERR:
          ret->set_err( exp );
          break;
        case IOSTMT_IOMSG:
          ret->set_iomsg( exp );
          break;
        default:
          XEV_FATAL("Invalid attribute \"" << name << "\" ");
          break;
        }
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
STMT_DEFAULT(BackspaceStatement);

// ===============================================================================
/// Visitor of a SgBasicBlock element in an XML document
SgNode*
XevXmlVisitor::visitSgBasicBlock(xe::DOMNode* node, SgNode* astParent)
{
  SgBasicBlock* ret = sb::buildBasicBlock();
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  sb::pushScopeStack(ret);
  traverseStatementsAndTexts(this, node,ret);
  sb::popScopeStack();
  return ret;
}
STMT_DEFAULT(BasicBlock);

// ===============================================================================
/// Visitor of a SgBreakStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgBreakStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  string do_label;
  SgBreakStmt* ret = sb::buildBreakStmt();
  XEV_ASSERT(ret!=NULL);
  if(XmlGetAttributeValue(node,"slabel",&do_label)){
    ret->set_do_string_label(do_label);
  }

  return ret;
}
/** XML attribute writer of SgBreakStmt */
void XevSageVisitor::attribSgBreakStmt(SgNode* node)
{
  SgBreakStmt*  n = isSgBreakStmt(node);
  if(n) {
    if(n->get_do_string_label().size())
      sstr() << " slabel=\"" << n->get_do_string_label() << "\"";
  }
  attribSgStatement(sstr(),node);
}
INODE_STMT_DEFAULT(BreakStmt);

// ===============================================================================
/// Visitor of a SgCaseOptionStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgCaseOptionStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgCaseOptionStmt*     ret  = new SgCaseOptionStmt(DEFAULT_FILE_INFO);
  SgExpression*         key  = 0;
  SgExpression*         kend = 0;
  SgStatement*          body = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(key==0)
        key = isSgExpression(astchild);
      else if (body==0)
        body = isSgStatement(astchild);
      else if(kend==0)
	kend = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  if(key==NULL)
    XEV_MISSING_NODE(SgCaseOptionStmt,SgExpression,true);
  if(body==NULL)
    XEV_MISSING_NODE(SgCaseOptionStmt,SgStatement,true);

  //ret = sb::buildCaseOptionStmt(key,body);
  ret->set_key (key);
  ret->set_body(body);
  key ->set_parent(ret);
  body->set_parent(ret);
  string c;
  if(XmlGetAttributeValue(node,"construct",&c)){
    ret->set_case_construct_name(c);
  }
  if(kend){
    ret->set_key_range_end(kend);
  }
  return ret;
}
/** XML attribute writer of SgCaseOptionStmt */
void XevSageVisitor::attribSgCaseOptionStmt(SgNode* node)
{
  SgCaseOptionStmt*      n = isSgCaseOptionStmt(node);
  if(n){
    if(n->get_case_construct_name().size()){
      sstr() << " construct=\""<<n->get_case_construct_name()<<"\" ";
    }
  }
  attribSgStatement(sstr(),node);
}
INODE_STMT_DEFAULT(CaseOptionStmt);

// ===============================================================================
/// Visitor of a SgClassDefinition element in an XML document
SgNode*
XevXmlVisitor::visitSgClassDefinition(xercesc::DOMNode* node, SgNode* astParent)
{
  SgClassDefinition*        ret = 0;
  SgClassDeclaration*       dec = isSgClassDeclaration( astParent );
  SgDeclarationStatement*   fld = 0;
  std::vector<SgInterfaceStatement*> iflst;
  int seq=0;
  int pri=0;
  int abs=0;

  XmlGetAttributeValue(node,"sequence",&seq);
  XmlGetAttributeValue(node,"private",&pri);
  XmlGetAttributeValue(node,"abstract",&abs);
  if(dec==0){
    XEV_DEBUG_INFO(node);
    XEV_FATAL("SgClassDefinition's parent must to be SgClassDeclaration");
    //XEV_ABORT();
  }
  ret = sb::buildClassDefinition( dec );
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  sb::pushScopeStack(ret);

  if(si::is_Fortran_language())
    ret->setCaseInsensitive(true);

  ret->set_isSequence(seq);
  ret->set_isPrivate(pri);
  ret->set_isAbstract(abs);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      fld = isSgDeclarationStatement(astchild);
      if( fld ) {
        ret->append_member(fld);
        fld->set_parent(ret);
        SgInterfaceStatement* ifs = isSgInterfaceStatement(fld);
        if(ifs)
          iflst.push_back(ifs);
      }
    }
  SUBTREE_VISIT_END();

  if(si::is_Fortran_language() && iflst.size() > 0 ){
    std::vector<SgRenameSymbol*> rsym;
    for(size_t ii(0);ii<iflst.size();ii++){
      SgInterfaceStatement* ifs = iflst[ii];
      SgInterfaceBodyPtrList &lst = ifs->get_interface_body_list();

      for(size_t i(0);i<lst.size();i++){
        if(lst[i]->get_functionDeclaration()==NULL){
          SgSymbol* sym = ret->first_any_symbol();
          while(sym){
            SgFunctionSymbol* fsym = isSgFunctionSymbol(sym);
            if(fsym){
              SgFunctionDeclaration* fdecl = isSgFunctionDeclaration(fsym->get_declaration());
              if(fdecl && fdecl->get_name() == lst[i]->get_function_name())
                lst[i]->set_functionDeclaration(fdecl);
              rsym.push_back(new SgRenameSymbol(fdecl,fsym,ifs->get_name()));
            }
            sym = ret->next_any_symbol();
          }
        }
      }
    }
    for(size_t i(0);i<rsym.size();i++){
      ret->insert_symbol(rsym[i]->get_name(),rsym[i]);
    }
    //ret->get_symbol_table()->print();
  }
  sb::popScopeStack();

  return ret;
}
/** XML attribute writer of SgClassDefinition */
void XevSageVisitor::attribSgClassDefinition(SgNode* node)
{
  SgClassDefinition*      n = isSgClassDefinition(node);

  // the following attributes will be required only for Fortran
  if(n && si::is_Fortran_language()==true) {
    /*
    if( n->get_declaration()->get_isUnNamed() )
      sstr() << " tag_name=\"\" ";
    else
      sstr() << " tag_name=" << n->get_declaration()->get_name() << " ";
    */
    sstr() << " sequence=\"" << n->get_isSequence() << "\" ";
    sstr() << " private=\"" << n->get_isPrivate() << "\" ";
    sstr() << " abstract=\"" << n->get_isAbstract() << "\" ";
  }
  attribSgStatement(sstr(),node);
}
INODE_STMT_DEFAULT(ClassDefinition);

// ===============================================================================
/// Visitor of a SgCloseStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgCloseStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgCloseStatement*     ret = new SgCloseStatement(DEFAULT_FILE_INFO);

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  allocIostmtAttrib();
  ret->set_io_statement(SgIOStatement::e_close);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgActualArgumentExpression* arg = isSgActualArgumentExpression(astchild);
      if(arg) {
        std::string   name = arg->get_argument_name().getString();
        if( g_iostmtAttrib.count(name) <= 0 ){
          XEV_FATAL("Unknown attribute \"" << name << "\" ");
        }
        int flg = g_iostmtAttrib[name];

        SgExpression* exp  = arg->get_expression();
        if( exp == 0 ){
          XEV_FATAL("Expression of \"" << name << "\" is null");
        }
        exp->set_parent(ret);
        exp->set_startOfConstruct(DEFAULT_FILE_INFO);

        switch(flg){
        case IOSTMT_UNIT:
          ret->set_unit( exp );
          break;
        case IOSTMT_IOSTAT:
          ret->set_iostat( exp );
          break;
        case IOSTMT_ERR:
          ret->set_err( exp );
          break;
        case IOSTMT_IOMSG:
          ret->set_iomsg( exp );
          break;
        case IOSTMT_STATUS:
          ret->set_status( exp );
          break;
        default:
          XEV_FATAL("Invalid attribute \"" << name << "\" ");
          break;
        }
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
/** XML attribute writer of SgCloseStatement */
ATTRIB_STMT_DEFAULT(CloseStatement);

/** XML attribute writer of SgCloseStatement */
void XevSageVisitor::inodeSgCloseStatement(SgNode* node)
{
  SgCloseStatement*      n = isSgCloseStatement(node);

  if(n) {
    allocIostmtAttrib();
    TRAVERSE_IF_EXISTS("unit",  n->get_unit());
    TRAVERSE_IF_EXISTS("iostat",n->get_iostat());
    TRAVERSE_IF_EXISTS("err",   n->get_err());
    TRAVERSE_IF_EXISTS("err",   n->get_iomsg());
    TRAVERSE_IF_EXISTS("status",n->get_status());
  }
}


// ===============================================================================
/// Visitor of a SgComputedGotoStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgComputedGotoStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgExprListExp*          exp = 0;
  SgExpression*           var = 0;
  SgComputedGotoStatement*    ret =0;

  SUBTREE_VISIT_BEGIN(node,astchild,astParent)
    {
      if( exp==0 )
        exp = isSgExprListExp(astchild);
      else if( var==0 )
        var = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  if(exp==NULL)
    XEV_MISSING_NODE(SgComputedGotoStatement,SgExprListExp,true);
  if(var==NULL)
    XEV_MISSING_NODE(SgComputedGotoStatement,SgExpression,true);

  ret = new SgComputedGotoStatement(DEFAULT_FILE_INFO, exp,var );
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  exp->set_parent(ret);
  var->set_parent(ret);

  return ret;
}
STMT_DEFAULT(ComputedGotoStatement);

// ===============================================================================
/// Visitor of a SgContinueStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgContinueStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  string do_label;
  SgContinueStmt* ret = sb::buildContinueStmt();

  if(XmlGetAttributeValue(node,"slabel",&do_label)){
    ret->set_do_string_label(do_label);
  }

  return ret;
}
/** XML attribute writer of SgContinueStmt */
void XevSageVisitor::attribSgContinueStmt(SgNode* node)
{
  SgContinueStmt*  n = isSgContinueStmt(node);
  if(n) {
    if(n->get_do_string_label().size())
      sstr() << " slabel=\"" << n->get_do_string_label() << "\"";
  }
  attribSgStatement(sstr(),node);
}
INODE_STMT_DEFAULT(ContinueStmt);

// ===============================================================================
/// Visitor of a SgDeallocateStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgDeallocateStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDeallocateStatement*    ret=0;
  SgExprListExp*          exp=0;
  SgExpression*           stt=0;

  ret = new SgDeallocateStatement(DEFAULT_FILE_INFO);
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( exp==0 )
        exp = isSgExprListExp(astchild);
      else if( stt==0 )
        stt = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  //printf( "ret=%p,exp=%p\n",ret,exp);
  ret->set_expr_list(exp);
  if(stt)
    ret->set_stat_expression(stt);
  //exp->set_parent(ret);

  return ret;
}
STMT_DEFAULT(DeallocateStatement);

// ===============================================================================
/// Visitor of a SgDefaultOptionStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgDefaultOptionStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgStatement*              body = 0;
  SgDefaultOptionStmt*      ret  = new SgDefaultOptionStmt(DEFAULT_FILE_INFO);

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if (body==NULL)
        body = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  if(body==NULL)
    XEV_MISSING_NODE(SgDefaultOptionStmt,SgStatement,true);
  ret->set_body(body);

  string c;
  if(XmlGetAttributeValue(node,"construct",&c)){
    ret->set_default_construct_name(c);
  }
  return ret;
}
/** XML attribute writer of SgDefaultOptionStmt */
void XevSageVisitor::attribSgDefaultOptionStmt(SgNode* node)
{
  SgDefaultOptionStmt* n = isSgDefaultOptionStmt(node);
  if(n){
    if(n->get_default_construct_name().size()){
      sstr() << " construct=\"" << n->get_default_construct_name() << "\" ";
    }
  }
  attribSgStatement(sstr(),node);
}
INODE_STMT_DEFAULT(DefaultOptionStmt);

// ===============================================================================
/// Visitor of a SgDoWhileStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgDoWhileStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  //SgDoWhileStmt*        ret  = 0;
  SgDoWhileStmt*        ret  = new SgDoWhileStmt(DEFAULT_FILE_INFO);
  SgStatement*          body = 0;
  SgExprStatement*      cond = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      /* assuming these stmts appear in this order */
      if(body==0)
        body = isSgStatement(astchild);
      else if (cond==0)
        cond = isSgExprStatement(astchild);
    }
  SUBTREE_VISIT_END();
  //ret = sb::buildDoWhileStmt(body,cond);
  if(body==NULL)
    XEV_MISSING_NODE(SgDoWhileStmt,SgStatement,true);
  if(cond==NULL)
    XEV_MISSING_NODE(SgDoWhileStmt,SgExprStatement,true);

  ret->set_body(body);
  ret->set_condition(cond);
  body->set_parent(ret);
  cond->set_parent(ret);

  return ret;
}
STMT_DEFAULT(DoWhileStmt);

// ===============================================================================
/// Visitor of a SgElseWhereStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgElseWhereStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgElseWhereStatement* ret =
    new SgElseWhereStatement(DEFAULT_FILE_INFO);
  SgExpression*         cond  = 0;
  SgBasicBlock*         body  = 0;
  SgElseWhereStatement* elsew  = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(cond==0)
        cond = isSgExpression(astchild);
      else if (body==0)
        body = isSgBasicBlock(astchild);
      else if (elsew==0)
        elsew = isSgElseWhereStatement(astchild);
    }
  SUBTREE_VISIT_END();

  if(body==NULL)
    XEV_MISSING_NODE(SgDoElseWhileStmt,SgBasicBlock,true);
  if(cond==NULL)
    XEV_MISSING_NODE(SgDoElseWhileStmt,SgExpression,true);

  ret->set_condition( cond );
  ret->set_body( body );
  if(elsew!=NULL){
    ret->set_elsewhere(elsew);
    elsew->set_parent(ret);
  }
  return ret;
}
STMT_DEFAULT(ElseWhereStatement);

// ===============================================================================
/// Visitor of a SgEndfileStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgEndfileStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgEndfileStatement*     ret =
    new SgEndfileStatement(DEFAULT_FILE_INFO);

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  allocIostmtAttrib();
  ret->set_io_statement(SgIOStatement::e_endfile);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgActualArgumentExpression* arg = isSgActualArgumentExpression(astchild);
      if(arg) {
        std::string   name = arg->get_argument_name().getString();
        if( g_iostmtAttrib.count(name) <= 0 ){
          XEV_FATAL("Unknown attribute \"" << name << "\" ");
        }
        int flg = g_iostmtAttrib[name];

        SgExpression* exp  = arg->get_expression();
        if( exp == 0 ){
          XEV_FATAL("Expression of \"" << name << "\" is null");
        }
        exp->set_parent(ret);
        exp->set_startOfConstruct(DEFAULT_FILE_INFO);

        switch(flg){
        case IOSTMT_UNIT:
          ret->set_unit( exp );
          break;
        case IOSTMT_IOSTAT:
          ret->set_iostat( exp );
          break;
        case IOSTMT_ERR:
          ret->set_err( exp );
          break;
        case IOSTMT_IOMSG:
          ret->set_iomsg( exp );
          break;
        default:
          XEV_FATAL("Invalid attribute \"" << name << "\" ");
          break;
        }
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
STMT_DEFAULT(EndfileStatement);

// ===============================================================================
/// Visitor of a SgExprStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgExprStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgExpression* exp = 0;
  SgExprStatement* ret = sb::buildExprStatement(exp);
  //SgExprStatement* ret = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(exp==0)
        exp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  if(exp==NULL)
    XEV_MISSING_NODE(SgExprStatement,SgExpression,true);
  ret->set_expression(exp);
  exp->set_parent(ret);

  return ret;
}
STMT_DEFAULT(ExprStatement);

// ===============================================================================
/// Visitor of a SgFlushStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgFlushStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgFlushStatement* ret = new SgFlushStatement(DEFAULT_FILE_INFO);

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  allocIostmtAttrib();
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgActualArgumentExpression*  arg = isSgActualArgumentExpression(astchild);

      if(arg) {
        std::string   name = arg->get_argument_name().getString();
        if( g_iostmtAttrib.count(name) <= 0 ){
          XEV_FATAL("Unknown attribute \"" << name << "\" ");
        }
        int flg = g_iostmtAttrib[name];

        SgExpression* exp  = arg->get_expression();
        if( exp == 0 ){
          XEV_FATAL("Expression of \"" << name << "\" is null");
        }
        exp->set_parent(ret);
        exp->set_startOfConstruct(DEFAULT_FILE_INFO);

        switch(flg){
        case IOSTMT_UNIT:
          ret->set_unit( exp );
          break;
        case IOSTMT_IOSTAT:
          ret->set_iostat( exp );
          break;
        case IOSTMT_ERR:
          ret->set_err( exp );
          break;
        case IOSTMT_IOMSG:
          ret->set_iomsg( exp );
          break;
        default:
          XEV_FATAL("Invalid attribute \"" << name << "\" ");
          break;
        }
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
STMT_DEFAULT(FlushStatement);

// ===============================================================================
/// Visitor of a SgForAllStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgForAllStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForAllStatement*  ret = new SgForAllStatement(DEFAULT_FILE_INFO);
  SgExprListExp*      hed = 0;
  SgBasicBlock*       bdy = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( hed==0 )
        hed = isSgExprListExp(astchild);
      else if( bdy==0 )
        bdy = isSgBasicBlock(astchild);
    }
  SUBTREE_VISIT_END();

  if(hed==NULL)
    XEV_MISSING_NODE(SgForAllStatement,SgExprListExp,true);
  if(bdy==NULL)
    XEV_MISSING_NODE(SgForAllStatement,SgBasicBlock,true);
  hed->set_parent(ret);
  hed->set_startOfConstruct(DEFAULT_FILE_INFO);
  bdy->set_parent(ret);
  bdy->set_startOfConstruct(DEFAULT_FILE_INFO);
  ret->set_forall_header(hed);
  ret->set_body(bdy);
  //ret->set_has_end_statement(true);
  //ret->set_has_end_statement(false);

  if(si::is_Fortran_language()==true)
    bdy->setCaseInsensitive(true);
  return ret;
}
STMT_DEFAULT(ForAllStatement);

// ===============================================================================
/// Visitor of a SgForInitStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgForInitStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForInitStatement* ret = 0;
  SgStatement* stmt  = 0;
  SgStatementPtrList lst;

  SUBTREE_VISIT_BEGIN(node,astchild,astParent)
    {
      if((stmt = isSgStatement(astchild))!=0)
        lst.push_back(stmt);
    }
  SUBTREE_VISIT_END();

  //lst could be empty
  ret = sb::buildForInitStatement(lst);
  XEV_ASSERT(ret!=NULL);
  return ret;
}
STMT_DEFAULT(ForInitStatement);

// ===============================================================================
/// Visitor of a SgForStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgForStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForStatement* ret = new SgForStatement(DEFAULT_FILE_INFO);
  SgForInitStatement*    ini = 0;
  SgStatement*           tst = 0;
  SgExpression*          inc = 0;
  SgStatement*           bdy = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      /* assuming these stmts appear in this order */
      if(ini==0)
        ini = isSgForInitStatement(astchild);
      else if (tst==0)
        tst = isSgStatement(astchild);
      else if (inc==0)
        inc = isSgExpression(astchild);
      else if (bdy==0)
        bdy = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  //ret = sb::buildForStatement(ini,tst,inc,bdy);
  //ret->set_parent(astParent);
  if(ini==NULL||tst==NULL||inc==NULL||bdy==NULL){
    XEV_DEBUG_INFO(node);
    XEV_WARN("SgForStatement needs 4 child nodes in the sequence of "
             "SgForInitStatement, "
             "SgStatement, "
             "SgExpression, and "
             "SgStatement");
  }
  if(ini==NULL)
    XEV_MISSING_NODE(SgForStatement,SgForInitStatement,true);
  if(bdy==NULL)
    XEV_MISSING_NODES(SgForStatement,2,SgStatement,true);
  if(inc==NULL)
    XEV_MISSING_NODE(SgForStatement,SgExpression,true);

  ret->set_for_init_stmt(ini);
  ini->set_parent(ret);
  ret->set_test(tst);
  tst->set_parent(ret);
  ret->set_increment(inc);
  inc->set_parent(ret);
  ret->set_loop_body(bdy);
  bdy->set_parent(ret);

  return ret;
}
STMT_DEFAULT(ForStatement);

// ===============================================================================
/// Visitor of a SgFortranDo element in an XML document
SgNode*
XevXmlVisitor::visitSgFortranDo(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFortranDo*              ret  = 0;
  SgExpression*             ini  = 0;
  SgExpression*             bnd  = 0;
  SgExpression*             inc  = 0;
  SgBasicBlock*             body = 0;

  int                       style = 0;
  int                       enddo = 0;
  //int                       ino   = 0;
  string                    slabel;
  int                       nlabel= 0;
  stringstream              val;

  XmlGetAttributeValue(node,"style",&style);
  XmlGetAttributeValue(node,"end",&enddo);
  XmlGetAttributeValue(node,"slabel",&slabel);
  XmlGetAttributeValue(node,"elabel",&nlabel);

  Sg_File_Info* info = DEFAULT_FILE_INFO;

  ret = new SgFortranDo(info);
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  if(si::is_Fortran_language()==true)
    ret->setCaseInsensitive(true);
  sb::pushScopeStack(ret);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      /* assuming these stmts appear in this order */
      if(ini==0)
        ini = isSgExpression(astchild);
      else if (bnd==0)
        bnd = isSgExpression(astchild);
      else if (inc==0)
        inc = isSgExpression(astchild);
      else if(body==0)
        body = isSgBasicBlock(astchild);
    }
  SUBTREE_VISIT_END();
  sb::popScopeStack();

  //ret = new SgFortranDo( astParent->get_file_info(), ini,bnd,inc,body);
  ret->set_initialization(ini);
  ret->set_bound(bnd);
  ret->set_increment(inc);
  ret->set_body(body);

  ret->set_old_style( style );
  ret->set_has_end_statement( enddo );
  ret->set_parent(astParent);

  if(ini) ini->set_parent(ret);
  if(bnd) bnd->set_parent(ret);
  if(inc) inc->set_parent(ret);
  if(body) body->set_parent(ret);

  // they are special labels of SgFortranDo
  if( slabel.size() )
    ret->set_string_label( slabel );
  if( nlabel != 0 ) {
    SgLabelSymbol*  s = new SgLabelSymbol();
    //s->set_label_type( SgLabelSymbol::label_type_enum.e_non_numeric_label_type );
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    s->get_fortran_statement()->set_parent(s);
    s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    //val.str("");
    //val.clear(iostream::goodbit);
    //val << nlabel;
    //val >> ino;
    s->set_numeric_label_value( nlabel );
    ret->set_end_numeric_label( new SgLabelRefExp( s ) );
  }

  return ret;
}
/** XML attribute writer of SgFortranDo */
void XevSageVisitor::attribSgFortranDo(SgNode* node)
{
  SgFortranDo*      n = isSgFortranDo(node);
  SgLabelRefExp*    l = 0;
  if(n) {
    sstr() << " style=\"" << n->get_old_style() << "\" ";
    sstr() << " end=\"" << n->get_has_end_statement() << "\" ";
    l = n->get_end_numeric_label();
    if(l){
      sstr() << " elabel=\"" << l->get_numeric_label_value() << "\" ";
    }
    if(n->get_string_label().size()) {
      sstr() << " slabel=\"" << n->get_string_label() << "\" ";
    }
  }
  attribSgStatement(sstr(),node);
}
INODE_STMT_DEFAULT(FortranDo);

// ===============================================================================
/// Visitor of a SgFunctionDefinition element in an XML document
SgNode*
XevXmlVisitor::visitSgFunctionDefinition(xe::DOMNode* node, SgNode* astParent)
{
  //SgBasicBlock*   ret   = 0;
  SgBasicBlock* blk = 0;
  Sg_File_Info* info = DEFAULT_FILE_INFO;
  info->setOutputInCodeGeneration();
  SgFunctionDefinition* ret  = new SgFunctionDefinition(info);
#if 0
  SgFunctionDefinition* ret  = 0;
  SgFunctionDeclaration* decl = isSgFunctionDeclaration(astParent);
  if(decl==0 ||decl->get_definition()==0)
    ret = new SgFunctionDefinition(info);
  else
    ret = decl->get_definition();
#endif
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  sb::pushScopeStack(ret);
  if(si::is_Fortran_language())
    ret->setCaseInsensitive(true);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(blk == 0 ){
        blk = isSgBasicBlock(astchild);
      }
    }
  SUBTREE_VISIT_END();
  if(blk){
    ret->set_body(blk);
    blk->set_parent(ret);
  }
  sb::popScopeStack();
  return ret;
}
STMT_DEFAULT(FunctionDefinition);

// ===============================================================================
/// Visitor of a SgGlobal element in an XML document
SgNode*
XevXmlVisitor::visitSgGlobal(xe::DOMNode* node, SgNode* astParent)
{
  Sg_File_Info* info = DEFAULT_FILE_INFO;
  SgGlobal* ret = new SgGlobal(info);
  XEV_ASSERT(ret!=NULL);
  XEV_ASSERT(_file!=NULL);
  _file->set_globalScope(ret);
  ret->set_parent(_file);
  ret->set_file_info(info);
  ret->get_globalFunctionTypeTable()->set_parent(ret);
  if(si::is_Fortran_language())
    ret->setCaseInsensitive(true);

  sb::pushScopeStack(ret);
  traverseStatementsAndTexts(this,node,ret);
  sb::popScopeStack();

  //si::fixVariableReferences(ret);
  //CheckUndeclVars checker;
  //checker.visit(ret);
  //si::fixVariableReferences(ret);
  return ret;
}
STMT_DEFAULT(Global);

// ===============================================================================
/// Visitor of a SgGotoStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgGotoStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgGotoStatement*      ret   = 0;
  SgLabelStatement*     label = 0;
  SgStatement*          body  = 0;
  SgScopeStatement*     scope = 0;
  SgLabelSymbol*        s     = 0;
  SgLabelRefExp*        l     = 0;

  string                s_name, n_name;
  int                   ino = 0;

  if(XmlGetAttributeValue(node,"slabel",&s_name))
    label = sb::buildLabelStatement( s_name.c_str(), body, scope );
  if(XmlGetAttributeValue(node,"nlabel",&n_name)){
    label = sb::buildLabelStatement( n_name.c_str(), body, scope );
    s = new SgLabelSymbol();
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    s->set_label_type(SgLabelSymbol::e_start_label_type);
    s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    ino = atoi(n_name.c_str());
    s->set_numeric_label_value( ino );
    l = new SgLabelRefExp( s );
    s->set_parent(l);
    label->set_numeric_label( l );
  }

  ret = sb::buildGotoStatement( label );
  XEV_ASSERT(ret!=NULL);
  if( n_name.size() ){
    ret->set_label_expression(l);
    //printf( "l=%p\n",l );
    l->set_parent(ret);
    /* */
    /*
    ret= new SgGotoStatement(label);
    //SgLabelRefExp* labelRefExp = new SgLabelRefExp(s);
        ret->set_label_expression(l);
        l->set_parent(ret);
    */
    //printf( "get_label_expression=%p\n",ret->get_label_expression() );
  }

  return ret;
}
/** XML attribute writer of SgGotoStatement */
void XevSageVisitor::attribSgGotoStatement(SgNode* node)
{
  SgGotoStatement* n = isSgGotoStatement(node);
  if(n) {
    if( n->get_label() )
      sstr() << " slabel=" << n->get_label()->get_label();

    if( n->get_label_expression() )                 // add (0826)
      sstr() << " nlabel=\"" << n->get_label_expression()->get_numeric_label_value() << "\" ";

  }
  attribSgStatement(sstr(),node);
}
INODE_STMT_DEFAULT(GotoStatement);

// ===============================================================================
/// Visitor of a SgIfStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgIfStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgIfStmt* ret = 0;
  SgStatement* tstmt  = 0;
  SgStatement* fstmt  = 0;
  SgExprStatement* cond  = 0;
  int estmt = 1;
  int ukey  = 1;

  XmlGetAttributeValue(node,"then",&ukey);
  XmlGetAttributeValue(node,"end",&estmt);

  ret = new SgIfStmt(DEFAULT_FILE_INFO);
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  if(si::is_Fortran_language()==true)
    ret->setCaseInsensitive(true);
  sb::pushScopeStack(ret);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      /* assuming these stmts appear in this order */
      if(cond==0)
        cond = isSgExprStatement(astchild);
      else if (tstmt==0)
        tstmt = isSgStatement(astchild);
      else if (fstmt==0)
        fstmt = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();
  sb::popScopeStack();
  if(cond==NULL)
    XEV_MISSING_NODE(SgIfStmt,SgExprStatement,true);
  if(tstmt==NULL)
    XEV_MISSING_NODE(SgIfStmt,SgStatement,true);
  // fstmt can be NULL

  ret->set_conditional(cond);
  ret->set_true_body(tstmt);
  ret->set_false_body(fstmt);

  ret->set_has_end_statement(estmt);
  ret->set_use_then_keyword(ukey);
  int elabel = 0;
  if(XmlGetAttributeValue(node,"elabel",&elabel)){
    SgLabelSymbol*  s = new SgLabelSymbol();
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    s->get_fortran_statement()->set_parent(s);
    s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    s->set_numeric_label_value( elabel );
    SgLabelRefExp*  l = new SgLabelRefExp( s );
    ret->set_end_numeric_label( l );
  }
  if(XmlGetAttributeValue(node,"ellabel",&elabel)){
    SgLabelSymbol*  s = new SgLabelSymbol();
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    s->get_fortran_statement()->set_parent(s);
    s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    s->set_numeric_label_value( elabel );
    SgLabelRefExp*  l = new SgLabelRefExp( s );
    ret->set_else_numeric_label( l );
  }
  string slabel;
  if(XmlGetAttributeValue(node,"slabel",&slabel)){
    ret->set_string_label(slabel);
  }
  return ret;
}
/** XML attribute writer of SgIfStmt */
void XevSageVisitor::attribSgIfStmt(SgNode* node)
{
  SgIfStmt*  n = isSgIfStmt(node);
  if(n) {
    sstr() << " end=\"" << n->get_has_end_statement() << "\" ";
    sstr() << " then=\"" << n->get_use_then_keyword() << "\" ";
    SgLabelRefExp* l = n->get_end_numeric_label();
    if(l)
      sstr() << " elabel=\"" << l->get_numeric_label_value() << "\" ";
    l = n->get_else_numeric_label();
    if(l)
      sstr() << " ellabel=\"" << l->get_numeric_label_value() << "\" ";
    if(n->get_string_label().size())
      sstr() << " slabel=\"" << n->get_string_label() << "\" ";
  }
  attribSgStatement(sstr(),node);
}
INODE_STMT_DEFAULT(IfStmt);

// ===============================================================================
/// Visitor of a SgInquireStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgInquireStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgInquireStatement* ret = new SgInquireStatement(DEFAULT_FILE_INFO);
  SgExprListExp*               exl = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  allocIostmtAttrib();
  ret->set_io_statement(SgIOStatement::e_inquire);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgActualArgumentExpression* arg = isSgActualArgumentExpression(astchild);
      if(exl==0){
        exl = isSgExprListExp(astchild);
        if(exl){
          exl->set_parent(ret);
          ret->set_io_stmt_list(exl);
        }
      }
      if( arg ){
        std::string name = arg->get_argument_name().getString();

        if( g_iostmtAttrib.count(name) <= 0 ){
          XEV_FATAL("unknown attribute \"" << name << "\" ");
        }
        int flg           = g_iostmtAttrib[name];
        SgExpression* exp = arg->get_expression();
        if( exp == 0 ){
          XEV_FATAL("Expression of \"" << name << "\" is null");
        }
        exp->set_parent(ret);
        exp->set_startOfConstruct(DEFAULT_FILE_INFO);

        switch( flg ){
        case IOSTMT_IOLENGTH:
          if( isSgVarRefExp(exp)) {
            ret->set_iolengthExp(isSgVarRefExp(exp));
          }
          else{
            XEV_MISSING_NODE(SgInquireStatement,SgVarRefExp,true);
          }
        break;
        case IOSTMT_UNIT:
          ret->set_unit( exp );
          break;
        case IOSTMT_IOSTAT:
          ret->set_iostat( exp );
          break;
        case IOSTMT_ERR:
          ret->set_err( exp );
          break;
        case IOSTMT_IOMSG:
          ret->set_iomsg( exp );
          break;
        case IOSTMT_FILE:
          ret->set_file( exp );
          break;
        case IOSTMT_ACCESS:
          ret->set_access( exp );
          break;
        case IOSTMT_FORM:
          ret->set_form( exp );
          break;
        case IOSTMT_RECL:
          ret->set_recl( exp );
          break;
        case IOSTMT_BLANK:
          ret->set_blank( exp );
          break;
        case IOSTMT_EXIST:
          ret->set_exist( exp );
          break;
        case IOSTMT_OPENED:
          ret->set_opened( exp );
          break;
        case IOSTMT_NUMBER:
          ret->set_number( exp );
          break;
        case IOSTMT_NAMED:
          ret->set_named( exp );
          break;
        case IOSTMT_NAME:
          ret->set_name( exp );
          break;
        case IOSTMT_SEQUENTIAL:
          ret->set_sequential( exp );
          break;
        case IOSTMT_DIRECT:
          ret->set_direct( exp );
          break;
        case IOSTMT_FORMATTED:
          ret->set_formatted( exp );
          break;
        case IOSTMT_UNFORMATTED:
          ret->set_unformatted( exp );
          break;
        case IOSTMT_NEXTREC:
          ret->set_nextrec( exp );
          break;
        case IOSTMT_POSITION:
          ret->set_position( exp );
          break;
        case IOSTMT_ACTION:
          ret->set_action( exp );
          break;
        case IOSTMT_READ:
          ret->set_read( exp );
          break;
        case IOSTMT_WRITE:
          ret->set_write( exp );
          break;
        case IOSTMT_READWRITE:
          ret->set_readwrite( exp );
          break;
        case IOSTMT_DELIM:
          ret->set_delim( exp );
          break;
        case IOSTMT_PAD:
          ret->set_pad( exp );
          break;
        case IOSTMT_ASYNCHRONOUS:
          ret->set_asynchronous( exp );
          break;
        case IOSTMT_DECIMAL:
          ret->set_decimal( exp );
          break;
        case IOSTMT_STREAM:
          ret->set_stream( exp );
          break;
        case IOSTMT_SIZE:
          ret->set_size( exp );
          break;
        case IOSTMT_PENDING:
          ret->set_pending( exp );
          break;
        }
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
/** XML attribute writer of SgInquireStatement */
ATTRIB_STMT_DEFAULT(InquireStatement);

/** XML interanal node writer of SgInquireStatement */
void XevSageVisitor::inodeSgInquireStatement(SgNode* node)
{
  SgInquireStatement* inq = isSgInquireStatement(node);
  if(inq){
    if(inq->get_io_stmt_list()){
      this->visit(inq->get_io_stmt_list());
    }
    allocIostmtAttrib();
    TRAVERSE_IF_EXISTS("iolength",inq->get_iolengthExp());
    TRAVERSE_IF_EXISTS("unit",inq->get_unit());
    TRAVERSE_IF_EXISTS("iostat",inq->get_iostat());
    TRAVERSE_IF_EXISTS("err",inq->get_err());
    TRAVERSE_IF_EXISTS("iomsg",inq->get_iomsg());
    TRAVERSE_IF_EXISTS("file",inq->get_file());
    TRAVERSE_IF_EXISTS("access",inq->get_access());
    TRAVERSE_IF_EXISTS("form",inq->get_form());
    TRAVERSE_IF_EXISTS("recl",inq->get_recl());
    TRAVERSE_IF_EXISTS("blank",inq->get_blank());
    TRAVERSE_IF_EXISTS("exist",inq->get_exist());
    TRAVERSE_IF_EXISTS("opened",inq->get_opened());
    TRAVERSE_IF_EXISTS("number",inq->get_number());
    TRAVERSE_IF_EXISTS("named",inq->get_named());
    TRAVERSE_IF_EXISTS("name",inq->get_name());
    TRAVERSE_IF_EXISTS("sequential",inq->get_sequential());
    TRAVERSE_IF_EXISTS("direct",inq->get_direct());
    TRAVERSE_IF_EXISTS("formatted",inq->get_formatted());
    TRAVERSE_IF_EXISTS("unformatted",inq->get_unformatted());
    TRAVERSE_IF_EXISTS("nextrec",inq->get_nextrec());
    TRAVERSE_IF_EXISTS("position",inq->get_position());
    TRAVERSE_IF_EXISTS("action",inq->get_action());
    TRAVERSE_IF_EXISTS("read",inq->get_read());
    TRAVERSE_IF_EXISTS("write",inq->get_write());
    TRAVERSE_IF_EXISTS("readwrite",inq->get_readwrite());
    TRAVERSE_IF_EXISTS("delim",inq->get_delim());
    TRAVERSE_IF_EXISTS("pad",inq->get_pad());
    TRAVERSE_IF_EXISTS("asynchronous",inq->get_asynchronous());
    TRAVERSE_IF_EXISTS("decimal",inq->get_decimal());
    TRAVERSE_IF_EXISTS("stream",inq->get_stream());
    TRAVERSE_IF_EXISTS("size",inq->get_size());
    TRAVERSE_IF_EXISTS("padding",inq->get_pending());
  }
}

// ===============================================================================
/// Visitor of a SgLabelStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgLabelStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgLabelStatement*         ret   = 0;
  SgStatement*              body  = 0;
  SgScopeStatement*         scope = 0;

  //xe::DOMNamedNodeMap* amap = node->getAttributes();
  //xe::DOMNode* nameatt = 0;
  string                    slabel,nlabel;
  stringstream              val;
  int                       ino   = 0;

  XmlGetAttributeValue(node,"slabel",&slabel);
  XmlGetAttributeValue(node,"nlabel",&nlabel);

  SUBTREE_VISIT_BEGIN(node,astchild,astParent)
    {
      if (body==0)
        body = isSgStatement(astchild);
      if(scope==0)
        scope = isSgScopeStatement(astchild);
    }
  SUBTREE_VISIT_END();

  if( nlabel.size() ){
    ret = sb::buildLabelStatement( nlabel.c_str(), body, scope );
    SgLabelSymbol*  s = new SgLabelSymbol();
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    s->get_fortran_statement()->set_parent(s);
    //s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    s->set_label_type(SgLabelSymbol::e_start_label_type);
    val << nlabel;
    val >> ino;
    s->set_numeric_label_value( ino );
    SgLabelRefExp*  l = new SgLabelRefExp( s );
    ret->set_numeric_label( l );
  }
  /*
  else if( slabel.size() ){
    ret = sb::buildLabelStatement( slabel.c_str(), body, scope );
  }
  else XEV_ABORT();
  */
  else // slabel is allowed to be empty
    ret = sb::buildLabelStatement( slabel.c_str(), body, scope );
  XEV_ASSERT(ret!=NULL);
  return ret;
}
/** XML attribute writer of SgLabelStatement */
void XevSageVisitor::attribSgLabelStatement(SgNode* node)
{
  SgLabelStatement* n = isSgLabelStatement(node);

  if(n) {
    if( n->get_numeric_label() )
      sstr() << " nlabel=" << n->get_numeric_label()->get_name()  << " ";
    else
      sstr() << " slabel=" << n->get_label() << " ";
  }
  // don't call attribSgStatement
}
INODE_STMT_DEFAULT(LabelStatement);

// ===============================================================================
/// Visitor of a SgNullStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgNullStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgNullStatement* ret = sb::buildNullStatement();
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  return ret;
}
STMT_DEFAULT(NullStatement);

// ===============================================================================
/// Visitor of a SgNullifyStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgNullifyStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgNullifyStatement* ret = new SgNullifyStatement(DEFAULT_FILE_INFO);
  SgExprListExp*      plst = NULL;
  //std::vector< SgExpression * > exprs;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( plst==NULL )
        plst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  if(plst==NULL)
    XEV_MISSING_NODE(SgNullifyStatement,SgExprListExp,true);
  plst->set_parent(ret);
  ret->set_pointer_list( plst );

  return ret;
}
STMT_DEFAULT(NullifyStatement);

// ===============================================================================
/// Visitor of a SgOpenStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgOpenStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgOpenStatement* ret = new SgOpenStatement(DEFAULT_FILE_INFO);

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  allocIostmtAttrib();
  ret->set_io_statement(SgIOStatement::e_open);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgActualArgumentExpression* arg = isSgActualArgumentExpression(astchild);
      if(arg) {
        std::string   name = arg->get_argument_name().getString();
        if( g_iostmtAttrib.count(name) <= 0 ){
          XEV_FATAL("Unknown attribute \"" << name << "\" ");
        }
        int flg = g_iostmtAttrib[name];

        SgExpression* exp  = arg->get_expression();
        if( exp == 0 ){
          XEV_FATAL("Expression of \"" << name << "\" is null");
        }
        exp->set_parent(ret);
        exp->set_startOfConstruct(DEFAULT_FILE_INFO);

        switch(flg){
        case IOSTMT_UNIT:
          ret->set_unit( exp );
          break;
        case IOSTMT_IOSTAT:
          ret->set_iostat( exp );
          break;
        case IOSTMT_ERR:
          ret->set_err( exp );
          break;
        case IOSTMT_IOMSG:
          ret->set_iomsg( exp );
          break;
        case IOSTMT_FILE:
          ret->set_file( exp );
          break;
        case IOSTMT_ACCESS:
          ret->set_access( exp );
          break;
        case IOSTMT_FORM:
          ret->set_form( exp );
          break;
        case IOSTMT_RECL:
          ret->set_recl( exp );
          break;
        case IOSTMT_BLANK:
          ret->set_blank( exp );
          break;
        case IOSTMT_POSITION:
          ret->set_position( exp );
          break;
        case IOSTMT_ACTION:
          ret->set_action( exp );
          break;
        case IOSTMT_DELIM:
          ret->set_delim( exp );
          break;
        case IOSTMT_PAD:
          ret->set_pad( exp );
          break;
        case IOSTMT_ASYNCHRONOUS:
          ret->set_asynchronous( exp );
          break;
        case IOSTMT_STATUS:
          ret->set_status( exp );
          break;
        case IOSTMT_ROUND:
          ret->set_round( exp );
          break;
        case IOSTMT_SIGN:
          ret->set_sign( exp );
          break;
        default:
          XEV_FATAL("Invalid attribute \"" << name << "\" ");
          break;
        }
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
/** XML attribute writer of SgOpenStatement */
ATTRIB_STMT_DEFAULT(OpenStatement)

/** XML internal node writer of SgOpenStatement */
void XevSageVisitor::inodeSgOpenStatement(SgNode* node)
{
  SgOpenStatement*      n = isSgOpenStatement(node);

  if(n) {
    allocIostmtAttrib();
    TRAVERSE_IF_EXISTS("unit",  n->get_unit() );
    TRAVERSE_IF_EXISTS("iostat",n->get_iostat() );
    TRAVERSE_IF_EXISTS("err",   n->get_err() );
    TRAVERSE_IF_EXISTS("file",  n->get_file() );
    TRAVERSE_IF_EXISTS("status", n->get_status() );
    TRAVERSE_IF_EXISTS("access", n->get_access() );
    TRAVERSE_IF_EXISTS("form",   n->get_form() );
    TRAVERSE_IF_EXISTS("recl",   n->get_recl() );
    TRAVERSE_IF_EXISTS("blank",  n->get_blank() );
    TRAVERSE_IF_EXISTS("position", n->get_position() );
    TRAVERSE_IF_EXISTS("action", n->get_action() );
    TRAVERSE_IF_EXISTS("delim", n->get_delim() );
    TRAVERSE_IF_EXISTS("pad",   n->get_pad() );
    TRAVERSE_IF_EXISTS("iomsg", n->get_iomsg() );
    TRAVERSE_IF_EXISTS("round", n->get_round() );
    TRAVERSE_IF_EXISTS("sign",  n->get_sign() );
    TRAVERSE_IF_EXISTS("asynchronous", n->get_asynchronous() );
  }
}

// ===============================================================================
/// Visitor of a SgPrintStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgPrintStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgPrintStatement*     ret = new SgPrintStatement(DEFAULT_FILE_INFO);
  SgExprListExp*        exl = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  allocIostmtAttrib();
  ret->set_io_statement(SgIOStatement::e_print);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgActualArgumentExpression* arg = isSgActualArgumentExpression(astchild);
      if( exl==0 ) {
        exl = isSgExprListExp(astchild);
        if( exl) {
          exl->set_parent(ret);
          ret->set_io_stmt_list(exl);
        }
      }
      if( arg ) {
        std::string   name = arg->get_argument_name().getString();
        if( g_iostmtAttrib.count(name) <= 0 ){
          XEV_FATAL("Unknown attribute \"" << name << "\" ");
        }
        int flg = g_iostmtAttrib[name];

        SgExpression* exp  = arg->get_expression();
        if( exp == 0 ){
          XEV_FATAL("Expression of \"" << name << "\" is null");
        }
        exp->set_parent(ret);
        exp->set_startOfConstruct(DEFAULT_FILE_INFO);

        switch(flg){
        case IOSTMT_FMT:
          ret->set_format( exp );
          break;
        default:
          XEV_FATAL("Invalid attribute \"" << name << "\" ");
          break;
        }
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
/** XML attribute writer of SgPrintStatement */
ATTRIB_STMT_DEFAULT(PrintStatement);

/** XML internal node writer of SgPrintStatement */
void XevSageVisitor::inodeSgPrintStatement(SgNode* node)
{
  SgPrintStatement*      n = isSgPrintStatement(node);

  if(n) {
    if(n->get_io_stmt_list()){
      this->visit(n->get_io_stmt_list());
    }
    allocIostmtAttrib();
    TRAVERSE_IF_EXISTS("fmt",  n->get_format() );
  }
}



// ===============================================================================
/// Visitor of a SgReadStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgReadStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgReadStatement*        ret = new SgReadStatement(DEFAULT_FILE_INFO);
  SgExprListExp*          exl = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  allocIostmtAttrib();
  ret->set_io_statement(SgIOStatement::e_read);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgActualArgumentExpression* arg = isSgActualArgumentExpression(astchild);
      if( exl==0 ) {
        exl = isSgExprListExp(astchild);
        if(exl) {
          exl->set_parent(ret);
          ret->set_io_stmt_list(exl);
        }
      }
      if( arg ) {
        std::string   name = arg->get_argument_name().getString();
        if( g_iostmtAttrib.count(name) <= 0 ){
          XEV_FATAL("Unknown attribute \"" << name << "\" ");
        }
        int flg = g_iostmtAttrib[name];

        SgExpression* exp  = arg->get_expression();
        if( exp == 0 ){
          XEV_FATAL("Expression of \"" << name << "\" is null");
        }
        exp->set_parent(ret);
        exp->set_startOfConstruct(DEFAULT_FILE_INFO);

        switch(flg){
        case IOSTMT_UNIT:
          ret->set_unit( exp );
          break;
        case IOSTMT_IOSTAT:
          ret->set_iostat( exp );
          break;
        case IOSTMT_ERR:
          ret->set_err( exp );
          break;
        case IOSTMT_IOMSG:
          ret->set_iomsg( exp );
          break;
        case IOSTMT_FMT:
          ret->set_format( exp );
          break;
        case IOSTMT_REC:
          ret->set_rec( exp );
          break;
        case IOSTMT_END:
          ret->set_end( exp );
          break;
        case IOSTMT_NML:
          ret->set_namelist( exp );
          break;
        case IOSTMT_ADVANCE:
          ret->set_advance( exp );
          break;
        case IOSTMT_SIZE:
          ret->set_size( exp );
          break;
        case IOSTMT_EOR:
          ret->set_eor( exp );
          break;
        case IOSTMT_ASYNCHRONOUS:
          ret->set_asynchronous( exp );
          break;
        default:
          XEV_FATAL("Invalid attribute \"" << name << "\" ");
          break;
        }
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
/** XML attribute writer of SgReadStatement */
ATTRIB_STMT_DEFAULT(ReadStatement);

/** XML internal node writer of SgReadStatement */
void XevSageVisitor::inodeSgReadStatement(SgNode* node)
{
  SgReadStatement* n = isSgReadStatement(node);

  if(n) {
    if(n->get_io_stmt_list()){
      this->visit(n->get_io_stmt_list());
    }

    allocIostmtAttrib();
    TRAVERSE_IF_EXISTS("unit",         n->get_unit());
    TRAVERSE_IF_EXISTS("iostat",       n->get_iostat());
    TRAVERSE_IF_EXISTS("err",          n->get_err());
    TRAVERSE_IF_EXISTS("iomsg",        n->get_iomsg());
    TRAVERSE_IF_EXISTS("fmt",          n->get_format());
    TRAVERSE_IF_EXISTS("rec",          n->get_rec());
    TRAVERSE_IF_EXISTS("end",          n->get_end());
    TRAVERSE_IF_EXISTS("nml",          n->get_namelist());
    TRAVERSE_IF_EXISTS("advance",      n->get_advance());
    TRAVERSE_IF_EXISTS("size",         n->get_size());
    TRAVERSE_IF_EXISTS("eor",          n->get_eor());
    TRAVERSE_IF_EXISTS("asynchronous", n->get_asynchronous());
  }
}


// ===============================================================================
/// Visitor of a SgReturnStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgReturnStmt(xe::DOMNode* node, SgNode* astParent)
{
  SgReturnStmt*        ret   = 0;
  SgExpression*        exp   = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,astParent)
    {
      if(exp==0)
        exp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildReturnStmt(exp);
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);

  if(exp) exp->set_parent(ret);

  if( si::is_Fortran_language() && isSgLabelRefExp(exp) ){
    // --- Fortran alternative return support ---
    // see return_stmt in FortranParserActionROSE.C
    size_t retval = isSgLabelRefExp(exp)->get_numeric_label_value();
    SgInitializedName* aname = NULL;
    SgFunctionDefinition* fdef = si::getEnclosingFunctionDefinition(sb::topScopeStack());
    SgFunctionDeclaration* decl = NULL;
    if( fdef == NULL || (decl=fdef->get_declaration()) == NULL){
      XEV_DEBUG_INFO(node);
      XEV_WARN("def="<<fdef<<", decl="<<decl);
    }
    if(fdef==NULL)
      XEV_FATAL("enclosing function definition not found");
    if(decl==NULL)
      XEV_FATAL("declaration of the enclosing function not found");

    SgInitializedNamePtrList& args =decl->get_args();
    if(retval<1 || retval>args.size() ) {
      XEV_DEBUG_INFO(node);
      XEV_FATAL("invalid label for alternative return");
    }
    size_t counter = 1;
    for (size_t i = 0; i < args.size(); i++) {
      SgTypeLabel* typ = isSgTypeLabel(args[i]->get_type());
      if (typ){
        if(counter == retval)
          aname = args[i];
        counter++;
      }
    }
    if(aname){
      SgSymbol* s = aname->search_for_symbol_from_symbol_table();
      if(s==NULL){
        XEV_DEBUG_INFO(node);
        XEV_FATAL("symbol for alternative return not found");
      }
      SgLabelSymbol* sym = isSgLabelSymbol(s);
      if(sym==NULL)
        XEV_FATAL("label symbol for alternative return not found");
      delete exp;
      exp = new SgLabelRefExp(sym);
      XEV_ASSERT(exp!=NULL);
      si::setSourcePosition(exp);
      exp->set_parent(ret);
      ret->set_expression(exp);
    }
  }

  return ret;
}
STMT_DEFAULT(ReturnStmt);

// ===============================================================================
/// Visitor of a SgRewindStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgRewindStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgRewindStatement*      ret =  new SgRewindStatement(DEFAULT_FILE_INFO);

  // The attributes by attribSgIOStatement() are simply ignored
  // because unit is always required and the others are not used.
  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  allocIostmtAttrib();
  ret->set_io_statement(SgIOStatement::e_rewind);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgActualArgumentExpression* arg = isSgActualArgumentExpression(astchild);
      if(arg) {
        std::string   name = arg->get_argument_name().getString();
        if( g_iostmtAttrib.count(name) <= 0 ){
          XEV_FATAL("Unknown attribute \"" << name << "\" ");
        }
        int flg = g_iostmtAttrib[name];

        SgExpression* exp  = arg->get_expression();
        if( exp == 0 ){
          XEV_FATAL("Expression of \"" << name << "\" is null");
        }
        exp->set_parent(ret);
        exp->set_startOfConstruct(DEFAULT_FILE_INFO);

        switch(flg){
        case IOSTMT_UNIT:
          ret->set_unit( exp );
          break;
        case IOSTMT_IOSTAT:
          ret->set_iostat( exp );
          break;
        case IOSTMT_ERR:
          ret->set_err( exp );
          break;
        case IOSTMT_IOMSG:
          ret->set_iomsg( exp );
          break;
        default:
          XEV_FATAL("Invalid attribute \"" << name << "\" ");
          break;
        }
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
STMT_DEFAULT(RewindStatement);

// ===============================================================================
/// Visitor of a SgStopOrPauseStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgStopOrPauseStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgStopOrPauseStatement*     ret=0;
  SgExpression*               cod=0;

  int                         typ=0;

  XmlGetAttributeValue(node,"type",&typ);

  ret = new SgStopOrPauseStatement(DEFAULT_FILE_INFO);
  XEV_ASSERT(ret!=NULL);
  ret->set_stop_or_pause( (SgStopOrPauseStatement::stop_or_pause_enum) typ );
  ret->set_parent(astParent);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( cod==0 )
        cod = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();
  if( cod==NULL )
    cod = new SgNullExpression(DEFAULT_FILE_INFO);

  ret->set_code( cod );
  cod->set_parent( ret );

  return ret;
}
/** XML attribute writer of SgStopOrPauseStatement */
void XevSageVisitor::attribSgStopOrPauseStatement(SgNode* node)
{
  SgStopOrPauseStatement* n = isSgStopOrPauseStatement(node);
  if(n) {
    sstr() << " type=\"" << n->get_stop_or_pause () << "\" ";
  }
  attribSgStatement(sstr(),node);
}
void XevSageVisitor::inodeSgStopOrPauseStatement(SgNode* node)
{
  SgStopOrPauseStatement* n = isSgStopOrPauseStatement(node);
  if( n && n->get_code() ){
    this->visit(n->get_code());
  }
}

// ===============================================================================
/// Visitor of a SgSwitchStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgSwitchStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgSwitchStatement*    ret  = new SgSwitchStatement(DEFAULT_FILE_INFO);
  SgStatement*          item = 0;
  SgStatement*          body = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  sb::pushScopeStack(ret);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      // assuming the order
      if(item==0)
        item = isSgStatement(astchild);
      else if (body==0)
        body = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();
  sb::popScopeStack();

  if(item==0||body==0){
    XEV_MISSING_NODES(SgSwitchStatement,2,SgStatement,true);
  }
  ret->set_item_selector(item);
  ret->set_body(body);
  item->set_parent(ret);
  body->set_parent(ret);

  //ret = sb::buildSwitchStatement(item,body);
  if(si::is_Fortran_language()){
    int elabel=0;
    string slabel;
    if(XmlGetAttributeValue(node,"elabel",&elabel)){
      SgLabelSymbol*  s = new SgLabelSymbol();
      XEV_ASSERT(s!=NULL);
      s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
      s->get_fortran_statement()->set_parent(s);
      s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
      s->set_numeric_label_value( elabel );
      SgLabelRefExp*  l = new SgLabelRefExp( s );
      ret->set_end_numeric_label( l );
    }
    if(XmlGetAttributeValue(node,"slabel",&slabel)){
      ret->set_string_label(slabel);
    }
  }
  return ret;
}
/** XML attribute writer of SgSwidthStatement */
void XevSageVisitor::attribSgSwitchStatement(SgNode* node)
{
  SgSwitchStatement* n = isSgSwitchStatement(node);
  if(n) {
    SgLabelRefExp* l = n->get_end_numeric_label();
    if(l)
      sstr() << " elabel=\"" << l->get_numeric_label_value() << "\" ";
    if(n->get_string_label().size())
      sstr() << " slabel=\"" << n->get_string_label() << "\" ";
  }
  attribSgStatement(sstr(),node);
}
INODE_STMT_DEFAULT(SwitchStatement);

// ===============================================================================
/// Visitor of a SgWaitStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgWaitStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgWaitStatement*     ret = new SgWaitStatement(DEFAULT_FILE_INFO);

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  allocIostmtAttrib();
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgActualArgumentExpression* arg = isSgActualArgumentExpression(astchild);
      if(arg) {
        std::string   name = arg->get_argument_name().getString();
        if( g_iostmtAttrib.count(name) <= 0 ){
          XEV_FATAL("Unknown attribute \"" << name << "\" ");
        }
        int flg = g_iostmtAttrib[name];

        SgExpression* exp  = arg->get_expression();
        if( exp == 0 ){
          XEV_FATAL("Expression of \"" << name << "\" is null");
        }
        exp->set_parent(ret);
        exp->set_startOfConstruct(DEFAULT_FILE_INFO);

        switch(flg){
        case IOSTMT_UNIT:
          ret->set_unit( exp );
          break;
        case IOSTMT_IOSTAT:
          ret->set_iostat( exp );
          break;
        case IOSTMT_ERR:
          ret->set_err( exp );
          break;
        case IOSTMT_IOMSG:
          ret->set_iomsg( exp );
          break;
        default:
          XEV_FATAL("Invalid attribute \"" << name << "\" ");
          break;
        }
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
STMT_DEFAULT(WaitStatement);

// ===============================================================================
/// Visitor of a SgWhereStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgWhereStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgWhereStatement*     ret = new SgWhereStatement(DEFAULT_FILE_INFO);
  SgExpression*         cond = 0;
  SgBasicBlock*         body = 0;
  SgElseWhereStatement* elsw = 0;
  int                   endw = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  XmlGetAttributeValue(node,"end",&endw);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      /* assuming these stmts appear in this order */
      if(cond==0)
        cond = isSgExpression(astchild);
      else if (body==0)
        body = isSgBasicBlock(astchild);
      else if (elsw==0)
        elsw = isSgElseWhereStatement(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_condition( cond );
  ret->set_body( body );
  ret->set_elsewhere( elsw );
  ret->set_has_end_statement(endw);
  string slabel;
  if(XmlGetAttributeValue(node,"slabel",&slabel)){
    ret->set_string_label(slabel);
  }
  return ret;
}
/** XML attribute writer of SgWhereStmt */
void XevSageVisitor::attribSgWhereStatement(SgNode* node)
{
  SgWhereStatement*   n = isSgWhereStatement(node);

  if(n) {
    sstr() << " end=\"" << n->get_has_end_statement() << "\" ";
    if(n->get_string_label().size())
      sstr() << " slabel=\"" << n->get_string_label() << "\" ";
    SgLabelRefExp* l = n->get_end_numeric_label();
    if(l){
      sstr() << " elabel=\"" << l->get_numeric_label_value() << "\" ";
    }
  }
  attribSgStatement(sstr(),n);
}
INODE_STMT_DEFAULT(WhereStatement)

// ===============================================================================
/// Visitor of a SgWhileStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgWhileStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgWhileStmt*          ret   = new SgWhileStmt(DEFAULT_FILE_INFO);
  SgExprStatement*      cond  = 0;
  SgStatement*          tstmt = 0;
  SgStatement*          fstmt = 0;
  int enddo=0;
  int nlabel=0;
  string slabel;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      /* assuming these stmts appear in this order */
      if(cond==0)
        cond = isSgExprStatement(astchild);
      else if (tstmt==0)
        tstmt = isSgStatement(astchild);
      else if (fstmt==0)
        fstmt = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  //ret = sb::buildWhileStmt(cond,tstmt,fstmt);
  if(cond) {
    ret->set_condition(cond);
    cond->set_parent(ret);
  }
  if(tstmt) {
    ret->set_body(tstmt);
    tstmt->set_parent(ret);
  }
  if(fstmt) {
    ret->set_else_body(fstmt);
    fstmt->set_parent(ret);
  }

  if(si::is_Fortran_language()){
    XmlGetAttributeValue(node,"end",&enddo);
    XmlGetAttributeValue(node,"slabel",&slabel);
    XmlGetAttributeValue(node,"elabel",&nlabel);

    if(enddo){
      ret->set_has_end_statement(true);
    }
    if( slabel.size() )
      ret->set_string_label( slabel );
    else if( nlabel) {
      SgLabelSymbol*  s = new SgLabelSymbol();
      XEV_ASSERT(s!=NULL);
      s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
      s->get_fortran_statement()->set_parent(s);
      s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
      s->set_numeric_label_value( nlabel );
      SgLabelRefExp*  l = new SgLabelRefExp( s );
      ret->set_end_numeric_label( l );
    }
  }
  return ret;
}
/** XML attribute writer of SgWhileStmt */
void XevSageVisitor::attribSgWhileStmt(SgNode* node)
{
  SgWhileStmt*   n = isSgWhileStmt(node);
  SgLabelRefExp* l = 0;

  if(n&&si::is_Fortran_language()) {
    sstr() << " end=\"" << n->get_has_end_statement() << "\" ";
    l = n->get_end_numeric_label();
    if(l){
      sstr() << " elabel=\"" << l->get_numeric_label_value() << "\" ";
    }
    else {
      sstr() << " slabel=\"" << n->get_string_label() << "\" ";
    }
  }
  attribSgStatement(sstr(),node);
}
INODE_STMT_DEFAULT(WhileStmt)

// ===============================================================================
/// Visitor of a SgWriteStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgWriteStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgWriteStatement*     ret = new SgWriteStatement(DEFAULT_FILE_INFO);
  SgExprListExp*        exl = 0;

  XEV_ASSERT(ret!=NULL);
  ret->set_parent(astParent);
  allocIostmtAttrib();
  ret->set_io_statement(SgIOStatement::e_write);
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      SgActualArgumentExpression* arg = isSgActualArgumentExpression(astchild);
      if( exl==0 ) {
        exl = isSgExprListExp(astchild);
        if( exl ) {
          exl->set_parent(ret);
          ret->set_io_stmt_list(exl);
        }
      }
      if(arg) {
        std::string   name = arg->get_argument_name().getString();
        if( g_iostmtAttrib.count(name) <= 0 ){
          XEV_FATAL("Unknown attribute \"" << name << "\" ");
        }
        int flg = g_iostmtAttrib[name];

        SgExpression* exp  = arg->get_expression();
        if( exp == 0 ){
          XEV_FATAL("Expression of \"" << name << "\" is null");
        }
        exp->set_parent(ret);
        exp->set_startOfConstruct(DEFAULT_FILE_INFO);

        switch(flg){
        case IOSTMT_UNIT:
          ret->set_unit( exp );
          break;
        case IOSTMT_IOSTAT:
          ret->set_iostat( exp );
          break;
        case IOSTMT_ERR:
          ret->set_err( exp );
          break;
        case IOSTMT_IOMSG:
          ret->set_iomsg( exp );
          break;
        case IOSTMT_FMT:
          ret->set_format( exp );
          break;
        case IOSTMT_REC:
          ret->set_rec( exp );
          break;
        case IOSTMT_ADVANCE:
          ret->set_advance( exp );
          break;
        case IOSTMT_NML:
          ret->set_namelist( exp );
          break;
        case IOSTMT_ASYNCHRONOUS:
          ret->set_asynchronous( exp );
          break;
        default:
          XEV_FATAL("Invalid attribute \"" << name << "\" ");
          break;
        }
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
/** XML attribute writer of SgWriteStatement */
ATTRIB_STMT_DEFAULT(WriteStatement);

/** XML internal node writer of SgWriteStatement */
void XevSageVisitor::inodeSgWriteStatement(SgNode* node)
{
  SgWriteStatement* n = isSgWriteStatement(node);

  if(n) {
    if(n->get_io_stmt_list()){
      this->visit(n->get_io_stmt_list());
    }

    allocIostmtAttrib();
    TRAVERSE_IF_EXISTS("unit",     n->get_unit());
    TRAVERSE_IF_EXISTS("iostat",   n->get_iostat());
    TRAVERSE_IF_EXISTS("err",      n->get_err());
    TRAVERSE_IF_EXISTS("iomsg",    n->get_iomsg());
    TRAVERSE_IF_EXISTS("fmt",      n->get_format());
    TRAVERSE_IF_EXISTS("rec",      n->get_rec());
    TRAVERSE_IF_EXISTS("advance",  n->get_advance());
    TRAVERSE_IF_EXISTS("nml",      n->get_namelist());
    TRAVERSE_IF_EXISTS("asynchronous", n->get_asynchronous());
  }
}
