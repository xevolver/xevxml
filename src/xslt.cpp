/**
 * @file     xslt.cpp
 * @brief    A command to apply XSLT rules to XML documents.
 * \license This project is released under the BSD 2-clause license
 *
 * Copyright (C) 2010-2013 Hiroyuki TAKIZAWA. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *    
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "xslt.hpp"
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
      XEV_ABORT();
    }
  }
  catch(...){
    cerr << "Error: XalanTransformer::transform() failed" << endl;
    XEV_ABORT();
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

  XevXML::XmlInitialize();

  //while((c=cin.get()) != cin.eof()){
  while(cin.get(c)){
    istr << c;
  }

  XsltTransform(istr,ostr,fn);

  cout << ostr.str() << flush;

  XevXML::XmlFinalize();  
  return 0;
}

