#include "xevxml.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;

namespace xevxml {

static string REPLACE( string str1, string str2, string str3 )
{
  string::size_type  Pos( str1.find( str2 ) );

  while( Pos != string::npos ){
    str1.replace( Pos, str2.length(), str3 );
    Pos = str1.find( str2, Pos + str3.length() );
  }
  
  return str1;
}

void XmlInitialize(void)
{
  try {
    xe::XMLPlatformUtils::Initialize();
    xa::XalanTransformer::initialize();
  }
  catch (const xe::XMLException& toCatch) {
    // Do your failure processing here
    ABORT();
  }
  return;
}

void XmlFinalize(void)
{
  xa::XalanTransformer::terminate();
  xe::XMLPlatformUtils::Terminate();
}

string XmlStr2Entity( string str )
{
    str = REPLACE( str,"&","&amp;" );
    str = REPLACE( str,"<","&lt;" );
    str = REPLACE( str,">","&gt;" );
    str = REPLACE( str,"\"","&quot;" );
    str = REPLACE( str,"\'","&apos;" );
    return str;
}
}
