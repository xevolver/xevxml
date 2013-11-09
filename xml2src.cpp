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
#include "xevxml.hpp"
#include "xml2ast.hpp"

using namespace std;

int main(int argc, char** argv)
{
  ofstream ofs(argv[1]);
  SgProject* sageProject=frontend(argc,argv);
  //SgProject* sageProject=frontend(argc,argv);
  char c;
  /*
  if( argc < 2 ) {
    cerr << "USAGE:" << argv[0] << " [output file name] " << endl;
    return -1;
  }
  */
  string fn(argv[1]);
  stringstream istr;

  while(cin.get(c)){
    istr << c;
  }
  //cerr << fn << ":" << sageProject->get_Fortran_only() << endl;
  xevxml::XmlInitialize();
  
  SgFile* file = xevxml::Xml2Ast(istr,sageProject);
  //AstPostProcessing(file);

  //cerr << "Writing to " << file->getFileName() << endl;
  //file->unparse();
  //std::cout << xmlString1.str();
  ofs << file->unparseToCompleteString()  << endl;
  xevxml::XmlFinalize();
  //cerr << "Creating " << sageProject->get_file(sageProject->numberOfFiles()-1).getFileName() << endl;
  //sageProject->get_file(sageProject->numberOfFiles()-1).unparse();
  //sageProject->unparse();
  return 0;
}
