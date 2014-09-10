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
#include "ast2xml.hpp"
#include "xml2ast.hpp"

#include <getopt.h>
#include <vector>
#include <string>

using namespace std;

static vector<string> cmdopt(int argc, char** argv, xevxml::Ast2XmlOpt* opt)
{
  int c;
  int digit_optind = 0;
  vector<string> args;
  
  args.push_back(argv[0]);
#ifndef XEV_USE_ROSEHPCT
  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"xev:verbose", required_argument, 0,  0 },
      {"xev:address", required_argument, 0,  0 },
      {"xev:rosehpct", required_argument, 0,  0 },
      {0,         0,                 0,  0 }
    };
    
    c = getopt_long(argc, argv, "",
		    long_options, &option_index);
    if (c == -1) {
      break;
    }

    switch (c) {
    case 0:
      cerr << "option " << long_options[option_index].name << endl;
      if (optarg) {
	cerr << " with arg " <<  optarg;
	if(option_index==0)
	  SgProject::set_verbose(atoi(optarg));
	if(option_index==1)
	  opt->address = atoi(optarg);
	if(option_index==2)
	  opt->rosehpct = atoi(optarg);
      }
      cerr << "\n";
      break;

    case '?':
      break;
      
    default:
      cerr << "?? getopt returned character code " << c << endl;
    }
  }

  for(int i(optind);i<argc;i++)
    args.push_back(argv[i]);
#else
  opt->rosehpct=1;
  for(int i(1);i<argc;i++)
    args.push_back(argv[i]);
  if( opt->rosehpct )
    opt->profiles = RoseHPCT::loadProfilingFiles(args);
#endif
  return args;
}


int main(int argc, char** argv)
{
  int             fd=0;
  stringstream    xmlString1;
  SgProject*      sageProject=0;
  SgFile*         file=0;
  xevxml::Ast2XmlOpt opt;

  fd = dup(1); 
  dup2(2,1); // printf messages are written to stderr  
  //SgProject::set_verbose(10);
  sageProject = frontend(cmdopt(argc,argv,&opt));
  dup2(fd,1); // printf messages are written to stdout  
  file = &sageProject->get_file(sageProject->numberOfFiles()-1);

#ifdef XEV_USE_ROSEHPCT
  if(opt.rosehpct)
    RoseHPCT::attachMetrics (opt.profiles, 
			     sageProject, sageProject->get_verbose () > 0);
#endif
  xevxml::XmlInitialize();
  xevxml::Ast2Xml(xmlString1,file,&opt);

  cout << xmlString1.str();

  /*
  char c;
  while((c=cin.get()) != cin.eof()){
    xmlString1 << c;
  }

  xevxml::Xml2Ast(xmlString1,sageProject,"dummy.c");
  sageProject->unparse();
  */
  xevxml::XmlFinalize();
  return 0;
}
