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

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;
using namespace XevXml;

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
          si::attachArbitraryText(prev,remain,PreprocessingInfo::before);
          remain="";
        }
      }
    }
    else if (child->getNodeType() == xe::DOMNode::TEXT_NODE){
      std::string tmp = xe::XMLString::transcode(child->getNodeValue());
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
          si::attachArbitraryText(prev,text,PreprocessingInfo::after);
        else // the text is located before the 1st statement
          remain = text;
      }
    }
    child=child->getNextSibling();
  }
  if(remain.length()) // for empty basic block
    si::attachArbitraryText(isSgLocatedNode(blk),remain,PreprocessingInfo::inside);
}


static void attribSgIOStatement(std::ostream& str,SgNode* node)
{
  SgIOStatement*      n = isSgIOStatement(node);

  if(n) {
    // print the attribute only if it should be 1.
    // (assuming the default value is 0)
    if(n->get_unit())
      str << " unit=\"1\" ";
    if(n->get_iostat())
      str << " iostat=\"1\" ";
    if(n->get_err())
      str << " err=\"1\" ";
  }
}

void attribSgStatement(std::ostream& str,SgNode* node)
{
  SgStatement*    stmt = isSgStatement(node);

  if( stmt ) {
    SgLabelRefExp*  l = stmt->get_numeric_label();
    if(l)
      str << " label=\"" << l->get_numeric_label_value() << "\" ";
  }
  attribSgIOStatement(str,node);
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
    return;                                             \
  }

#define STMT_DEFAULT(x)                         \
  ATTRIB_STMT_DEFAULT(x)                        \
  INODE_STMT_DEFAULT(x)

#define TRAVERSE_IF_EXISTS(x) if(x){this->visit(x);}

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

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      //assuming these stmts appear in this order
      if( les==NULL )
        les = isSgLabelRefExp(astchild);
      else if( eql==NULL )
        eql = isSgLabelRefExp(astchild);
      else if( grt==NULL )
        grt = isSgLabelRefExp(astchild);
      else if( cnd==NULL )
        cnd = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_parent(astParent);

  ret->set_less_label( les );
  les->set_parent( ret );

  ret->set_equal_label( eql );
  eql->set_parent( ret );

  ret->set_greater_label( grt );
  grt->set_parent( ret );

  ret->set_conditional( cnd );
  cnd->set_parent( ret );

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
/// Visitor of a SgBackspaceStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgBackspaceStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgBackspaceStatement*   ret =
    new SgBackspaceStatement(DEFAULT_FILE_INFO);
  SgExpression*           unt = 0;
  SgExpression*           err = 0;
  SgExpression*           ist = 0;
  bool f_ist, f_unt, f_err;

  XmlGetAttributeValue(node,"iostat",&f_ist);
  XmlGetAttributeValue(node,"unit",  &f_unt);
  XmlGetAttributeValue(node,"err",   &f_err);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      //assuming these stmts appear in this order
      if( f_unt && unt==0 )
        unt = isSgExpression(astchild);
      else if( f_ist && ist==0 )
        ist = isSgExpression(astchild);
      else if( f_err && err==0 )
        err = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  if( unt ) {
    ret->set_unit( unt );
    unt->set_parent(ret);
    unt->set_startOfConstruct(DEFAULT_FILE_INFO);
  }

  if( err ) {
    ret->set_err( err );
    err->set_parent(ret);
    err->set_startOfConstruct(DEFAULT_FILE_INFO);
  }
  if( ist ) {
    ret->set_iostat( ist );
  }

  return ret;
}
STMT_DEFAULT(BackspaceStatement);

// ===============================================================================
/// Visitor of a SgBasicBlock element in an XML document
SgNode*
XevXmlVisitor::visitSgBasicBlock(xe::DOMNode* node, SgNode* astParent)
{
  SgBasicBlock* ret = sb::buildBasicBlock();
  //SgScopeStatement* scope = sb::topScopeStack();
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
  SgBreakStmt* ret = 0;
  ret = sb::buildBreakStmt();

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
  SgCaseOptionStmt*     ret  = 0;
  SgExpression*         key  = 0;
  SgStatement*          body = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(key==0)
        key = isSgExpression(astchild);
      else if (body==0)
        body = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildCaseOptionStmt(key,body);
  return ret;
}
STMT_DEFAULT(CaseOptionStmt);

