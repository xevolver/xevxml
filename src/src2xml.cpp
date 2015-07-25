/**
 * @file     src2xml.cpp
 * @brief    A command to convert a code to an XML AST document
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
#include "rose2xml.hpp"
#include "xml2rose.hpp"

#include <getopt.h>
#include <vector>
#include <string>

using namespace std;

#ifdef XEV_COMPILE4XMLREBUILD
extern char* convertXml2TmpFile(void);
extern bool  isFilenameGiven(const vector<string>& args);
#endif

static  struct option long_opts[]={
  {"check_fortran_pragma",  1, NULL, 'F'},
  {"help",                  0, NULL, 'h'},
  {0,0,0,0}
};
bool fortran_pragma=true;

void ProcessOpts(int argc,char** argv)
{
  // See manpage of getopt
  int c;
  int digit_optind = 0;


  while(1){
    int option_index = 0;
    c = getopt_long(argc,argv,"F:h",long_opts,&option_index);
    if(c==-1){
      break;
    }
    switch(c){
    case'F':
      if( string("true") == optarg )
        fortran_pragma = true;
      else if ( string("false") == optarg )
        fortran_pragma = false;
      else
        fortran_pragma = atoi(optarg);
      break;
    case 'h':
      cerr << "USAGE:" << argv[0] << " [OPTION]... FILENAME " << endl;
      cerr << "OPTIONS:" << endl;
      cerr << "-F, --check_fortran_pragma <bool>\t Enable Fortran pragma support (default:true)\n";
      cerr << "-h, --help                       \t Print this message\n";
      exit(0);
      break;
    case ':':
    case '?':
      ; /* do nothing */
    }
  }
}

int main(int argc, char** argv)
{
  int             fd=0;
  stringstream    xmlString1;
  SgProject*      sageProject=0;
  //xevxml::Ast2XmlOpt opt;
  vector<string> args;

  XevXml::XmlInitialize();

  ProcessOpts(argc,argv);
  for(int id(0);id<argc;++id)
    args.push_back( string(argv[id]) );
  args.push_back( string("-rose:skip_syntax_check")); // some Fortran codes need this

#ifdef XEV_COMPILE4XMLREBUILD
  if(isFilenameGiven(args)==true){
    XEV_WARN("xmlrebuild read an XML file from standard input");
    XEV_ABORT();
  }
  char* tmpl = convertXml2TmpFile();
  args.push_back( string(tmpl)); // name of the temporal file created above
#endif

  fd = dup(fileno(stdout));
  dup2(fileno(stderr),fileno(stdout)); // printf messages are written to stderr
  sageProject = frontend(args); // build an ROSE AST from a code
  //SgProject::set_verbose(10);

#ifdef XEV_USE_ROSEHPCT
  if(opt.rosehpct)
    RoseHPCT::attachMetrics (opt.profiles,
                             sageProject, sageProject->get_verbose () > 0);
#endif

  XevXml::XevXmlOption opt;
  opt.getFortranPragmaFlag() = fortran_pragma;
  //opt.getSkipCompilerGeneratedFlag() = true;
  opt.getSkipCompilerGeneratedFlag() = false;
  fflush(stdout);
  dup2(fd,1); // printf messages are written to stdout
  clearerr(stdout);
  XevXml::XevConvertRoseToXml(cout,&sageProject,&opt);

  XevXml::XmlFinalize();
  return 0;
}
