#include "dir2xml.hpp"
#include "direlms.hpp"

using namespace std;
using namespace boost;
using namespace boost::spirit;

using namespace xevxml;
namespace xe=xercesc;
namespace xa=xalanc;


template<typename Iterator>
struct ReadDir: qi::grammar<Iterator,DirAST()>
{
  qi::rule<Iterator,DirAST()> line,dirname,clause,lst,arg;
  qi::rule<Iterator,string()> commasep,claussep,var,exp;

  /* grammer definition of OpenMP-like directives */
  ReadDir() : ReadDir::base_type(line)
  {
    line = D_PREFIX >> +qi::space   // prefix
		    >> dirname [_val = _1 << D_DIRECTIVE ]  // directive name
		    >> *(claussep >> clause[_val += _1 << D_CLAUSE ]);  //clauses
    
    // dirname is a string
    dirname = (qi::as_string[+(qi::char_-','-qi::space)]) [ _val = _1];

    // clause can optionally take a list of arguments
    clause = (qi::as_string[+(qi::char_-','-qi::space-'(')]) [ _val = _1]
      >> - ( *qi::space >> qi::lit('(') 
	     >> lst[_val += _1 << D_LIST ] 
	     >> *qi::space >> qi::lit(')') );

    // clause separator
    claussep = (+qi::lit(' ')) | commasep;

    // list
    lst = (arg[_val += _1 << D_ARG ]  % commasep) ;

    // arguments of a clause
    arg = qi::as_string[exp] [_val=_1];
    exp = var [_val=_1] 
      >> *qi::space 
      >> -(qi::lit('(') 
	   >> qi::as_string[*(qi::char_-')')] [_val+="("+_1+")"]
	   >> qi::lit(')')
	   );
    var = *qi::space >> as_string[+(qi::char_-','-'('-')'-qi::space)][_val=_1];

    // comma separator
    commasep = *qi::lit(' ') >> qi::lit(',') >> *qi::lit(' ');
  }

};


DirAST ParsePragmaString( std::string& str)
{
  DirAST retval;
  if( str.size() > 0 ) {
    ReadDir<std::string::iterator> reader;

    if( qi::parse(str.begin(), str.end(), reader, retval) == 0 ){
      std::cerr << "WARN @ " << __FUNCTION__ << " :";		
      std::cerr << __FILE__ ;					
      std::cerr << "(" << __LINE__ << "): \"" << str << "\" is ignored" << std::endl;		
    }
  }

  return retval;
}

static DirAST* FindClause(DirAST* dir,std::string& str)
{
  if( dir->cname == D_CLAUSE && str == dir->str )
    return dir;
  else {
    DirAST* ret=0;
    for(size_t i(0);i<dir->succ.size();i++) {
      ret = FindClause(&(dir->succ[i]),str);
      if(ret) return ret;
    }
    return ret;
  }
}

void ReplaceParams( xe::DOMDocument* doc, xe::DOMNode* node, DirAST& dir)
{
  if(node) {
    if( xe::XMLString::transcode(node->getNodeName()) == string(D_CLAUSE) ) {
      xe::DOMNamedNodeMap* amap = node->getAttributes();
      xe::DOMNode* att = 0;
      string name;

      if(amap) {
	/* number of params should be checked here */
	att=amap->getNamedItem(xe::XMLString::transcode("name"));
	if(att)
	  name = xe::XMLString::transcode(att->getNodeValue());
      }

      DirAST* clause = FindClause(&dir,name);
      if(clause && clause->succ.size() > 0){
	/* remove XML elements of default PLIST */
	xe::DOMNode* child=node->getFirstChild();
	while(child){
	  xe::DOMNode* prev = child;
	  child=child->getNextSibling();
	  //if(xe::XMLString::transcode(prev->getNodeName()) == string(D_LIST)) {
	  //if(prev->getNodeType()== xe::DOMNode::TEXT_NODE)
	  node->removeChild(prev);
	}
	/* append LI elements with specified values */
	xe::DOMElement *plst = doc-> createElement(xe::XMLString::transcode("PLIST"));
	node->appendChild((xe::DOMNode*)plst);
	for(size_t i(0);i<clause->succ[0].succ.size();i++){
	  xe::DOMElement *li = doc-> createElement(xe::XMLString::transcode("LI"));
	  li-> setAttribute(xe::XMLString::transcode("value"), 
			    xe::XMLString::transcode(clause->succ[0].succ[i].str.c_str()));
	  plst->appendChild((xe::DOMNode*)li);
	}
      }
    }
    else {
      xe::DOMNode* child=node->getFirstChild();
      while(child){
	ReplaceParams(doc,child,dir);
	child=child->getNextSibling();
      }
    }
  }
}

