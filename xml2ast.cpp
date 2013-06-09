#include "xevxml.hpp"
#include "xml2ast.hpp"

namespace sb=SageBuilder;
namespace si=SageInterface;
namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;


#define VISIT(x) if(nname==#x) { return visit##x (node,astParent);}

static int Xml2AstDOMVisit(stringstream& tr, SgProject* prj, string ofn)
{
  try {
    xe::XercesDOMParser parser;
    string buf = tr.str();
    xe::MemBufInputSource 
      membuf((const XMLByte*)buf.c_str(), buf.length(), "memory_buffer");
    parser.parse(membuf);
    xe::DOMDocument* doc = parser.getDocument();

    //class Xml2AstVisitor visit(prj->get_file(0).getFileName(),ofn.c_str(),prj);
    class Xml2AstVisitor visit("dummy.c",ofn.c_str(),prj);
    visit.visit(doc,0);
  }
  catch (...){
    return 1;
  }
  return 0;
}


void Xml2Ast(stringstream& str, SgProject* prj, string ofn)
{
  if(Xml2AstDOMVisit(str,prj,ofn)==1){
    cerr << "Error: XML parsing failed" << endl;
    ABORT();
  }
  // Other terminations and cleanup.
  return;
}


Xml2AstVisitor::
Xml2AstVisitor(const string& ifn, const string& ofn, SgProject* prj)
{
  _file = isSgSourceFile(sb::buildFile(ifn,ofn,prj));
  if(_file==0){ ABORT(); }
}

Xml2AstVisitor::
~Xml2AstVisitor() {}


SgNode* 
Xml2AstVisitor::visit(xe::DOMNode* node, SgNode* astParent)
{
  if(node) {
    if (node->getNodeType() == xe::DOMNode::DOCUMENT_NODE){
      xe::DOMNode* child=node->getFirstChild();
      while(child) {
	this->visit(child);
	xe::DOMNode* next=child->getNextSibling();
	child=next;
      } 
    }
    else {
      string nname = xe::XMLString::transcode(node->getNodeName());
      
      VISIT(SgSourceFile);
      VISIT(SgGlobal);
      VISIT(SgBasicBlock);
      VISIT(SgPragmaDeclaration);
      VISIT(SgPragma);
      VISIT(SgVariableDeclaration);
      VISIT(SgFunctionDeclaration);
      VISIT(SgFunctionParameterList);
      VISIT(SgInitializedName);
      VISIT(SgAssignInitializer);

      VISIT(SgBoolValExp);
      VISIT(SgCharVal);
      //VISIT(SgWcharVal);
      //VISIT(SgComplexVal);
      VISIT(SgDoubleVal);
      VISIT(SgFloatVal);
      VISIT(SgIntVal);
      VISIT(SgLongIntVal);
      VISIT(SgLongLongIntVal);
      //VISIT(SgEnumVal);
      VISIT(SgLongDoubleVal);
      VISIT(SgShortVal);
      VISIT(SgStringVal);
      VISIT(SgUnsignedCharVal);
      VISIT(SgUnsignedShortVal);
      VISIT(SgUnsignedIntVal);
      VISIT(SgUnsignedLongVal);
      VISIT(SgUnsignedLongLongIntVal);

      VISIT(SgTypeInt);
      VISIT(SgTypeFloat);
      VISIT(SgTypeDouble);
      VISIT(SgModifierType);
      VISIT(SgPointerType);
     
      VISIT(SgExprStatement);
      VISIT(SgForStatement);
      VISIT(SgForInitStatement);
      VISIT(SgIfStmt);
      VISIT(SgReturnStmt);
      VISIT(SgFunctionDefinition);
      
      VISIT(SgVarRefExp);
      VISIT(SgCastExp);

      VISIT(SgPlusPlusOp);
      VISIT(SgMinusOp);
      VISIT(SgAssignOp);
      VISIT(SgPlusAssignOp);
      VISIT(SgMultAssignOp);
      VISIT(SgAddOp);
      VISIT(SgLessThanOp);

      if( nname != "PreprocessingInfo" ) {
	cerr << "unknown AST node found " << nname << endl;
	ABORT();
      }
    }
  }
  return 0;
}

