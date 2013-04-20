#include "xevxml.hpp"
#include "xml2ast.hpp"

using namespace std;
namespace xe=xercesc;
namespace xa=xalanc;

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

