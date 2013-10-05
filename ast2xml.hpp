#ifndef ___AST2XML_HPP___
#define ___AST2XML_HPP___

#include <rose.h>

namespace xevxml {

class Ast2XmlInheritedAttribute
{
public:
  int level;
  Ast2XmlInheritedAttribute ():level(0) {};
};

class Ast2XmlVisitorInternal: public AstTopDownProcessing <Ast2XmlInheritedAttribute>
{
protected:
  std::stringstream& sstr_;

  Ast2XmlInheritedAttribute 
  evaluateInheritedAttribute(SgNode* node,
			     Ast2XmlInheritedAttribute att);
  
  void 
  destroyInheritedValue (SgNode* node,
			 Ast2XmlInheritedAttribute att);
  
public:
  Ast2XmlVisitorInternal(std::stringstream& s): sstr_(s) {}
  ~Ast2XmlVisitorInternal() {}
};

class Ast2XmlVisitor:public Ast2XmlVisitorInternal
{
public:
  Ast2XmlVisitor(std::stringstream& s): Ast2XmlVisitorInternal(s){
    sstr_ << "<?xml version=\"1.0\" encoding=\"shift_jis\"?>" << std::endl;
  }
  ~Ast2XmlVisitor() {}
protected:
  void atTraversalStart() {/* do nothing */}
  void atTraversalEnd() 
  {
    sstr_<<std::endl<<std::ends;
  }
};

extern void Ast2Xml(std::stringstream& str, SgProject* prj, int fileid=0);
}


#endif


