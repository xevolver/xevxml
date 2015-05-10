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
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"
#include "xercesc/sax/EntityResolver.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XMLEntityResolver.hpp"
#include "xercesc/util/XMLResourceIdentifier.hpp"
#include "xercesc/util/XMLException.hpp"

#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xercesc/util/XMLString.hpp>

#include <sstream>
#include <fstream>
#include <stdexcept>
#include <getopt.h>

const char* DEFAULT_LIBRARY_PATH=XEVXML_PREFIX"/lib/xev-trans";
const char* XEVXML_LIB_ENV="XEVXML_LIBRARY_PATH";

using namespace std;
namespace xe=xercesc;
namespace xa=xalanc;

string libdir=DEFAULT_LIBRARY_PATH;

static std::string GetPath(const XMLCh* systemId, std::string xsltfn)
{
  std::string abs = xe::XMLString::transcode(systemId);
  if(strncmp(abs.c_str(),"file:///",8) != 0)
    return abs.c_str();
  std::ifstream ifs(abs.c_str()+7);
  if(ifs) {
    ifs.close();
    return abs.c_str()+7; //remove "file://"
  }


  std::string::size_type n(0),m(0);

  // get the directory in which the XSLT file exists.
  std::string base;
  while((n=xsltfn.find("/",n))!=std::string::npos){
    base = xsltfn.substr(0,n);
    n++;
  }

  // remove "file://"
  abs = abs.c_str()+7;

  // compare the XSLT directory and the systemId.
  n=0;m=0;
  string relpath;
  while((n=abs.find("/",n))!=std::string::npos
        && (m=base.find("/",m))!=std::string::npos){
    relpath =  abs.substr(n);
    if(abs.substr(0,n) !=base.substr(0,m)) break;
    n++;m++;
  }
  if(base.substr(0,m).size()==base.size() )
    return libdir+abs.substr(n);

  int up=1;
  m++;
  while((m=base.find("/",m))!=std::string::npos){
    up++;m++;
  }
  for(int i(0);i<up;i++){
    relpath = "/../" + relpath;
  }
  //cerr << relpath <<endl;
  return libdir+relpath;
}

class MyEntityResolver: public xe::EntityResolver, public xe::XMLEntityResolver
{
  std::string xsltfn_;
public:
  MyEntityResolver(std::string& xsltfn):xsltfn_(xsltfn.c_str()) {
    char* apath=NULL;
    apath = realpath(xsltfn_.c_str(),NULL);
    if(apath==NULL) {
      XEV_WARN("cannot find the absolute path of the XSLT file");
      XEV_ABORT();
    }
    xsltfn_ = apath;
    free(apath);
  }
  ~MyEntityResolver(){}

  virtual xe::InputSource*
  resolveEntity(
                const XMLCh* const  p/* publicId */,
                const XMLCh* const  systemId)
  {
    std::string uri = GetPath(systemId,xsltfn_);
    //return xe::EntityResolver::resolveEntity(p,systemId);
    xe::InputSource* src = 0;
    //cerr << "pwd=" <<getenv("PWD") <<endl;
    //cerr << "xslt="<<xsltfn_ <<endl;
    //cerr << "sysid="<<xe::XMLString::transcode(systemId) << endl;
    //cerr << xe::XMLString::transcode(resourceIdentifier->getSchemaLocation()) << endl;
    //cerr <<  "uri="<<uri <<endl;
    //cerr << "libdir="<<libdir<<endl;
    src = new xe::LocalFileInputSource(xe::XMLString::transcode(uri.c_str()));

    return src;
  }
  virtual xe::InputSource*
  resolveEntity(xe::XMLResourceIdentifier*    resourceIdentifier)

  {
    //return xe::EntityResolver::resolveEntity(p,systemId);
    if (resourceIdentifier->getResourceIdentifierType()
            == xe::XMLResourceIdentifier::ExternalEntity){
      std::string uri = GetPath(resourceIdentifier->getSystemId(),xsltfn_);
      xe::InputSource* src = 0;
      //cerr << "pwd=" <<getenv("PWD") <<endl;
      //cerr << "xslt="<<xsltfn_ <<endl;
      //cerr << "sysid="<<xe::XMLString::transcode(resourceIdentifier->getSystemId()) << endl;
      //cerr << xe::XMLString::transcode(resourceIdentifier->getSchemaLocation()) << endl;
      //cerr <<  "uri="<<uri <<endl;
      //cerr << "libdir="<<libdir<<endl;
      src = new xe::LocalFileInputSource(xe::XMLString::transcode(uri.c_str()));

      return src;
    }

    return 0;
  }
};


void XsltTransform(stringstream& istr, stringstream& ostr, string xsltfn)
{
  // work with Xalan-C++
  try {
    //xalanc::XSLTInputSource xmlIn(str);
    xa::XalanTransformer transformer;
    transformer.setEntityResolver(new MyEntityResolver(xsltfn));
    if( 0 != transformer.transform(istr,xsltfn.c_str(),ostr)){
      XEV_WARN( transformer.getLastError());
      XEV_ABORT();
    }
  }
  catch(xe::XMLException& e){
    char* message = xe::XMLString::transcode( e.getMessage() );
    XEV_WARN( message );
    xe::XMLString::release( &message );
    XEV_ABORT();
  }
  catch(std::exception& e){
    XEV_WARN( "XalanTransformer::transform() failed" );
    XEV_WARN( e.what() );
    XEV_ABORT();
  }
  catch(...){
    XEV_WARN( "XalanTransformer::transform() failed due to an unknown error" );
    XEV_ABORT();
  }
}

static  struct option long_opts[]={
  {"libdir",  1, NULL, 'L'},
  {"help",    0, NULL, 'h'},
  {0,0,0,0}
};

void ProcessOpts(int argc,char** argv, string& xslt)
{
  // See manpage of getopt
  int c;
  int digit_optind = 0;
  const char* env = getenv(XEVXML_LIB_ENV);
  if(env!=NULL){
    libdir = env;
  }
  while(1){
    int option_index = 0;
    c = getopt_long(argc,argv,"L:h",long_opts,&option_index);
    if(c==-1){
      if(optind < argc){
        xslt = argv[optind];
      }
      break;
    }
    switch(c){
    case'L':
      libdir = optarg;
      break;
    case 'h':
      cerr << "USAGE:" << argv[0] << " [OPTION]... FILENAME " << endl;
      cerr << "OPTIONS:" << endl;
      cerr << "-L, --libdir <path>\t Specify the library path\n";
      cerr << "-h, --help         \t Print this message\n";
      //cerr << libdir;
      exit(0);
      break;
    case ':':
    case '?':
      exit(-1);
    }
  }
}

int main(int argc,char** argv)
{
  stringstream istr;
  stringstream ostr;
  char c;
  string fn;
  ProcessOpts(argc,argv,fn);
  if( argc < 2 || fn.size()==0) {
    cerr << "Try option `--help' for more information" <<endl;
    return -1;
  }

  XevXml::XmlInitialize();

  //while((c=cin.get()) != cin.eof()){
  while(cin.get(c)){
    istr << c;
  }

  XsltTransform(istr,ostr,fn);

  cout << ostr.str() << flush;

  XevXml::XmlFinalize();
  return 0;
}
