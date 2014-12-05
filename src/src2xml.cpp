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

int main(int argc, char** argv)
{
  int             fd=0;
  stringstream    xmlString1;
  SgProject*      sageProject=0;
  //xevxml::Ast2XmlOpt opt;
  vector<string> args;

  for(int id(0);id<argc;++id)
    args.push_back( string(argv[id]) );
  args.push_back( string("-rose:skip_syntax_check")); // some Fortran codes need this

  fd = dup(fileno(stdout));
  dup2(fileno(stderr),fileno(stdout)); // printf messages are written to stderr
  sageProject = frontend(args); // build an ROSE AST from a code
  //SgProject::set_verbose(10);

#ifdef XEV_USE_ROSEHPCT
  if(opt.rosehpct)
    RoseHPCT::attachMetrics (opt.profiles,
                             sageProject, sageProject->get_verbose () > 0);
#endif
  XevXml::XmlInitialize();
  XevXml::XevXmlOption opt;
  opt.getFortranPragmaFlag() = true;
  fflush(stdout);
  dup2(fd,1); // printf messages are written to stdout
  clearerr(stdout);
  XevXml::XevConvertRoseToXml(cout,&sageProject,&opt);

  XevXml::XmlFinalize();
  return 0;
}
