#include "xevxml.hpp"
//#include "xml2ast.hpp"
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

using namespace std;
//namespace xe=xercesc;
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


int main(int argc,char** argv)
{
  stringstream istr;
  stringstream ostr;
  char c;
  if( argc < 2 ) {
    cerr << "USAGE:" << argv[0] << " [xslt_file_name] " << endl;
    return -1;
  }
  string fn(argv[1]);

  xevxml::XmlInitialize();

  while((c=cin.get()) != cin.eof()){
    istr << c;
  }

  XsltTransform(istr,ostr,fn);

  cout << ostr.str();

  xevxml::XmlFinalize();  
  return 0;
}