// ===============================================================================
/// Visitor of a SgClassDefinition element in an XML document
SgNode*
XevXmlVisitor::visitSgClassDefinition(xercesc::DOMNode* node, SgNode* astParent)
{
  SgClassDefinition*        ret = 0;
  SgClassDeclaration*       dec = isSgClassDeclaration( astParent );
  SgDeclarationStatement*   fld = 0;
  int seq=0;
  int pri=0;
  int abs=0;

  XmlGetAttributeValue(node,"sequence",&seq);
  XmlGetAttributeValue(node,"private",&pri);
  XmlGetAttributeValue(node,"abstract",&abs);
  if(dec==0) XEV_ABORT();
  ret = sb::buildClassDefinition( dec );
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
      }
    }
  SUBTREE_VISIT_END();
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
  SgExpression*         unt = 0;
  SgExpression*         err = 0;
  SgExpression*         ist = 0;
  SgExpression*         stt = 0;
  bool f_ist=false, f_unt=false, f_err=false,f_stt=false;

  XmlGetAttributeValue(node,"iostat",&f_ist);
  XmlGetAttributeValue(node,"unit",  &f_unt);
  XmlGetAttributeValue(node,"err",   &f_err);
  XmlGetAttributeValue(node,"status",&f_stt);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( f_unt && unt==0 )
        unt = isSgExpression(astchild);
      else if( f_ist && ist==0 )
        ist = isSgExpression(astchild);
      else if( f_err && err==0 )
        err = isSgExpression(astchild);
      else if( f_stt && stt==0 )
        stt = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  if(unt)
    ret->set_unit(unt);
  if(err)
    ret->set_err(err);
  if(ist)
    ret->set_iostat(ist);
  if(stt)
    ret->set_status(stt);
  ret->set_parent(astParent);
  return ret;
}
/** XML attribute writer of SgClassDefinition */
void XevSageVisitor::attribSgCloseStatement(SgNode* node)
{
  SgCloseStatement*      n = isSgCloseStatement(node);

  if(n) {
    sstr() << " status=\"" << (n->get_status()!=0) << "\" ";
  }
  attribSgStatement(sstr(),node);
}
INODE_STMT_DEFAULT(CloseStatement);

// ===============================================================================
/// Visitor of a SgComputedGotoStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgComputedGotoStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgExprListExp*          exp = 0;
  SgExpression*           var = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if( exp==0 )
        exp = isSgExprListExp(astchild);
      else if( var==0 )
        var = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  SgComputedGotoStatement*    ret =
    new SgComputedGotoStatement(DEFAULT_FILE_INFO, exp,var );

  exp->set_parent(ret);
  var->set_parent(ret);
  ret->set_parent(astParent);
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
  ret->set_parent(astParent);
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
  SgDefaultOptionStmt*      ret  = 0;
  SgStatement*              body = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if (body==0)
        body = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildDefaultOptionStmt(body);

  return ret;
}
STMT_DEFAULT(DefaultOptionStmt);

