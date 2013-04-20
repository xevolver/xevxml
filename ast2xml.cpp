#include "ast2xml.hpp"
#include "attrib.hpp"

using namespace std;

void Ast2Xml(stringstream& sstr, SgProject* prj, int fileid)
{
  Ast2XmlVisitor visitor(sstr);
  Ast2XmlInheritedAttribute att;
  visitor.traverseWithinFile(&prj->get_file(fileid),att);
  
  return;
}

static bool hasInternalNode(SgNode* n)
{
  if(isSgArrayType(n)){
    return true;
  }
  if(isSgFunctionParameterList(n)){
    return true;
  }

  return false;
}

static void writeInternalNode(stringstream& sstr,
			      SgNode* n, 
			      Ast2XmlInheritedAttribute att)
{
  if(hasInternalNode(n)==false) return;

  Ast2XmlVisitorInternal visitor(sstr);
  if(isSgArrayType(n))
    visitor.traverse(isSgArrayType(n)->get_dim_info(),att);
  
  else if(isSgFunctionParameterList(n)){
    SgFunctionParameterList* plst = isSgFunctionParameterList(n);
    SgInitializedNamePtrList& args = plst->get_args();
    // args.size() must be divided by sizeof(void*) ???
    for(size_t i(0);i<args.size()/sizeof(void*);i++)
      visitor.traverse(args[i],att);
  }

  return;
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

  if(t->containsInternalTypes()==true && f){
    sstr << ">" << endl;
    Rose_STL_Container<SgType*> types = t->getInternalTypes();
    att.level += 1;
    for(size_t i(0);i<types.size();++i){
      writeTypesRecursive(sstr,types[i], att,false);
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
  if(loc) {
    info = loc->getAttachedPreprocessingInfo();
    if(info) {
      for(size_t i(0);i<(*info).size();i++) {
	sstr_ << "<PreprocessingInfo pos=\"";
	sstr_ << (*info)[i]->getRelativePosition() <<"\" ";
	sstr_ << " type=\"";
	sstr_ << (*info)[i]->getTypeOfDirective() << "\" >\n";
	sstr_ << (*info)[i]->getString() << "\n";
	sstr_ << "</PreprocessingInfo>\n";
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