SgNode* 
Xml2AstVisitor::visitSgSourceFile(xe::DOMNode* node, SgNode* astParent)
{
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      this->visit(child,_file);
    }
    child=child->getNextSibling();
  } 
  return _file;
}

SgNode* 
Xml2AstVisitor::visitSgGlobal(xe::DOMNode* node, SgNode* astParent)
{
  SgGlobal* ret = _file->get_globalScope();
  sb::pushScopeStack(ret);

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgStatement* astchild = isSgStatement(this->visit(child,astParent));
      if(astchild)
	si::appendStatement (astchild,isSgScopeStatement(ret));
    }
    child=child->getNextSibling();
  } 
  sb::popScopeStack();
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgPragmaDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgPragmaDeclaration* ret = 0;
  SgPragma* pr = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if(pr==0)
	pr = isSgPragma(astchild);
    }
    child=child->getNextSibling();
  } 
  if(pr) {
    ret = sb::buildPragmaDeclaration(pr->get_pragma()); 
  }
  else ABORT();
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgPragma(xe::DOMNode* node, SgNode* astParent)
{
  SgPragma* ret = 0;
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* att = 0;
  string line;

  if(amap) {
    att=amap->getNamedItem(xe::XMLString::transcode("pragma"));
    if(att)
      line = xe::XMLString::transcode(att->getNodeValue());
  }

  if(line.size()) {
    ret = sb::buildPragma(line); 
  }
  else ABORT();
  
  return ret;
}