// ===============================================================================
/// Visitor of a SgDoWhileStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgDoWhileStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgDoWhileStmt*        ret  = 0;
  SgStatement*          body = 0;
  SgExprStatement*      cond = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      /* assuming these stmts appear in this order */
      if(body==0)
        body = isSgStatement(astchild);
      else if (cond==0)
        cond = isSgExprStatement(astchild);
    }
  SUBTREE_VISIT_END();
  ret = sb::buildDoWhileStmt(body,cond);
  if(cond)  cond->set_parent(ret);
  if(body)  body->set_parent(ret);
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
  ret->set_parent(astParent);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(cond==0)
        cond = isSgExpression(astchild);
      else if (body==0)
        body = isSgBasicBlock(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_condition( cond );
  ret->set_body( body );
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
  SgExpression*          unt = 0;
  SgExpression*          err = 0;
  SgExpression*          ist = 0;
  bool f_ist, f_unt, f_err;

  XmlGetAttributeValue(node,"iostat",&f_ist);
  XmlGetAttributeValue(node,"unit",  &f_unt);
  XmlGetAttributeValue(node,"err",   &f_err);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( f_unt && unt==0 )
        unt = isSgExpression(astchild);
      else if( f_ist && ist==0 )
        ist = isSgExpression(astchild);
      else if( f_err && err==0 )
        err = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  if( unt ) {
    ret->set_unit( unt );
    unt->set_parent(ret);
    unt->set_startOfConstruct(DEFAULT_FILE_INFO);
  }

  if( err ) {
    ret->set_err( err );
    err->set_parent(ret);
    err->set_startOfConstruct(DEFAULT_FILE_INFO);
  }
  if( ist )
    ret->set_iostat( ist );

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

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if(exp==0)
        exp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_expression(exp);
  ret->set_parent(astParent);
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
  SgExpression*     unt = 0;

  // The attributes by attribSgIOStatement() are simply ignored
  // because unit is always required and the others are not used.

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( unt==0 )
        unt = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  if( unt ) {
    ret->set_unit( unt );
    unt->set_parent(ret);
    unt->set_startOfConstruct(DEFAULT_FILE_INFO);
  }
  ret->set_parent(astParent);
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

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( hed==0 )
        hed = isSgExprListExp(astchild);
      else if( bdy==0 )
        bdy = isSgBasicBlock(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_parent(astParent);
  hed->set_parent(ret);
  bdy->set_parent(ret);
  if(si::is_Fortran_language()==true)
    bdy->setCaseInsensitive(true);

  hed->set_startOfConstruct(DEFAULT_FILE_INFO);
  bdy->set_startOfConstruct(DEFAULT_FILE_INFO);

  ret->set_forall_header(hed);
  ret->set_body(bdy);
  //ret->set_has_end_statement(true);
  //ret->set_has_end_statement(false);

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

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if((stmt = isSgStatement(astchild))!=0)
        lst.push_back(stmt);
    }
  SUBTREE_VISIT_END();

  //lst could be empty
  ret = sb::buildForInitStatement(lst);

  return ret;
}
STMT_DEFAULT(ForInitStatement);

