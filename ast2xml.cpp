/*!
* @file     ast2xml.cpp
* @brief    ROSE Sage III AST is converted to XML.
*/
#include "xevxml.hpp"
#include "ast2xml.hpp"
#include "attrib.hpp"
#include <string>

using namespace std;

string REPLACE( string str1, string str2, string str3 )
{
  string::size_type  Pos( str1.find( str2 ) );

  while( Pos != string::npos ){
    str1.replace( Pos, str2.length(), str3 );
    Pos = str1.find( str2, Pos + str3.length() );
  }
  
  return str1;
}


string REP_XML( string str )
{
    str = REPLACE( str,"&","&amp;" );
    str = REPLACE( str,"<","&lt;" );
    str = REPLACE( str,">","&gt;" );
    str = REPLACE( str,"\"","&quot;" );
    str = REPLACE( str,"\'","&apos;" );
    return str;
}


void Ast2Xml(stringstream& sstr, SgProject* prj, int fileid)
{
  Ast2XmlVisitor visitor(sstr);
  Ast2XmlInheritedAttribute att;
  visitor.traverseWithinFile(&prj->get_file(fileid),att);
  
  return;
}

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

  return false;
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
  sstr << " address=\"";
  sstr.setf(ios::hex,ios::basefield);
  sstr << t << "\"";
  sstr.unsetf(ios::hex);
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
      writeTypesRecursive(sstr,types[i], att,false);
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
      visitor.traverse(isSgInquireStatement(n)->get_iolengthExp(),att);
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
  Ast2XmlInheritedAttribute retatt;
  SgLocatedNode* loc = isSgLocatedNode(node);
  AttachedPreprocessingInfoType* info=0; 
  if(loc) info = loc->getAttachedPreprocessingInfo();

  for(int i(0);i<att.level;i++)
    sstr_ << "  "; // indent
  retatt.level = att.level+1;
  sstr_ << '<';
  sstr_ << node->class_name();  
  sstr_ << " address=\"";
  sstr_.setf(ios::hex,ios::basefield);
  sstr_ << node << "\"" ;
  sstr_.unsetf(ios::hex);

  /* write attributes of this element */
  writeXmlAttribs(sstr_,node);

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
  SgLocatedNode* loc = isSgLocatedNode(node);
  AttachedPreprocessingInfoType* info=0; 
  std::string str;
  int    idx;

  if(loc) {
    info = loc->getAttachedPreprocessingInfo();
    if(info) {
      for(size_t i(0);i<(*info).size();i++) {
       
        str = (*info)[i]->getString();
        idx = str.find( "$PRAGMA" );
        if( idx < 0 )
            idx = str.find( "$pragma");

        if( idx > 0 ) {
          sstr_ << "<SgPragmaDeclaration >\n";
          sstr_ << "  "; // indent
          sstr_ << "<SgPragma pragma=\"";
          sstr_ << str.substr( idx+7 ) << "\" />\n";
          sstr_ << "</SgPragmaDeclaration >\n";

        }
        else {
          str = (*info)[i]->getString();
          str = REP_XML( str );                     
          sstr_ << "<PreprocessingInfo pos=\"";
          sstr_ << (*info)[i]->getRelativePosition() <<"\" ";
          sstr_ << " type=\"";
          sstr_ << (*info)[i]->getTypeOfDirective() << "\" >\n";
          //sstr_ << (*info)[i]->getString() << "\n";
          sstr_ << str << "\n";
          sstr_ << "</PreprocessingInfo>\n";
        }
      }
    }
  }

  if ( isLeafNode(node) == false || info != 0) {
    for(int i(0);i<att.level-1;i++)
      sstr_ << ' ' << ' '; // indent
    sstr_ << "</";
    sstr_ << node->class_name() << '>' << endl;
  }
  return;
}


