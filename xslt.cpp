#include "xevxml.hpp"
#include "xml2ast.hpp"

using namespace std;
namespace xe=xercesc;
namespace xa=xalanc;

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
    class Xml2AstVisitor visit(ofn,ofn,prj);
    visit.visit(doc,0);
  }
  catch (...){
    return 1;
  }
  return 0;
}


void XsltTransform(stringstream& istr, stringstream& ostr, string xsltfn) 
{
  // work with Xalan-C++ 
  try {
    //xalanc::XSLTInputSource xmlIn(str);
    xa::XalanTransformer transformer;
    if( 0 != transformer.transform(istr,xsltfn.c_str(),ostr)){
      cerr << "Error: XalanTransformer::transform() failed" << endl;
      ABORT();
    }
  }
  catch(...){
    cerr << "Error: XalanTransformer::transform() failed" << endl;
    ABORT();
  }
}

void Xml2Ast(stringstream& str, SgProject* prj, string ofn)
{
  if(Xml2AstDOMVisit(str,prj,ofn)==1){
    cerr << "Error: DOMXMLReader failed" << endl;
    ABORT();
  }
  // Other terminations and cleanup.
  return;
}