// ===============================================================================
/// Visitor of a SgForStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgForStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForStatement* ret = 0;
  SgStatement*    ini = 0;
  SgStatement*    tst = 0;
  SgExpression*   inc = 0;
  SgStatement*    bdy = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      /* assuming these stmts appear in this order */
      if(ini==0)
        ini = isSgStatement(astchild);
      else if (tst==0)
        tst = isSgStatement(astchild);
      else if (inc==0)
        inc = isSgExpression(astchild);
      else if (bdy==0)
        bdy = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildForStatement(ini,tst,inc,bdy);
  ret->set_parent(astParent);
  if(ini)ini->set_parent(ret);
  if(tst)tst->set_parent(ret);
  if(inc)inc->set_parent(ret);
  if(bdy)bdy->set_parent(ret);

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
    else {
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
  if(blk)
    ret->set_body(blk);
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

  if( n_name.size() ){
    /**/
    ret = sb::buildGotoStatement( label );
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
  else
    ret = sb::buildGotoStatement( label );

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
  if(si::is_Fortran_language()==true)
    ret->setCaseInsensitive(true);
  ret->set_parent(astParent);
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

  ret->set_conditional(cond);
  ret->set_true_body(tstmt);
  ret->set_false_body(fstmt);

  //ret = sb::buildIfStmt(cond,tstmt,fstmt);
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
  SgExpression*       exp = 0;
  SgLabelRefExp*      lbl = 0;
  SgVarRefExp*        var = 0;
  SgExprListExp*      ele = 0;
  //xe::DOMNamedNodeMap*    amap = node->getAttributes();
  //xe::DOMNode*            nameatt = 0;
  //string                  nlabel;
  int                 ino = 0;
  int                 flg[33];
  const char* arg_type[] ={
    "iolength",    // 1
    "unit",        // 2
    "iostat",      // 3
    "err",         // 4
    "iomsg",       // 5
    "file",        // 6
    "access",      // 7
    "form",        // 8
    "recl",        // 9
    "blank",       //10
    "exist",       //11
    "opened",      //12
    "number",      //13
    "named",       //14
    "name",        //15
    "sequential",  //16
    "direct",      //17
    "formatted",   //18
    "unformatted", //19
    "nextrec",     //20
    "position",    //21
    "action",      //22
    "access",      //23
    "read",        //24
    "write",       //25
    "readwrite",   //26
    "delim",       //27
    "pad",         //28
    "asynchronous",//29
    "decimal",     //30
    "stream",      //31
    "size",        //32
    "pending"      //33
  };

  ret->set_parent(astParent);
  ino = 0;
  memset( flg,0,sizeof(flg) );
  for(int i(0);i<33;i++){
    int val;
    if( XmlGetAttributeValue(node,arg_type[i],&val) ){
      flg[ino] = i+1;
      ino++;
    }
  }

  ino = 0;
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      exp = isSgExpression(astchild);
      switch( flg[ino] ){
      case 1:
        ele = isSgExprListExp(astchild);
        var = isSgVarRefExp(astchild);
        if( ele ) {
          ret->set_io_stmt_list(ele);
          ele->set_parent(ret);
        }
        if( var ) {
          ret->set_iolengthExp(var);
          var->set_parent(ret);
        }
        ino--;
        break;

      case 2:
        ret->set_unit( exp );
        break;
      case 3:
        ret->set_iostat( exp );
        break;
      case 4:
        lbl = isSgLabelRefExp(astchild);
        ret->set_err( lbl );
        lbl->set_parent(ret);
        break;
      case 5:
        ret->set_iomsg( exp );
        break;
      case 6:
        ret->set_file( exp );
        break;
      case 7:
        ret->set_access( exp );
        break;
      case 8:
        ret->set_form( exp );
        break;
      case 9:
        ret->set_recl( exp );
        break;
      case 10:
        ret->set_blank( exp );
        break;
      case 11:
        ret->set_exist( exp );
        break;
      case 12:
        ret->set_opened( exp );
        break;
      case 13:
        ret->set_number( exp );
        break;
      case 14:
        ret->set_named( exp );
        break;
      case 15:
        ret->set_name( exp );
        break;
      case 16:
        ret->set_sequential( exp );
        break;
      case 17:
        ret->set_direct( exp );
        break;
      case 18:
        ret->set_formatted( exp );
        break;
      case 19:
        ret->set_unformatted( exp );
        break;
      case 20:
        ret->set_nextrec( exp );
        break;
      case 21:
        ret->set_position( exp );
        break;
      case 22:
        ret->set_action( exp );
        break;
      case 23:
        ret->set_read( exp );
        break;
      case 24:
        ret->set_write( exp );
        break;
      case 25:
        ret->set_readwrite( exp );
        break;
      case 26:
        ret->set_delim( exp );
        break;
      case 27:
        ret->set_pad( exp );
        break;
      case 28:
        ret->set_asynchronous( exp );
        break;
      case 29:
        ret->set_opened( exp );
        break;
      case 30:
        ret->set_decimal( exp );
        break;
      case 31:
        ret->set_stream( exp );
        break;
      case 32:
        ret->set_size( exp );
        break;
      case 33:
        ret->set_pending( exp );
        break;
      }
      ino++;
    }
  SUBTREE_VISIT_END();

  return ret;
}
/** XML attribute writer of SgInquireStatement */
void XevSageVisitor::attribSgInquireStatement(SgNode* node)
{
  SgInquireStatement*      n = isSgInquireStatement(node);

  if(n) {
    if( n->get_iolengthExp() )  sstr() << " iolength=\"1\" ";
    if( n->get_unit() )         sstr() << " unit=\"1\" ";
    if( n->get_iostat() )       sstr() << " iostat=\"1\" ";
    if( n->get_err() )          sstr() << " err=\"1\" ";
    if( n->get_iomsg() )        sstr() << " iomsg=\"1\" ";
    if( n->get_file() )         sstr() << " file=\"1\" ";
    if( n->get_access() )       sstr() << " access=\"1\" ";
    if( n->get_form() )         sstr() << " form=\"1\" ";
    if( n->get_recl() )         sstr() << " recl=\"1\" ";
    if( n->get_blank() )        sstr() << " blank=\"1\" ";
    if( n->get_exist() )        sstr() << " exist=\"1\" ";
    if( n->get_opened() )       sstr() << " opened=\"1\" ";
    if( n->get_number() )       sstr() << " number=\"1\" ";
    if( n->get_named() )        sstr() << " named=\"1\" ";
    if( n->get_name() )         sstr() << " name=\"1\" ";
    if( n->get_sequential() )   sstr() << " sequential=\"1\" ";
    if( n->get_direct() )       sstr() << " direct=\"1\" ";
    if( n->get_formatted() )    sstr() << " formatted=\"1\" ";
    if( n->get_unformatted() )  sstr() << " unformatted=\"1\" ";
    if( n->get_nextrec() )      sstr() << " nextrec=\"1\" ";
    if( n->get_position() )     sstr() << " position=\"1\" ";
    if( n->get_action() )       sstr() << " action=\"1\" ";
    if( n->get_read() )         sstr() << " read=\"1\" ";
    if( n->get_write() )        sstr() << " write=\"1\" ";
    if( n->get_readwrite() )    sstr() << " readwrite=\"1\" ";
    if( n->get_delim() )        sstr() << " delim=\"1\" ";
    if( n->get_pad() )          sstr() << " pad=\"1\" ";
    if( n->get_asynchronous() ) sstr() << " asynchronous=\"1\" ";
    if( n->get_stream() )       sstr() << " decimal=\"1\" ";
    if( n->get_stream() )       sstr() << " stream=\"1\" ";
    if( n->get_size() )         sstr() << " size=\"1\" ";
    if( n->get_pending() )      sstr() << " pending=\"1\" ";
  }
}
/** XML interanal node writer of SgInquireStatement */
void XevSageVisitor::inodeSgInquireStatement(SgNode* node)
{
  if(isSgInquireStatement(node)){
    SgInquireStatement* inq = isSgInquireStatement(node);
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

  SUBTREE_VISIT_BEGIN(node,astchild,0)
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

  return ret;
}
/** XML attribute writer of SgLabelStatement */
void XevSageVisitor::attribSgLabelStatement(SgNode* node)
{
  SgLabelStatement* n = isSgLabelStatement(node);

  if(n) {
    if( n->get_numeric_label() )
      sstr() << " label=" << n->get_numeric_label()->get_name()  << " ";
    sstr() << " slabel=" << n->get_label() ;
  }
}
INODE_STMT_DEFAULT(LabelStatement);

// ===============================================================================
/// Visitor of a SgNullStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgNullStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgNullStatement* ret = sb::buildNullStatement();
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

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( plst==NULL )
        plst = isSgExprListExp(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_parent(astParent);
  ret->set_pointer_list( plst );
  ret->set_parent(astParent);
  return ret;
}
STMT_DEFAULT(NullifyStatement);

// ===============================================================================
/// Visitor of a SgOpenStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgOpenStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgOpenStatement* ret = new SgOpenStatement(DEFAULT_FILE_INFO);
  SgExpression*    exp = NULL;
  SgLabelRefExp*   lbl = NULL;
  int              ino = 0;
  int              flg[17];
  int              val;
  const char* open_arg [] = {
    "unit",    // 1
    "iostat",  // 2
    "err",     // 3
    "file",    // 4
    "status",  // 5
    "access",  // 6
    "form",    // 7
    "recl",    // 8
    "blank",   // 9
    "position",// 10
    "action",  // 11
    "delim",   // 12
    "pad",     // 13
    "iomsg",   // 14
    "round",   // 15
    "sign",    // 16
    "asynchronous" // 17
  };

  ino = 0;
  memset( flg,0,sizeof(flg) );
  for(int i=0;i<17;i++){
    if(XmlGetAttributeValue(node,open_arg[i],&val)){
      flg[ino] = i+1;
      ino++;
    }
  }

  ino = 0;
  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      exp = isSgExpression(astchild);
      if(exp) {
        switch( flg[ino] ){
        case 1:
          ret->set_unit( exp );
          break;
        case 2:
          ret->set_iostat( exp );
          break;
        case 3:
          lbl = isSgLabelRefExp(astchild);
          ret->set_err( lbl );
          lbl->set_parent(ret);
          break;
        case 4:
          ret->set_file( exp );
          break;
        case 5:
          ret->set_status( exp );
          break;
        case 6:
          ret->set_access( exp );
          break;
        case 7:
          ret->set_form( exp );
          break;
        case 8:
          ret->set_recl( exp );
          break;
        case 9:
          ret->set_blank( exp );
          break;
        case 10:
          ret->set_position( exp );
          break;
        case 11:
          ret->set_action( exp );
          break;
        case 12:
          ret->set_delim( exp );
          break;
        case 13:
          ret->set_pad( exp );
          break;
        case 14:
          ret->set_iomsg( exp );
          break;
        case 15:
          ret->set_round( exp );
          break;
        case 16:
          ret->set_sign( exp );
          break;
        case 17:
          ret->set_asynchronous( exp );
          break;
        }
        ino++;
      }
    }
  SUBTREE_VISIT_END();

  return ret;
}
/** XML attribute writer of SgOpenStatement */
void XevSageVisitor::attribSgOpenStatement(SgNode* node)
{
  SgOpenStatement*      n = isSgOpenStatement(node);

  if(n) {
    if( n->get_unit() )         sstr() << " unit=\"1\" ";
    if( n->get_iostat() )       sstr() << " iostat=\"1\" ";
    if( n->get_err() )          sstr() << " err=\"1\" ";
    if( n->get_file() )         sstr() << " file=\"1\" ";
    if( n->get_status() )       sstr() << " status=\"1\" ";
    if( n->get_access() )       sstr() << " access=\"1\" ";
    if( n->get_form() )         sstr() << " form=\"1\" ";
    if( n->get_recl() )         sstr() << " recl=\"1\" ";
    if( n->get_blank() )        sstr() << " blank=\"1\" ";
    if( n->get_position() )     sstr() << " position=\"1\" ";
    if( n->get_action() )       sstr() << " action=\"1\" ";
    if( n->get_delim() )        sstr() << " delim=\"1\" ";
    if( n->get_pad() )          sstr() << " pad=\"1\" ";
    if( n->get_iomsg() )        sstr() << " iomsg=\"1\" ";
    if( n->get_round() )        sstr() << " round=\"1\" ";
    if( n->get_sign() )         sstr() << " sign=\"1\" ";
    if( n->get_asynchronous() ) sstr() << " asynchronous=\"1\" ";
  }
}
INODE_STMT_DEFAULT(OpenStatement)

