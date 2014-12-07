/**
 * @file     xml2src.cpp
 * @brief    A command to convert an XML document to a code.
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
#include "common.hpp"
#include "xml2rose.hpp"
#include "unparse.hpp"

using namespace std;

//! Build a SgFile node
SgSourceFile*
buildEmptySourceFile(const string& fn, SgProject* project=0)
{
  // This is a more direct, alternative implementation (not sure if it is better).
  SgSourceFile* newFile = new SgSourceFile();
  ROSE_ASSERT(newFile != NULL);

  // Mark as a C file for now.
  newFile->set_Fortran_only(true);

  // Specify the name of the file (and line and column numbers), using a Sg_File_Info object.
  Sg_File_Info* info
    = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
  //Sg_File_Info* fileInfo = new Sg_File_Info(outputFileName,0,0);
  ROSE_ASSERT(info != NULL);

  newFile->set_startOfConstruct(info);
  info->set_parent(newFile);

  SgGlobal* globalScope = new SgGlobal();
  ROSE_ASSERT(globalScope != NULL);

  newFile->set_globalScope(globalScope);
  globalScope->set_parent(newFile);

  ROSE_ASSERT(newFile->get_globalScope() != NULL);


  project = new SgProject();
  ROSE_ASSERT(project);
  project->get_fileList().clear();
  Rose_STL_Container<std::string> arglist;
  //int nextErrorCode = 0;

  arglist.push_back("cc");
  arglist.push_back("-c");
  arglist.push_back("dummy");
  arglist.push_back("-rose:o");
  arglist.push_back(fn);
  arglist.push_back("-rose:verbose 2");
  project->set_originalCommandLineArgumentList (arglist);
  project->get_sourceFileNameList().push_back("dummy");

  newFile->set_parent(project);
  SgFilePtrList& flist = project->get_fileList();
  flist.insert(flist.begin(),newFile);
  if (newFile->get_globalMangledNameMap().size() != 0)
    newFile->clearGlobalMangledNameMap();

  return newFile;
}

int main(int argc, char** argv)
{
  SgProject* prj = 0;
  bool r = false;

  XevXml::XevInitialize();
  if( XevXml::XevConvertXmlToRose(cin,&prj) == false ){
    XEV_ABORT();
  }
  else {
    /* prtine all symbol tables  for debugging */
    //XevXML::PrintSymTable test;
    //test.visit(&prj->get_file(0));

    if( argc < 2 )
      r = XevXml::XevUnparseToStream(cout,&prj);
    else {
      fstream os(argv[1],ios::out);
      if (!os) {
        printf ("Error detected in opening file %s for output \n",argv[1]);
        return -1;
      }
      r = XevXml::XevUnparseToStream(os,&prj);
      os.close();
    }
  }
  XevXml::XevFinalize();
  return r?0:(-1);
}
