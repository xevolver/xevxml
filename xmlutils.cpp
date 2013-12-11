/**
 * @file     xmlutils.cpp
 * @brief    Utility functions for handling XML data
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

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>

namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;

namespace xevxml {

static string REPLACE( string str1, string str2, string str3 )
{
  string::size_type  Pos( str1.find( str2 ) );

  while( Pos != string::npos ){
    str1.replace( Pos, str2.length(), str3 );
    Pos = str1.find( str2, Pos + str3.length() );
  }
  
  return str1;
}

void XmlInitialize(void)
{
  try {
    xe::XMLPlatformUtils::Initialize();
    xa::XalanTransformer::initialize();
  }
  catch (const xe::XMLException& toCatch) {
    // Do your failure processing here
    ABORT();
  }
  return;
}

void XmlFinalize(void)
{
  xa::XalanTransformer::terminate();
  xe::XMLPlatformUtils::Terminate();
}

string XmlStr2Entity( string str )
{
    str = REPLACE( str,"&","&amp;" );
    str = REPLACE( str,"<","&lt;" );
    str = REPLACE( str,">","&gt;" );
    str = REPLACE( str,"\"","&quot;" );
    str = REPLACE( str,"\'","&apos;" );
    return str;
}

string XmlEntity2Str( string str )
{
  str = REPLACE( str,"&amp;","&" );
  str = REPLACE( str,"&lt;","<" );
  str = REPLACE( str,"&gt;",">" );
  str = REPLACE( str,"&quot;","\"" );
  str = REPLACE( str,"&apos;","\'" );
    return str;
}

bool XmlWriteToString( xe::DOMNode* node, std::stringstream& str )
{
  XMLCh tempStr[100];
  xe::XMLString::transcode("LS", tempStr, 99);
  xe::DOMImplementation *impl = xe::DOMImplementationRegistry::getDOMImplementation(tempStr);
  xe::DOMLSSerializer *ser = ((xe::DOMImplementationLS*)impl)->createLSSerializer();
  if (ser-> getDomConfig()-> canSetParameter(xe::XMLUni::fgDOMWRTFormatPrettyPrint, true)){
    ser-> getDomConfig()-> setParameter(xe::XMLUni::fgDOMWRTFormatPrettyPrint, true);
  }
  ser-> setNewLine(xe::XMLString::transcode("\r\n"));

  //XMLFormatTarget *formatTarget = new LocalFileFormatTarget( tempFilePath );
  //xe::XMLFormatTarget *formatTarget = new xe::StdOutFormatTarget();
  xe::MemBufFormatTarget *formatTarget = new xe::MemBufFormatTarget();
  xe::DOMLSOutput *output = ((xe::DOMImplementationLS*)impl)-> createLSOutput();
  output-> setByteStream(formatTarget);
  ser->write(node,output);
  
  char* s = (char*)formatTarget->getRawBuffer();
  str << s;
  ser->release();
  delete formatTarget;
  output->release();
  return true;
} 

}
