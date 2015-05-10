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
#include <vector>
#include <getopt.h>

using namespace std;
namespace xe=xercesc;
namespace xa=xalanc;

class MyEntityResolver: public xe::EntityResolver, public xe::XMLEntityResolver
{
  std::string xsltfn_;
  std::string xsltloc_;
  vector<string> libdirs_;


  /*
    systemId has the absolute path to the file being accessed.
  */
  std::string GetPath(const XMLCh* systemId)
  {
    char* buf = xe::XMLString::transcode(systemId);
    std::string abs = buf;
    xe::XMLString::release(&buf);
    if(strncmp(abs.c_str(),"file:///",8) != 0)
      // this is not a local file. don't change it.
      return abs.c_str();

    std::ifstream ifs(abs.c_str()+7);
    // the file exists at the location given by the absolute path.
    if(ifs) {
      ifs.close();
      return abs.c_str()+7; //remove "file://"
    }

    std::string::size_type n(0),m(0);

    // remove "file://"
    abs = abs.c_str()+7;

    // compare the XSLT directory and the systemId.
    string relpath;
    while((n=abs.find("/",n))!=std::string::npos
          && (m=xsltloc_.find("/",m))!=std::string::npos){
      relpath =  abs.substr(n);
      if(abs.substr(0,n) !=xsltloc_.substr(0,m)) break;
      n++;m++;
    }
    if(xsltloc_.substr(0,m).size()==xsltloc_.size() ){
      /* it will be located in the subdirectories */
      for(size_t i(0);i<libdirs_.size();i++){
        string fn = libdirs_[i]+abs.substr(n);
        ifs.open(fn.c_str(),ios::in);
        if(ifs){ // found!
          ifs.close();
          return fn;
        }
      }
      return NULL;
    }

    int up=1;
    m++;
    while((m=xsltloc_.find("/",m))!=std::string::npos){
      up++;m++;
    }
    for(int i(0);i<up;i++){
      relpath = "/../" + relpath;
    }
    //cerr << relpath <<endl;
    for(size_t i(0);i<libdirs_.size();i++){
      string fn = libdirs_[i]+abs.substr(n);
      ifs.open(fn.c_str(), ios::in);
      if(ifs){ // found!
        ifs.close();
        return fn;
      }
    }
    return NULL;
  }

public:
  MyEntityResolver(std::string& xsltfn, std::string& libdirs):xsltfn_(xsltfn.c_str()) {
    char* apath=NULL;
    apath = realpath(xsltfn_.c_str(),NULL);
    if(apath==NULL) {
      XEV_WARN("cannot find the absolute path of the XSLT file");
      XEV_ABORT();
    }
    xsltfn_ = apath;
    free(apath);

    string::size_type n(0),prev(0);
    // get the directory in which the XSLT file exists.
    while((n=xsltfn_.find("/",n))!=std::string::npos){
      xsltloc_ = xsltfn_.substr(prev,n);
      n++;
    }
    cerr << "libdirs=" << libdirs << endl;
    if(libdirs.size()>0){
      n=0;
      string dir;
      while((n=libdirs.find(":",n))!=std::string::npos){
        dir = libdirs.substr(prev,n);
        if(dir.size()>0)
          libdirs_.push_back(dir);
        prev = n;
        n++;
      }
      dir = libdirs.substr(prev);
      if(dir.size()>0)
        libdirs_.push_back(dir);
    }
    if(libdirs_.size() == 0)
      libdirs_.push_back(XEVXML_DEFAULT_LIBRARY_PATH);

    for(size_t i(0);i<libdirs_.size();i++)
      cerr << libdirs_[i] << endl;
  }
  ~MyEntityResolver(){}

  virtual xe::InputSource*
  resolveEntity(
                const XMLCh* const  p/* publicId */,
                const XMLCh* const  systemId)
  {
    std::string uri = GetPath(systemId);
    xe::InputSource* src = 0;

    src = new xe::LocalFileInputSource(xe::XMLString::transcode(uri.c_str()));
    return src;
  }
  virtual xe::InputSource*
  resolveEntity(xe::XMLResourceIdentifier*    resourceIdentifier)

  {
    if (resourceIdentifier->getResourceIdentifierType()
        == xe::XMLResourceIdentifier::ExternalEntity){
      std::string uri = GetPath(resourceIdentifier->getSystemId());
      xe::InputSource* src = 0;

      src = new xe::LocalFileInputSource(xe::XMLString::transcode(uri.c_str()));
      return src;
    }

    return 0;
  }
};


void XsltTransform(stringstream& istr, stringstream& ostr, string xsltfn, string libdirs)
{
  // work with Xalan-C++
  try {
    //xalanc::XSLTInputSource xmlIn(str);
    xa::XalanTransformer transformer;
    transformer.setEntityResolver(new MyEntityResolver(xsltfn,libdirs));
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

void ProcessOpts(int argc,char** argv, string& xslt, string& libdirs)
{
  // See manpage of getopt
  int c;
  int digit_optind = 0;
  const char* env = getenv(XEVXML_LIB_ENV);
  if(env!=NULL){
    libdirs = env;
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
      libdirs = optarg;
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
  string libdirs=XEVXML_DEFAULT_LIBRARY_PATH;

  ProcessOpts(argc,argv,fn,libdirs);
  if( argc < 2 || fn.size()==0) {
    cerr << "Try option `--help' for more information" <<endl;
    return -1;
  }

  XevXml::XmlInitialize();

  //while((c=cin.get()) != cin.eof()){
  while(cin.get(c)){
    istr << c;
  }

  XsltTransform(istr,ostr,fn,libdirs);

  cout << ostr.str() << flush;

  XevXml::XmlFinalize();
  return 0;
}
