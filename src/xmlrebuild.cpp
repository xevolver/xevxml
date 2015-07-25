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

#include <vector>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

void removeTmpFile(int, void* name)
{
  unlink((const char*)name);
}

char* convertXml2TmpFile(void)
{
  SgProject* prj   = 0;
  char*      tmpl  = 0;
  size_t     fnlen = 0;

  std::string tmpfn;

  // convert an XML AST to a source code
  if( XevXml::XevConvertXmlToRose(std::cin,&prj) == true ){
    SgSourceFile* src = isSgSourceFile(&prj->get_file(0));
    tmpfn = src->get_sourceFileNameWithoutPath();
    fnlen = tmpfn.size();
  }
  else {
    XEV_WARN("Conversion from XML to Source failed");
    XEV_ABORT();
  }

  // decide the name of a temporal file
  tmpfn = "/tmp/.XXXXXX-"+tmpfn;
  tmpl = new char[tmpfn.size()+1];
  memcpy(tmpl,tmpfn.c_str(),tmpfn.size()+1);

  // create a temporal file
  int fd = mkstemps(tmpl,fnlen+1); // '-' + (the filename length)
  if(fd<0) {
    XEV_WARN("Cannot create a temporal file: " << tmpl << " (" << strerror(errno) << ")");
    XEV_ABORT();
  }
  else
    std::cerr << tmpl << " is created" << std::endl;

  // remove the temporal file at exit
  on_exit(removeTmpFile,tmpl);

  // unparse the code and write it to the temporal file
  std::ofstream os(tmpl,ios::out);
  if(XevXml::XevUnparseToStream(os,&prj)==false){
    XEV_WARN("Cannot open a temporal file: " << tmpl);
    XEV_ABORT();
  }

  // the project is no longer required
  delete prj;

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
    XEV_WARN("Cannot read a temporal file: " << tmpl);
    XEV_ABORT();
  }
#endif

  return tmpl;
}

bool isFilenameGiven(const vector<string>& args){
  /* not implemented yet*/
  return false;
}