static std::string DirVisit(stringstream& itr, stringstream& dtr) 
{
  std::string ret;
  xe::DOMDocument* idoc = 0;
  xe::DOMDocument* ddoc = 0;
  xe::XercesDOMParser parser;
  string ibuf = itr.str();
  string dbuf = dtr.str();
  xe::MemBufInputSource 
    imembuf((const XMLByte*)ibuf.c_str(), ibuf.length(), "memory_buffer");
  xe::MemBufInputSource 
    dmembuf((const XMLByte*)dbuf.c_str(), dbuf.length(), "memory_buffer");
  parser.parse(imembuf);
  idoc = parser.getDocument();
  parser.parse(dmembuf);
  ddoc = parser.getDocument();

  xevxml::Dir2XmlVisitor visit(idoc);
  visit.getDefs(ddoc);
  visit.visit();
  stringstream ost;

  xevxml::XmlWriteToString(idoc,ost);
  ret = ost.str();
  return ret;
}

int main(int argc, char** argv)
{

  char c;
  if( argc < 2 ) {
    cerr << "USAGE:" << argv[0] << " [definition file] " << endl;
    return -1;
  }
  ifstream defs(argv[1]);
  stringstream istr;
  stringstream dstr;

  /* XML AST */
  while(cin.get(c)){
    istr << c;
  }
  /* directive definition */
  while(defs.get(c)){
    dstr << c;
  }

  xevxml::XmlInitialize();
  
  cout << DirVisit(istr,dstr);

  xevxml::XmlFinalize();
  return 0;
}

static void RemoveTextNode(xercesc::DOMNode* node)
{
  xercesc::DOMNode* child = node->getFirstChild();
  while(child) {
    xercesc::DOMNode* prev = child;
    if (prev->getNodeType() == xercesc::DOMNode::TEXT_NODE){
      node->removeChild(prev);
    }
    else RemoveTextNode(prev);
    child = child->getNextSibling();
  } 
}

namespace xevxml {
#define VISIT(x) if(nname==#x) {visit##x (node);return;}

  void Dir2XmlVisitor::visit(xercesc::DOMNode* node) {
    if(node) {
      if (node->getNodeType() == xe::DOMNode::DOCUMENT_NODE){
	xe::DOMNode* child=node->getFirstChild();
	while(child) {
	  this->visit(child);
	  child=child->getNextSibling();
	} 
      }
      else {
	string nname = xe::XMLString::transcode(node->getNodeName());
	if(node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE){
	  VISIT(SgPragma);
	}
	xercesc::DOMNode* child=node->getFirstChild();
	while(child){
	  this->visit(child);
	  child=child->getNextSibling();
	}
      }
    }
  }

  void Dir2XmlVisitor::visitSgPragma(xe::DOMNode* node) {
    xe::DOMNamedNodeMap* amap = node->getAttributes();
    xe::DOMNode* att = 0;
    string line;
    DirAST ast;
    if(amap) {
      att=amap->getNamedItem(xe::XMLString::transcode("pragma"));
      if(att)
	line = xe::XMLString::transcode(att->getNodeValue());
    }
    ast = ParsePragmaString(line);
    //ast.print();
    if(ast.str.size() > 0 ){
      for(size_t i(0);i<dnames_.size();i++){
	if(dnames_[i] == ast.str ) {
	  xe::DOMNode* newNode = defs_[i]->cloneNode(true);
	  ReplaceParams(doc_,newNode,ast);
	  RemoveTextNode(newNode);
	  node->appendChild(newNode);
	  //std::cerr << dnames_[i] << std::endl;
	}
      }
      //ast.print();
    }
  }

  void Dir2XmlVisitor::getDefs(xercesc::DOMNode* node)
  {
    if(node) {
      std::string nname = xercesc::XMLString::transcode(node->getNodeName());
      if( nname == D_DIRECTIVE ) {
	xe::DOMNamedNodeMap* amap = node->getAttributes();
	xe::DOMNode* att = 0;
	std::string name;
	if(amap) {
	  att=amap->getNamedItem(xe::XMLString::transcode("name"));
	  if(att)
	    name = xe::XMLString::transcode(att->getNodeValue());
	}
	if(name.size() > 0){
	  defs_.push_back(doc_->importNode(node,true));
	  dnames_.push_back(name);
	}
      }
      else {
	xercesc::DOMNode* child=node->getFirstChild();
	while(child){
	  this->getDefs(child);
	  child=child->getNextSibling();
	}
      }
    }
  }
}