SgNode* 
Xml2AstVisitor::visitSgVariableDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgVariableDeclaration* ret = 0;
  SgInitializedName* name =0;
  
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* satt = 0;
  string storage;
  
  if(amap) {
    satt=amap->getNamedItem(xe::XMLString::transcode("modifier"));
    if(satt)
      storage = xe::XMLString::transcode(satt->getNodeValue());
  }
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if(name==0)
	name = isSgInitializedName(astchild);
    }
    child=child->getNextSibling();
  } 
  if(name) {
    ret = sb::buildVariableDeclaration(name->get_name(), 
				       name->get_type(),
				       name->get_initializer());
  }
  else ABORT();

  if(storage=="unknown")
    ((ret->get_declarationModifier()).get_storageModifier()).setUnknown();
  else if(storage=="static")
    ((ret->get_declarationModifier()).get_storageModifier()).setStatic();
  else if(storage=="extern")
    ((ret->get_declarationModifier()).get_storageModifier()).setExtern();
  else if(storage=="auto")
    ((ret->get_declarationModifier()).get_storageModifier()).setAuto();
  else if(storage=="register")
    ((ret->get_declarationModifier()).get_storageModifier()).setRegister();
  else if(storage=="typedef")
    ((ret->get_declarationModifier()).get_storageModifier()).setTypedef();
  else 
    ((ret->get_declarationModifier()).get_storageModifier()).setDefault();
  
  checkPreprocInfo(node,ret);
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgFunctionDeclaration(xe::DOMNode* node, SgNode* astParent)
{
  SgFunctionDeclaration*   ret = 0;
  SgFunctionParameterList* lst = 0;
  //SgFunctionDefinition*    def = 0;
  SgBasicBlock*            def = 0;
  SgType*                  typ = 0;
  
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* satt = 0;
  string storage,name;
  
  if(amap) {
    satt=amap->getNamedItem(xe::XMLString::transcode("modifier"));
    if(satt)
      storage = xe::XMLString::transcode(satt->getNodeValue());
    satt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(satt)
      name = xe::XMLString::transcode(satt->getNodeValue());
  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if(lst==0)
	lst = isSgFunctionParameterList(astchild);
      if(def==0)
	def = isSgBasicBlock(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
    child=child->getNextSibling();
  } 
  if(lst) {
    if(def)
      ret 
	= sb::buildDefiningFunctionDeclaration(SgName(name.c_str()), typ, lst);
    else 
      ret 
	= sb::buildNondefiningFunctionDeclaration(SgName(name.c_str()), typ, lst);
  }
  else ABORT();

  if(storage=="unknown")
    ((ret->get_declarationModifier()).get_storageModifier()).setUnknown();
  else if(storage=="static")
    ((ret->get_declarationModifier()).get_storageModifier()).setStatic();
  else if(storage=="extern")
    ((ret->get_declarationModifier()).get_storageModifier()).setExtern();
  else if(storage=="auto")
    ((ret->get_declarationModifier()).get_storageModifier()).setAuto();
  else if(storage=="register")
    ((ret->get_declarationModifier()).get_storageModifier()).setRegister();
  else if(storage=="typedef")
    ((ret->get_declarationModifier()).get_storageModifier()).setTypedef();
  else 
    ((ret->get_declarationModifier()).get_storageModifier()).setDefault();
  
  checkPreprocInfo(node,ret);
  
  si::replaceStatement(ret->get_definition()->get_body(),def,true);
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgFunctionParameterList(xercesc::DOMNode* node, SgNode* astParent)
{
  SgFunctionParameterList* ret = sb::buildFunctionParameterList();
  SgInitializedName* ini=0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if((ini = isSgInitializedName(astchild)) != 0 ){
	si::appendArg(ret,ini);
      }
    }
    child=child->getNextSibling();
  } 
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgBasicBlock(xe::DOMNode* node, SgNode* astParent)
{
  SgBasicBlock* ret = sb::buildBasicBlock();
  
  sb::pushScopeStack(ret);
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if(isSgStatement(astchild)){
	//SageInterface::appendStatement(isSgStatement(astchild));
	ret->append_statement(isSgStatement(astchild));
      }
    }
    child=child->getNextSibling();
  } 
  sb::popScopeStack();
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgExprStatement(xe::DOMNode* node, SgNode* astParent)
{
  SgExprStatement* ret = 0;

  SgExpression* exp = 0;
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if(exp==0)
	exp = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildExprStatement(exp);

  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgForStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForStatement* ret = 0;
  SgStatement* ini  = 0;
  SgStatement* test = 0;
  SgExpression* inc = 0;
  SgStatement* body = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      /* assuming these stmts appear in this order */
      if(ini==0)
	ini = isSgStatement(astchild);
      else if (test==0)
	test = isSgStatement(astchild);
      else if (inc==0)
	inc = isSgExpression(astchild);
      else if(body==0)
	body = isSgStatement(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildForStatement(ini,test,inc,body);
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgForInitStatement(xercesc::DOMNode* node, SgNode* astParent)
{
  SgForInitStatement* ret = 0;
  SgStatement* stmt  = 0;
  SgStatementPtrList lst;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if((stmt = isSgStatement(astchild))!=0)
	lst.push_back(stmt);
    }
    child=child->getNextSibling();
  } 
  if(lst.size())
    ret = sb::buildForInitStatement(lst);
  else
    ABORT();
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgIfStmt(xercesc::DOMNode* node, SgNode* astParent)
{
  SgIfStmt* ret = 0;
  SgStatement* tstmt  = 0;
  SgStatement* fstmt  = 0;
  SgExprStatement* cond  = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      /* assuming these stmts appear in this order */
      if(cond==0)
	cond = isSgExprStatement(astchild);
      else if (tstmt==0)
	tstmt = isSgStatement(astchild);
      else if (fstmt==0)
	fstmt = isSgStatement(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildIfStmt(cond,tstmt,fstmt);

  return ret;
}

#define VISIT_BINARY_OP(op)						\
  SgNode*								\
  Xml2AstVisitor::visitSg##op(xercesc::DOMNode* node, SgNode* astParent) \
  {									\
    SgExpression* lhs = 0;						\
    SgExpression* rhs = 0;						\
    xe::DOMNode* child=node->getFirstChild();				\
    while(child) {							\
      if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){	\
	SgNode* astchild = this->visit(child);				\
	if(lhs==0)							\
	  lhs = isSgExpression(astchild);				\
	else if(rhs==0)							\
	  rhs = isSgExpression(astchild);				\
      }									\
      child=child->getNextSibling();					\
    }									\
    if( lhs && rhs )							\
      return sb::build##op(lhs,rhs);					\
    else								\
      ABORT();								\
  }									

#define VISIT_UNARY_OP(op)						\
  SgNode*								\
  Xml2AstVisitor::visitSg##op(xercesc::DOMNode* node, SgNode* astParent) \
  {									\
    SgExpression* exp = 0;						\
    xe::DOMNode* child=node->getFirstChild();				\
    while(child) {							\
      if(child->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){	\
	SgNode* astchild = this->visit(child);				\
	if(exp==0)							\
	  exp = isSgExpression(astchild);				\
      }									\
      child=child->getNextSibling();					\
    }									\
    if( exp )							\
      return sb::build##op(exp);					\
    else								\
      ABORT();								\
  }									
  

VISIT_UNARY_OP(PlusPlusOp);
VISIT_UNARY_OP(MinusOp);
VISIT_BINARY_OP(AssignOp);
VISIT_BINARY_OP(PlusAssignOp);
VISIT_BINARY_OP(MultAssignOp);
VISIT_BINARY_OP(AddOp);
VISIT_BINARY_OP(LessThanOp);


SgNode* 
Xml2AstVisitor::visitSgVarRefExp(xe::DOMNode* node, SgNode* astParent)
{
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* nameatt = 0;
  string name;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }
  if(name.size())
    return sb::buildVarRefExp(name);
  else 
    ABORT();
}