// ===============================================================================
/// Visitor of a SgPrintStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgPrintStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgPrintStatement*     ret = new SgPrintStatement(DEFAULT_FILE_INFO);
  SgExprListExp*        exp = 0;
  SgExpression*         fmt = 0;

  // The attributes by attribSgIOStatement() are simply ignored
  // because they are not used.

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( exp==0 )
        exp = isSgExprListExp(astchild);
      else if( fmt==0 )
        fmt = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_io_stmt_list(exp);
  ret->set_format( fmt );
  if(exp)exp->set_parent(ret);
  if(fmt)fmt->set_parent(ret);
  ret->set_parent(astParent);
  return ret;
}
STMT_DEFAULT(PrintStatement);

// ===============================================================================
/// Visitor of a SgReadStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgReadStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgReadStatement*        ret = new SgReadStatement(DEFAULT_FILE_INFO);
  SgExprListExp*          exp = 0;
  SgExpression*           fmt = 0;
  SgExpression*           iost = 0;
  SgExpression*           rec = 0;
  SgExpression*           end = 0;
  SgExpression*           err = 0;
  SgExpression*           unt = 0;

  int f_fmt = 0;
  int f_ios = 0;
  int f_rec = 0;
  int f_end = 0;
  int f_err = 0;

  XmlGetAttributeValue(node,"fmt"   ,&f_fmt);
  XmlGetAttributeValue(node,"iostat",&f_ios);
  XmlGetAttributeValue(node,"rec",   &f_rec);
  XmlGetAttributeValue(node,"end",   &f_end);
  XmlGetAttributeValue(node,"err",   &f_err);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      //assuming these stmts appear in this order
      if( exp==0 )
        exp = isSgExprListExp(astchild);
      else if( unt==0 )
        unt = isSgExpression(astchild);
      else if( f_fmt && fmt==0 )
        fmt = isSgExpression(astchild);
      else if( f_ios && iost==0 )
        iost = isSgExpression(astchild);
      else if( f_rec && rec==0 )
        rec = isSgExpression(astchild);
      else if( f_end && end==0 )
        end = isSgExpression(astchild);
      else if( f_err && err==0 )
        err = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_parent(astParent);
  ret->set_io_stmt_list(exp);
  ret->set_format( fmt );
  ret->set_unit(unt);
  if(exp)exp->set_parent(ret);
  if(fmt)fmt->set_parent(ret);
  if(iost)iost->set_parent(ret);
  if(rec)rec->set_parent(ret);
  if(end)end->set_parent(ret);
  if(err)err->set_parent(ret);
  return ret;
}
/** XML attribute writer of SgReadStatement */
void XevSageVisitor::attribSgReadStatement(SgNode* node)
{
  SgReadStatement* n = isSgReadStatement(node);
  if(n) {
    if( n->get_format() )
      sstr() << " fmt=\"1\"";
    if( n->get_iostat() )
      sstr() << " iostat=\"1\"";
    if( n->get_rec() )
      sstr() << " rec=\"1\"";
    if( n->get_end() )
      sstr() << " end=\"1\"";
    if( n->get_err() )
      sstr() << " err=\"1\"";
  }
}
INODE_STMT_DEFAULT(ReadStatement);

