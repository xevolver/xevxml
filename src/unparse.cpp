/**
 * @file     unparse.cpp
 * @brief    Unparse an XML document to generate a code.
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
#include "unparse.hpp"

using namespace std;

static bool myUnparser(ostream* os, SgFile* file, UnparseFormatHelp* unparseHelp = NULL,
              UnparseDelegate *repl  = NULL, SgScopeStatement* unparseScope = NULL )
{
  bool UseAutoKeyword                = false;
  // bool linefile                      = false;
  bool generateLineDirectives        = file->get_unparse_line_directives();

  bool useOverloadedOperators        = false;
  // bool useOverloadedOperators        = true;

  bool num                           = false;

  // It is an error to have this always turned off (e.g. pointer = this; will not unparse correctly)
  bool _this                         = true;

  bool caststring                    = false;
  bool _debug                        = false;
  bool _class                        = false;
  bool _forced_transformation_format = false;

  // control unparsing of include files into the source file (default is false)
  bool _unparse_includes             = file->get_unparse_includes();

  Unparser_Opt roseOptions( UseAutoKeyword,
                            generateLineDirectives,
                            useOverloadedOperators,
                            num,
                            _this,
                            caststring,
                            _debug,
                            _class,
                            _forced_transformation_format,
                            _unparse_includes );

  Unparser roseUnparser (os, file->get_file_info()->get_filenameString(),roseOptions,unparseHelp,repl);

  roseUnparser.set_embedColorCodesInGeneratedCode ( file->get_embedColorCodesInGeneratedCode() );
  roseUnparser.set_generateSourcePositionCodes    ( file->get_generateSourcePositionCodes() );

  SgUnparse_Info inheritedAttributeInfo;

  SgSourceFile* sourceFile = isSgSourceFile(file);
  if(sourceFile)
    roseUnparser.unparseFile(sourceFile,inheritedAttributeInfo, unparseScope);
  else
    return false;

  return true;
}

bool XevXml::XevUnparseToStream(std::ostream& os, SgProject** prj, UnparseFormatHelp* unparseHelp,
                        UnparseDelegate *repl, SgScopeStatement* unparseScope )
{
  return myUnparser(&os, &(*prj)->get_file(0),unparseHelp,repl,unparseScope);
}


/*
bool UnparseSgFile(SgFile* file, const char* filename, UnparseFormatHelp* unparseHelp,
                     UnparseDelegate *repl, SgScopeStatement* unparseScope )
{
  if(file==NULL) return false;
  if(filename!=NULL)
    file->set_unparse_output_filename(filename);
  if (file->get_skip_unparse()==true) return false;
  if (file->get_unparse_output_filename().empty() == true) {
    file->set_unparse_output_filename("-");
  }
  string outputFilename = file->get_unparse_output_filename();

  if (file->get_skip_unparse() == false) {
    if(outputFilename!="-") {
      fstream os(outputFilename.c_str(),ios::out);
      if (!os) {
        printf ("Error detected in opening file %s for output \n",outputFilename.c_str());
        return false;
      }
      bool ret = myUnparser(&os,file,unparseHelp,repl,unparseScope);
      os.close();
      return ret;
    }
    else
      return myUnparser(&cout, file,unparseHelp,repl,unparseScope);
  }
  return false;
}
*/
