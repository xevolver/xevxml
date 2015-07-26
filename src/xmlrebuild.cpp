/**
 * @file     xmlrebuild.cpp
 * @brief    A command to rebuild an XML document of an AST.
 * \license This project is released under the BSD 2-clause license
 *
 * Copyright (C) 2010-2015 Hiroyuki TAKIZAWA. All rights reserved.
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
#include "common.hpp"
#include "rose2xml.hpp"
#include "xml2rose.hpp"
#include "xmlparser.hpp"

#include <vector>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
using namespace XevXml;
namespace xe=xercesc;

static const char* findFilename(xe::DOMNode* node) {
  const char* filename=0;
  if(node->getNodeType() != xe::DOMNode::DOCUMENT_NODE ){
    if(node->getNodeType() == xe::DOMNode::ELEMENT_NODE){
      char* buf = xe::XMLString::transcode(node->getNodeName());
      string nname = buf;
      xe::XMLString::release(&buf);
      if(nname=="SgSourceFile"){
	std::string fn;
	XmlGetAttributeValue(node,"file",&fn);
	if(fn.size()>0)
	  return fn.c_str();
	else{
	  XEV_WARN("The SgSourceFile element does not have a \"file\" attribute");
	  XEV_ABORT();
	}
      }
    }
    else
      return NULL;
  }

  xercesc::DOMNode* cld = node->getFirstChild();
  while(cld) {
    filename = findFilename(cld);
    if(filename != NULL ) return filename;
  }
  return NULL;
}


void removeTmpFile(int, void* name)
{
  unlink((const char*)name);
}


char* convertXml2TmpFile(void)
{
  int          pid    = 0;
  int          status = 0;
  char*        tmpl   = NULL;
  size_t       fnlen  = 0;
  XevXmlDocument* doc = 0;
  std::string  tmpfn;
  XevXmlParser parser;

  parser.read(cin);
  doc = parser.getDocument();

  if(doc==NULL){
    XEV_FATAL("XML parse failed");
  }

  // decide the name of a temporal file
  const char* fn = findFilename(doc);
  if(fn!=NULL)
    tmpfn = fn;
  else {
    XEV_FATAL("cannot find a filename in the XML document");
  }

  fnlen = tmpfn.size();
  tmpfn = "/tmp/.XXXXXX-"+tmpfn;
  tmpl = new char[tmpfn.size()+1];
  memcpy(tmpl,tmpfn.c_str(),tmpfn.size()+1);

  // create a temporal file
  int fd = mkstemps(tmpl,fnlen+1); // '-' + (the filename length)
  if(fd<0) {
    XEV_FATAL("cannot create a temporal file: " << tmpl << " (" << strerror(errno) << ")");
  }
  else{
    XEV_INFO(tmpl << " is created");
  }

  if ((pid = fork()) == 0) {
    XevXmlVisitor visitor;
    SgProject*    prj=NULL;

    visitor.visit(doc,0);
    prj = visitor.getSgProject();
    if(prj==NULL){
      XEV_FATAL("XML-to-AST conversion failed");
    }

    // unparse the code and write it to the temporal file
    std::ofstream os(tmpl,ios::out);
    if(!os){
      XEV_FATAL("cannot open a temporal file: " << tmpl);
    }
    if(XevXml::XevUnparseToStream(os,&prj)==false){
      XEV_FATAL("AST unparse failed");
    }
    else {
      XEV_INFO("AST unparsed successfully");
    }
    exit(0);
  }
  else {
    if (pid != -1) {
      wait(&status);
    }
    else {
      XEV_FATAL("fork failed");
    }
  }

  // remove the temporal file at exit
  //on_exit(removeTmpFile,tmpl);
#if 0
  FILE *fp = fopen(tmpl,"r");
  if(fp!=NULL){
    int c = fgetc(fp);
    while(c!=EOF){
      putchar(c);
      c = fgetc(fp);
    }
  }
  else{
    XEV_FATAL("cannot read a temporal file: " << tmpl);
  }
#endif

  return tmpl;
}

bool isFilenameGiven(const vector<string>& args){
  for(size_t i(0);i<args.size();i++){
    /* wild guess */
    if(i>0 && args[i][0]!='-'){
      XEV_INFO( "\"" << args[i] << "\" may not be a command line option");
      return true;
    }
  }

  return false;
}