// ===============================================================================
/// Visitor of a SgReturnStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgReturnStmt(xe::DOMNode* node, SgNode* astParent)
{
  SgReturnStmt*        ret   = 0;
  SgExpression*        exp   = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      if(exp==0)
        exp = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildReturnStmt(exp);

  return ret;
}
STMT_DEFAULT(ReturnStmt);

// ===============================================================================
/// Visitor of a SgRewindStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgRewindStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgRewindStatement*      ret =  new SgRewindStatement(DEFAULT_FILE_INFO);
  SgExpression*           unt = 0;

  // The attributes by attribSgIOStatement() are simply ignored
  // because unit is always required and the others are not used.

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( unt==0 )
        unt = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  if( unt ) {
    ret->set_unit( unt );
    unt->set_parent(ret);
    unt->set_startOfConstruct(DEFAULT_FILE_INFO);
  }

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
  ret->set_stop_or_pause( (SgStopOrPauseStatement::stop_or_pause_enum) typ );
  ret->set_parent(astParent);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      if( cod )
        cod = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();
  if( cod==0 )
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
}
INODE_STMT_DEFAULT(StopOrPauseStatement);

// ===============================================================================
/// Visitor of a SgSwitchStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgSwitchStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgSwitchStatement*    ret  = 0;
  SgStatement*          item = 0;
  SgStatement*          body = 0;

  SUBTREE_VISIT_BEGIN(node,astchild,0)
    {
      // assuming the order
      if(item==0)
        item = isSgStatement(astchild);
      else if (body==0)
        body = isSgStatement(astchild);
    }
  SUBTREE_VISIT_END();

  ret = sb::buildSwitchStatement(item,body);

  return ret;
}
STMT_DEFAULT(SwitchStatement);