SgNode* 
Xml2AstVisitor::visitSgCastExp(xe::DOMNode* node, SgNode* astParent)
{
  SgType* typ = 0;
  SgExpression* exp = 0;

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if(exp==0)
	exp = isSgExpression(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
    child=child->getNextSibling();
  } 
  if(typ && exp)
    return sb::buildCastExp(exp,typ);
  else 
    ABORT();
}

SgNode* 
Xml2AstVisitor::visitSgReturnStmt(xe::DOMNode* node, SgNode* astParent)
{
  SgReturnStmt* ret = 0;
  SgExpression* exp = 0;
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,ret);
      if(exp==0)
	exp = isSgExpression(astchild);
    }
    child=child->getNextSibling();
  } 
  ret = sb::buildReturnStmt(exp);

  return ret;
}

/* NOTE: this function returns SgBasicBlock */
SgNode* 
Xml2AstVisitor::visitSgFunctionDefinition(xe::DOMNode* node, SgNode* astParent)
{
  SgBasicBlock* ret = 0;
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child);
      if(ret == 0 ){
	ret = isSgBasicBlock(astchild);
      }
    }
    child=child->getNextSibling();
  } 
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgInitializedName(xe::DOMNode* node, SgNode* astParent)
{
  SgInitializedName* ret = 0;
  SgInitializer* ini = 0;
  SgType* typ=0;
  
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* nameatt = 0;
  string name;
  
  if(amap) {
    nameatt=amap->getNamedItem(xe::XMLString::transcode("name"));
    if(nameatt)
      name = xe::XMLString::transcode(nameatt->getNodeValue());
  }

  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if(ini==0)
	ini = isSgInitializer(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
    child=child->getNextSibling();
  } 
  
  if(name.size()&&typ){
    ret = sb::buildInitializedName(name.c_str(),typ,ini);
  }
  else ABORT();
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgAssignInitializer(xe::DOMNode* node, SgNode* astParent)
{
  SgAssignInitializer* ret = 0;
  SgExpression* exp = 0;
  SgType* typ=0;
  
  xe::DOMNode* child=node->getFirstChild();
  while(child) {
    if(child->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      SgNode* astchild = this->visit(child,astParent);
      if(exp==0)
	exp = isSgExpression(astchild);
      if(typ==0)
	typ = isSgType(astchild);
    }
    child=child->getNextSibling();
  } 

  if(exp && typ){
    ret = sb::buildAssignInitializer(exp,typ);
  }
  else ABORT();
  
  return ret;
}

#define VISIT_VAL(valType,baseType,buildVal)				\
  SgNode* Xml2AstVisitor::						\
  visit##valType(xe::DOMNode* node, SgNode* astParent)			\
  {									\
    valType* ret = 0;							\
    xe::DOMNamedNodeMap* amap = node->getAttributes();			\
    xe::DOMNode* valatt = 0;						\
    stringstream val;							\
    if(amap) {								\
      valatt=amap->getNamedItem(xe::XMLString::transcode("value"));	\
      if(valatt)							\
	val << xe::XMLString::transcode(valatt->getNodeValue());	\
    }									\
    if(val.str().size()){						\
      baseType ival;							\
      val >> ival;							\
      ret = sb::build##buildVal(ival);					\
    }									\
    else ABORT();							\
    return ret;								\
  }


VISIT_VAL(SgBoolValExp,bool,BoolValExp);
VISIT_VAL(SgCharVal,char,CharVal);
//VISIT_VAL(SgWcharVal,wchar_t,WcharVal);
//VISIT_VAL(SgComplexVal);
VISIT_VAL(SgDoubleVal,double,DoubleVal);
VISIT_VAL(SgFloatVal,float,FloatVal);
VISIT_VAL(SgIntVal,int,IntVal);
VISIT_VAL(SgLongIntVal,long int,LongIntVal);
VISIT_VAL(SgLongLongIntVal,long long int,LongLongIntVal);
//VISIT_VAL(SgEnumVal);
VISIT_VAL(SgLongDoubleVal,long double,LongDoubleVal);
VISIT_VAL(SgShortVal,short,ShortVal);
VISIT_VAL(SgStringVal,string,StringVal);
VISIT_VAL(SgUnsignedCharVal,unsigned char,UnsignedCharVal);
VISIT_VAL(SgUnsignedShortVal,unsigned short,UnsignedShortVal);
VISIT_VAL(SgUnsignedIntVal,unsigned int, UnsignedIntVal);
VISIT_VAL(SgUnsignedLongVal,unsigned long, UnsignedLongVal);
VISIT_VAL(SgUnsignedLongLongIntVal,unsigned long long,UnsignedLongLongIntVal);

/*
VISIT_VAL(SgIntVal,int,IntVal);
VISIT_VAL(SgDoubleVal,double,DoubleVal);
VISIT_VAL(SgCharVal,char,CharVal);
VISIT_VAL(SgFloatVal,float,FloatVal);
*/

#if 0
SgNode* 
Xml2AstVisitor::visitSgIntVal(xe::DOMNode* node, SgNode* astParent)
{
  SgIntVal* ret = 0;
  
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* valatt = 0;
  std::stringstream val;
  
  if(amap) {
    valatt=amap->getNamedItem(xe::XMLString::transcode("value"));
    if(valatt)
      val << xe::XMLString::transcode(valatt->getNodeValue());
  }
  if(val.str().size()){
    int ival;
    val >> ival;
    ret = sb::buildIntVal(ival);
  }
  else ABORT();
  
  return ret;
}

SgNode* 
Xml2AstVisitor::visitSgDoubleVal(xe::DOMNode* node, SgNode* astParent=0)
{
  SgDoubleVal* ret = 0;
  
  xe::DOMNamedNodeMap* amap = node->getAttributes();
  xe::DOMNode* valatt = 0;
  stringstream val;
  
  if(amap) {
    valatt=amap->getNamedItem(xe::XMLString::transcode("value"));
    if(valatt)
      val << xe::XMLString::transcode(valatt->getNodeValue());
  }
  
  if(val.str().size()){
    double ival;
    val >> ival;
    ret = sb::buildDoubleVal(ival);
  }
  else ABORT();
  
  return ret;
}

#endif
