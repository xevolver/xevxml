#include <rose.h>
#include "attrib.hpp"

using namespace std;

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
  if(n) {							
    istr << " real=\"" << n->get_real_value() << "\" ";		
    istr << " imaginary=\"" << n->get_imaginary_value() << "\" ";		
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
DEFINE_VALUE_EXP(SgStringVal);
DEFINE_VALUE_EXP(SgUnsignedCharVal);
DEFINE_VALUE_EXP(SgUnsignedShortVal);
DEFINE_VALUE_EXP(SgUnsignedIntVal);
DEFINE_VALUE_EXP(SgUnsignedLongVal);
DEFINE_VALUE_EXP(SgUnsignedLongLongIntVal);

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
      istr << " modifier=\"extern\" " ;
    else if(m.isAuto())
      istr << " modifier=\"auto\" " ;
    else if(m.isRegister())
      istr << " modifier=\"register\" ";
    else if(m.isTypedef())
      istr << " modifier=\"typedef\" ";
  }
}

void writeXmlAttribs(stringstream& istr,SgNode* node)
{
  writeValueAttribs(istr,node);

  attribSgInitializedName(istr,node);
  attribSgVarRefExp(istr,node);
  attribSgPragma(istr,node);
  attribSgVariableDeclaration(istr,node);
  attribSgFunctionDeclaration(istr,node);

  return;
}