// ===============================================================================
/// Visitor of a SgWaitStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgWaitStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgWaitStatement*     ret = new SgWaitStatement(DEFAULT_FILE_INFO);
  SgExpression*           iost = 0;
  SgExpression*           err = 0;
  SgExpression*           unt = 0;

  int f_ios = 0;
  int f_err = 0;

  XmlGetAttributeValue(node,"iostat",&f_ios);
  XmlGetAttributeValue(node,"err",   &f_err);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      //assuming these stmts appear in this order
      if( unt==0 )
        unt = isSgExpression(astchild);
      else if( f_ios && iost==0 )
        iost = isSgExpression(astchild);
      else if( f_err && err==0 )
        err = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_parent(astParent);
  if(unt){
    unt->set_parent(ret);
    unt->set_startOfConstruct(DEFAULT_FILE_INFO);
    ret->set_unit(unt);
  }
  if(iost){
    iost->set_parent(ret);
    iost->set_startOfConstruct(DEFAULT_FILE_INFO);
    ret->set_iostat(iost);
  }
  if(err){
    err->set_parent(ret);
    err->set_startOfConstruct(DEFAULT_FILE_INFO);
    ret->set_err(err);
  }
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

  return ret;
}
/** XML attribute writer of SgWhereStmt */
void XevSageVisitor::attribSgWhereStatement(SgNode* node)
{
  SgWhereStatement*   n = isSgWhereStatement(node);

  if(n) {
    sstr() << " end=\"" << n->get_has_end_statement() << "\" ";
  }
  attribSgStatement(sstr(),n);
}
INODE_STMT_DEFAULT(WhereStatement)

// ===============================================================================
/// Visitor of a SgWhileStmt element in an XML document
SgNode*
XevXmlVisitor::visitSgWhileStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgWhileStmt*          ret   = 0;
  SgExprStatement*      cond  = 0;
  SgStatement*          tstmt = 0;
  SgStatement*          fstmt = 0;
  int enddo=0;
  int nlabel=0;
  string slabel;

  XmlGetAttributeValue(node,"end",&enddo);
  XmlGetAttributeValue(node,"slabel",&slabel);
  XmlGetAttributeValue(node,"elabel",&nlabel);

  SUBTREE_VISIT_BEGIN(node,astchild,0)
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

  ret = sb::buildWhileStmt(cond,tstmt,fstmt);
  if(cond)  cond->set_parent(ret);
  if(tstmt) tstmt->set_parent(ret);
  if(fstmt) fstmt->set_parent(ret);

  if(enddo){
    ret->set_has_end_statement(true);
  }
  if( slabel.size() )
    ret->set_string_label( slabel );
  else if( nlabel) {
    SgLabelSymbol*  s = new SgLabelSymbol();
    s->set_fortran_statement( new SgStatement(astParent->get_file_info()) );
    s->get_fortran_statement()->set_parent(s);
    s->set_label_type( SgLabelSymbol::e_non_numeric_label_type );
    s->set_numeric_label_value( nlabel );
    SgLabelRefExp*  l = new SgLabelRefExp( s );
    ret->set_end_numeric_label( l );
  }

  return ret;
}
/** XML attribute writer of SgWhileStmt */
void XevSageVisitor::attribSgWhileStmt(SgNode* node)
{
  SgWhileStmt*   n = isSgWhileStmt(node);
  SgLabelRefExp* l = 0;

  if(n) {
    sstr() << " end=\"" << n->get_has_end_statement() << "\" ";
    l = n->get_end_numeric_label();
    if(l){
      sstr() << " elabel=\"" << l->get_numeric_label_value() << "\" ";
    }
    else {
      sstr() << " slabel=\"" << n->get_string_label() << "\" ";
    }
  }
}
INODE_STMT_DEFAULT(WhileStmt)

// ===============================================================================
/// Visitor of a SgWriteStatement element in an XML document
SgNode*
XevXmlVisitor::visitSgWriteStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgWriteStatement*     ret = new SgWriteStatement(DEFAULT_FILE_INFO);
  SgExprListExp*        exp = 0;
  SgExpression*         fmt = 0;
  SgExpression*         iost = 0;
  SgExpression*         rec = 0;
  SgExpression*         err = 0;
  SgExpression*         unt = 0;
  SgExpression*         nml = 0;

  //xe::DOMNamedNodeMap*  amap = node->getAttributes();
  //xe::DOMNode*          nameatt = 0;
  //string                nlabel;

  int f_fmt = 0;
  int f_ios = 0;
  int f_rec = 0;
  int f_err = 0;
  int f_nml = 0;

  // ignore unit attribute written by attribSgIOStatement
  XmlGetAttributeValue(node,"fmt",   &f_fmt);
  XmlGetAttributeValue(node,"iostat",&f_ios);
  XmlGetAttributeValue(node,"rec",   &f_rec);
  XmlGetAttributeValue(node,"err",   &f_err);
  XmlGetAttributeValue(node,"nml",   &f_nml);

  SUBTREE_VISIT_BEGIN(node,astchild,ret)
    {
      //assuming these stmts appear in this order
      if( exp==0 )
        exp = isSgExprListExp(astchild);
      else if( unt==0 )
        unt = isSgExpression(astchild);
      else if( f_fmt && fmt==0 )
        fmt = isSgExpression(astchild);
      else if( f_ios && iost==0 )
        iost = isSgExpression(astchild);
      else if( f_rec && rec==0 )
        rec = isSgExpression(astchild);
      else if( f_err && err==0 )
        err = isSgExpression(astchild);
      else if( f_nml && nml==0 )
        nml = isSgExpression(astchild);
    }
  SUBTREE_VISIT_END();

  ret->set_io_stmt_list(exp);
  ret->set_format( fmt );
  ret->set_iostat( iost );
  ret->set_rec( rec );
  ret->set_err( err );
  ret->set_io_statement(SgIOStatement::e_write);
  ret->set_unit(unt);
  ret->set_namelist(nml);
  if(exp)exp->set_parent(ret);
  if(fmt)fmt->set_parent(ret);
  if(iost)iost->set_parent(ret);
  if(rec)rec->set_parent(ret);
  if(err)err->set_parent(ret);
  if(nml)nml->set_parent(ret);
  return ret;
}
/** XML attribute writer of SgWriteStatement */
void XevSageVisitor::attribSgWriteStatement(SgNode* node)
{
  SgWriteStatement* n = isSgWriteStatement(node);
  if(n) {
    if( n->get_format() )
      sstr() << " fmt=\"1\"";
    if( n->get_iostat() )
      sstr() << " iostat=\"1\"";
    if( n->get_rec() )
      sstr() << " rec=\"1\"";
    if( n->get_err() )
      sstr() << " err=\"1\"";
    if( n->get_namelist() )
      sstr() << " nml=\"1\"";
  }
}
INODE_STMT_DEFAULT(WriteStatement);
